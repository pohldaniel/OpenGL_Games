#pragma once
#include <string>
#include <deque>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

#include "ILoadingListener.h"

class LoadingManager {
public:
   
    LoadingManager(const LoadingManager&) = delete;
    LoadingManager& operator=(const LoadingManager&) = delete;

    void startBackgroundThreads(size_t threadCount = 1);

    void addTask(ILoadingListener* listener);
    void update();
    bool isFinished();

    static LoadingManager& Get();

private:
    LoadingManager() = default;
    ~LoadingManager();

    void workerThreadLoop();

    // Die Queues halten jetzt einfach Pointer auf das Interface
    std::deque<ILoadingListener*> m_backgroundQueue;
    std::deque<ILoadingListener*> m_mainThreadQueue;

    std::vector<std::thread> m_workers;
    std::mutex               m_queueMutex;
    std::condition_variable  m_cv;
    std::atomic<bool>        m_shutdown{ false };

    static LoadingManager Instance;
};