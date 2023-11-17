#pragma once
#include <string>
#include <deque>
#include <functional>
#include <engine/Texture.h>
#include <Thread/CThread.h>

struct Task {
	Task();
	virtual ~Task();
	virtual void Complete() = 0;
	virtual void Process() = 0;

	bool m_finished = false;
};

template<class T> struct MemberFunctionTask : public Task {
	typedef void (T::* MemberWorkFunctionPtr)();

	MemberFunctionTask(T* object_, MemberWorkFunctionPtr OnProcess_, MemberWorkFunctionPtr OnComplete_) : object(object_), OnProcess(OnProcess_), OnComplete(OnComplete_) {
	}

	void Process() override {
		(object->*OnProcess)();
	}
	
	void Complete() override {
		std::invoke(OnComplete, *object);
	}

	T* object;
	MemberWorkFunctionPtr OnProcess = nullptr;
	MemberWorkFunctionPtr OnComplete = nullptr;
	
};

class LoadingManager : public CThread {

public:

	LoadingManager();


	void startBackgroundThread();
	void addTask(Task* task);

	virtual bool OnTask() override = 0;
	virtual bool isFinished() = 0;
	virtual void completeCurTask() = 0;
	void update();

protected:

	std::deque<Task*> m_queue;
	std::deque<Task*> m_tasks;
	CMutexClass m_accessMutex;

	bool m_started;
	bool m_finished;
};

class LoadingManagerSerialized : public LoadingManager {

public:

	LoadingManagerSerialized();

	bool OnTask() override;
	bool isFinished() override;
	void completeCurTask() override;

	static LoadingManagerSerialized& Get();

private:
	
	static LoadingManagerSerialized Instance;
};

class LoadingManagerSplitted : public LoadingManager {

public:

	LoadingManagerSplitted();

	bool OnTask() override;
	bool isFinished() override;
	void completeCurTask() override;

	static LoadingManagerSplitted& Get();

private:

	static LoadingManagerSplitted Instance;
};