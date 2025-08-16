#include "Loadingmanager.h"

LoadingManager::Task::Task(){

}

LoadingManager::Task::~Task(){
}

LoadingManager::LoadingManager() : m_finished(false), m_accessMutex(), m_started(false){
	SetThreadType(ThreadTypeEventDriven);
}


void LoadingManager::startBackgroundThread() {
	m_finished = false;
	m_started = false;

	do{
		this->Event();
		Sleep(10);
	} while (!m_started);
}


void LoadingManager::addTask(Task* task) {
	m_tasks.push_back(task);
}

void LoadingManager::update() {
	if (!isFinished()) {
		completeCurTask();
	}
}

LoadingManagerSerialized LoadingManagerSerialized::Instance;

LoadingManagerSerialized::LoadingManagerSerialized() : LoadingManager() {

}

bool LoadingManagerSerialized::OnTask() {
	if (m_started) {
		return true;
	}

	m_started = true;

	m_tasks.front()->Process();

	//Important Note: The task Complete function will be set after finishing the task Process function
	m_accessMutex.Lock();
	m_queue.push_back(m_tasks.front());
	m_finished = true;
	m_accessMutex.Unlock();

	return true;
}

void LoadingManagerSerialized::completeCurTask() {
	if (m_queue.size() > 0) {
		m_accessMutex.Lock();
		LoadingManager::Task *curEntry = m_queue.front();
		m_tasks.front()->m_finished = true;
		m_accessMutex.Unlock();

		curEntry->Complete();

		m_tasks.pop_front();
		if (m_tasks.size() > 0) {
			startBackgroundThread();
		}

		delete curEntry;
		m_accessMutex.Lock();
		m_queue.pop_front();
		m_accessMutex.Unlock();
	}
}

bool LoadingManagerSerialized::isFinished() {
	bool result = false;
	m_accessMutex.Lock();
	result = m_finished && m_tasks.front()->m_finished;
	m_accessMutex.Unlock();
	return result;
}

LoadingManagerSerialized& LoadingManagerSerialized::Get() {
	return Instance;
}

LoadingManagerSplitted LoadingManagerSplitted::Instance;

LoadingManagerSplitted::LoadingManagerSplitted() : LoadingManager() {

}

bool LoadingManagerSplitted::OnTask() {
	if (m_started) {
		return true;
	}

	m_started = true;

	for (auto&& task : m_tasks)
		task->Process();

	//Important Note: The task Complete function will be set after finishing the task Process function
	m_accessMutex.Lock();
	std::copy(m_tasks.begin(), m_tasks.end(), std::inserter(m_queue, m_queue.end()));
	m_finished = true;
	m_accessMutex.Unlock();

	return true;
}

void LoadingManagerSplitted::completeCurTask() {
	if (m_queue.size() > 0) {
		m_accessMutex.Lock();
		LoadingManager::Task *curEntry = m_queue.front();
		m_accessMutex.Unlock();

		m_tasks.clear();
		curEntry->Complete();

		delete curEntry;
		m_accessMutex.Lock();
		m_queue.pop_front();
		m_accessMutex.Unlock();
	}
}

bool LoadingManagerSplitted::isFinished() {
	bool result = false;
	m_accessMutex.Lock();
	result = m_finished && m_queue.size() == 0;
	m_accessMutex.Unlock();
	return result;
}

LoadingManagerSplitted& LoadingManagerSplitted::Get() {
	return Instance;
}