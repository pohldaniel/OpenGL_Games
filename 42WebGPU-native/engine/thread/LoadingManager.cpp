#include "LoadingManager.h"

LoadingManager LoadingManager::Instance;

LoadingManager::~LoadingManager() {
    m_shutdown = true;
    m_cv.notify_all();
    for (std::thread& worker : m_workers) {
        if (worker.joinable()) worker.join();
    }
}

void LoadingManager::startBackgroundThreads(size_t threadCount) {
    m_shutdown = false;
    for (size_t i = 0; i < threadCount; ++i) {
        m_workers.emplace_back(&LoadingManager::workerThreadLoop, this);
    }
}

void LoadingManager::addTask(ILoadingListener* listener) {
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_backgroundQueue.push_back(listener);
    }
    m_cv.notify_one();
}

void LoadingManager::workerThreadLoop() {
    while (!m_shutdown) {
        ILoadingListener* listener = nullptr;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_cv.wait(lock, [this] { return !m_backgroundQueue.empty() || m_shutdown; });

            if (m_shutdown && m_backgroundQueue.empty()) return;

            listener = m_backgroundQueue.front();
            m_backgroundQueue.pop_front();
        }

        if (listener) {
            listener->OnProcess(); // Ruft Menu::OnProcess im Hintergrund auf

            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_mainThreadQueue.push_back(listener);
        }
    }
}

void LoadingManager::update() {
    std::deque<ILoadingListener*> listenersToComplete;
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        if (!m_mainThreadQueue.empty()) {
            listenersToComplete.swap(m_mainThreadQueue);
        }
    }

    for (auto* listener : listenersToComplete) {
        listener->OnComplete();
    }
}

bool LoadingManager::isFinished() {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    return m_backgroundQueue.empty() && m_mainThreadQueue.empty();
}

LoadingManager& LoadingManager::Get() {
	return Instance;
}