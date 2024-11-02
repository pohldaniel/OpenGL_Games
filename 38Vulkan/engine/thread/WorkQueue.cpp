#include "../thread/Thread.h"
#include "WorkQueue.h"

WorkQueue* WorkQueue::Instance = nullptr;
thread_local unsigned WorkQueue::ThreadIndex = 0;

Task::Task(){
    numDependencies.store(0);
}

Task::~Task(){
}

WorkQueue::WorkQueue(unsigned numThreads) : m_shouldExit(false){

	m_numQueuedTasks.store(0);
	m_numPendingTasks.store(0);

    if (numThreads == 0){
        numThreads = ThreadUtil::CPUCount();
        // Avoid completely excessive core count
        if (numThreads > 16)
            numThreads = 16;
    }

    for (unsigned  i = 0; i < numThreads - 1; ++i)
		m_threads.push_back(std::thread(&WorkQueue::workerLoop, this, i + 1));
}

WorkQueue::~WorkQueue(){

    if (!m_threads.size())
        return;

    // Signal exit and wait for threads to finish
	m_shouldExit = true;

	m_signal.notify_all();
    for (auto it = m_threads.begin(); it != m_threads.end(); ++it)
        it->join();
}

void WorkQueue::queueTask(Task* task){

    if(m_threads.size()){
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
			m_tasks.push(task);
        }

		m_numQueuedTasks.fetch_add(1);
		m_numPendingTasks.fetch_add(1);

		m_signal.notify_one();

    }else{
        // If no threads, execute directly
        completeTask(task, 0);
    }
}

void WorkQueue::queueTasks(size_t count, Task** tasks_){
    if (m_threads.size()){
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            for (size_t i = 0; i < count; ++i){
				m_tasks.push(tasks_[i]);
            }
        }

		m_numQueuedTasks.fetch_add((int)count);
		m_numPendingTasks.fetch_add((int)count);

        if (count >= m_threads.size())
			m_signal.notify_all();
        else{
            for (size_t i = 0; i < count; ++i)
				m_signal.notify_one();
        }
    }else{
        // If no threads, execute directly
        for (size_t i = 0; i < count; ++i){
            completeTask(tasks_[i], 0);
        }
    }
}

void WorkQueue::addDependency(Task* task, Task* dependency){
    dependency->dependentTasks.push_back(task);

    // If this is the first dependency added, increment the global pending task counter so we know to wait for the dependent task in Complete().
    if (task->numDependencies.fetch_add(1) == 0)
		m_numPendingTasks.fetch_add(1);
}

void WorkQueue::complete(){
    
    if (!m_threads.size())
        return;

    for (;;){
        if (!m_numPendingTasks.load())
            break;

        // Avoid locking the queue mutex if do not have tasks in queue, just wait for the workers to finish
        if (!m_numQueuedTasks.load())
            continue;

        // Otherwise if have still tasks, execute them in the main thread
        Task* task;
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            if (!m_tasks.size())
                continue;

            task = m_tasks.front();
			m_tasks.pop();
        }

		m_numQueuedTasks.fetch_add(-1);
        completeTask(task, 0);
    }
}

bool WorkQueue::tryComplete()
{
    if (!m_threads.size() || !m_numPendingTasks.load() || !m_numQueuedTasks.load())
        return false;

    Task* task;
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        if (!m_tasks.size())
            return false;

		task = m_tasks.front();
		m_tasks.pop();
    }

	m_numQueuedTasks.fetch_add(-1);
	completeTask(task, 0);

    return true;
}

void WorkQueue::workerLoop(unsigned threadIndex_){
    WorkQueue::ThreadIndex = threadIndex_;

    for (;;){
        Task* task;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
			m_signal.wait(lock, [this]{
                return !m_tasks.empty() || m_shouldExit;
            });

            if (m_shouldExit)
                break;

            task = m_tasks.front();
			m_tasks.pop();
        }

		m_numQueuedTasks.fetch_add(-1);
        completeTask(task, threadIndex_);
    }
}

void WorkQueue::completeTask(Task* task, unsigned threadIndex_){
    task->Complete(threadIndex_);

    if (task->dependentTasks.size()){
        // Queue dependent tasks now if no more dependencies left
        for (auto it = task->dependentTasks.begin(); it != task->dependentTasks.end(); ++it){
            Task* dependentTask = *it;

            if (dependentTask->numDependencies.fetch_add(-1) == 1){
                if (m_threads.size()){{
                        std::lock_guard<std::mutex> lock(m_queueMutex);
						m_tasks.push(dependentTask);
                    }
                    // Note: numPendingTasks counter was already incremented when adding the first dependency, do not do again here
					m_numQueuedTasks.fetch_add(1);
					m_signal.notify_one();
                }else{
                    // If no threads, execute directly
                    completeTask(dependentTask, 0);
                }
            }
        }
        task->dependentTasks.clear();
    }
    // Decrement pending task counter last, so that WorkQueue::Complete() will also wait for the potentially added dependent tasks
	m_numPendingTasks.fetch_add(-1);
}

WorkQueue* WorkQueue::Get() {
	return Instance;
}

void WorkQueue::Init(unsigned numThreads) {
	Instance = new WorkQueue(numThreads);
}