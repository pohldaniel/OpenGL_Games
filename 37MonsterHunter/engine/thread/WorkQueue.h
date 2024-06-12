#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

struct Task {
    Task();
    virtual ~Task();


    virtual void Complete(unsigned threadIndex) = 0;
    std::vector<Task*> dependentTasks;
    std::atomic<int> numDependencies;
};


struct FunctionTask : public Task {
    typedef void (*WorkFunctionPtr)(Task*, unsigned);

    FunctionTask(WorkFunctionPtr function) :function(function){
    }

    void Complete(unsigned threadIndex) override{
        function(this, threadIndex);
    }

    WorkFunctionPtr function;
};

template<class T> struct MemberFunctionTask : public Task {
    typedef void (T::* MemberWorkFunctionPtr)(Task*, unsigned);

    MemberFunctionTask(T* object, MemberWorkFunctionPtr function) : object(object), function(function){
    }

    void Complete(unsigned threadIndex) override{
        (object->*function)(this, threadIndex);
    }

    T* object;
    MemberWorkFunctionPtr function;
};

class WorkQueue{

public:

    WorkQueue(unsigned numThreads);
    ~WorkQueue();

    void queueTask(Task* task);
    void queueTasks(size_t count, Task** tasks);
    void addDependency(Task* task, Task* dependency);
    void complete();
    bool tryComplete();

    unsigned int getNumThreads() const { return (unsigned)m_threads.size() + 1; }

    static unsigned GetThreadIndex() { return ThreadIndex; }

	static void Init(unsigned numThreads);
	static WorkQueue* Get();

private:

    void workerLoop(unsigned threadIndex);
    void completeTask(Task*, unsigned threadIndex);

    std::mutex m_queueMutex;
    std::condition_variable m_signal;
    volatile bool m_shouldExit;
    std::queue<Task*> m_tasks;
    std::vector<std::thread> m_threads;
    std::atomic<int> m_numQueuedTasks;
    std::atomic<int> m_numPendingTasks;

    static thread_local unsigned ThreadIndex;
	static WorkQueue* Instance;
};
