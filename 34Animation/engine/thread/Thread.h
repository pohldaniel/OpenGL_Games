#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <stdexcept>
#include <mutex>
#include <queue>
#include <iostream>

class Looper{

public:
	using Runnable = std::function<void()>;

	class Dispatcher{
		friend class Looper;

	public:
		bool post(Looper::Runnable &&aRunnable);

	private:
		Dispatcher(Looper &aLooper);
		Looper &mAssignedLooper;
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


	std::thread mThread;
	std::atomic_bool mRunning;
	std::atomic_bool mAbortRequested;
	std::queue<Runnable> mRunnables;
	std::recursive_mutex mRunnablesMutex;
	std::shared_ptr<Dispatcher> mDispatcher;
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
	bool mTaskLoaded;

	std::thread mThread;
	std::atomic_bool mRunning;
	std::atomic_bool mAbortRequested;

	std::vector<std::function<void()>>  mRunnables;
	std::recursive_mutex  mRunnablesMutex;
	int taskIndex;
};