#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <stdexcept>
#include <mutex>
#include <queue>

namespace ThreadUtil {
	bool IsMainThread();
	unsigned CPUCount();
}

class Looper{

public:
	using Runnable = std::function<void()>;

	class Dispatcher{
		friend class Looper;

	public:
		bool post(Looper::Runnable &&aRunnable);

	private:
		Dispatcher(Looper &aLooper);
		Looper &m_assignedLooper;
	};


	Looper();
	~Looper();
	bool running() const;
	bool run();
	void stop();
	std::shared_ptr<Dispatcher> getDispatcher();

	static void Test();

private:

	void runFunc();
	void abortAndJoin();
	Runnable next();
	bool post(Runnable &&aRunnable);

	std::thread m_thread;
	std::atomic_bool m_running;
	std::atomic_bool m_abortRequested;
	std::queue<Runnable> m_runnables;
	std::recursive_mutex m_runnablesMutex;
	std::shared_ptr<Dispatcher> m_dispatcher;
};

class EndlessLooper {

public:

	EndlessLooper();
	~EndlessLooper();

	void run();
	bool running() const;	
	void stop();
	bool addTask(std::function<void()> &&aRunnable);

private:
	
	void runFunc();
	void abortAndJoin();
	std::function<void()> next();


	std::thread m_thread;
	std::atomic_bool m_running;
	std::atomic_bool m_abortRequested;

	std::vector<std::function<void()>>  m_runnables;
	std::recursive_mutex  m_runnablesMutex;
	int m_taskIndex;
};