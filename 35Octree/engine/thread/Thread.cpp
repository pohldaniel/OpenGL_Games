#include "Thread.h"

std::thread::id mainThreadId = std::this_thread::get_id();

bool ThreadUtil::IsMainThread() {
	return std::this_thread::get_id() == mainThreadId;
}

unsigned ThreadUtil::CPUCount() {
	return std::thread::hardware_concurrency();
}

Looper::Dispatcher::Dispatcher(Looper &aLooper) : mAssignedLooper(aLooper){
}

bool Looper::Dispatcher::post(Looper::Runnable &&aRunnable){
	return mAssignedLooper.post(std::move(aRunnable));
}


Looper::Looper()
	: mRunning(false),
	mAbortRequested(false), 
	mRunnables(), 
	mRunnablesMutex(), 
	mDispatcher(std::shared_ptr<Dispatcher>(new Dispatcher(*this))){ }


Looper::~Looper(){
	abortAndJoin();
}

bool Looper::running() const{
	return mRunning.load();
}

bool Looper::run(){
	try
	{
		mThread = std::thread(&Looper::runFunc, this);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

void Looper::stop(){
	abortAndJoin();
}

std::shared_ptr<Looper::Dispatcher> Looper::getDispatcher(){
	return mDispatcher;
}

void Looper::runFunc(){
	mRunning.store(true);

	while (false == mAbortRequested.load()){
		try{
			Runnable r = next();
			if (nullptr != r){
				r();
			}
		}
		catch (...){
			// Make sure that nothing leaves the thread for now...
		}
	}

	mRunning.store(false);
}

void Looper::abortAndJoin(){
	mAbortRequested.store(true);
	if (mThread.joinable()){
		mThread.join();
	}
}

// Runnables
Looper::Runnable Looper::next(){
	std::lock_guard<std::recursive_mutex> guard(mRunnablesMutex);

	if (mRunnables.empty()){
		return nullptr;
	}
	Runnable runnable = mRunnables.front();
	mRunnables.pop();

	return runnable;
}

bool Looper::post(Looper::Runnable &&aRunnable){
	if (!running()){
		// Deny insertion
		std::cout << "Denying insertion, as the looper is not running.\n";
		return false;
	}

	try{
		std::lock_guard<std::recursive_mutex> guard(mRunnablesMutex);
		mRunnables.push(std::move(aRunnable));
	}
	catch (...) {
		return false;
	}

	return true;
}

void Looper::Test() {
	auto looper = std::make_unique<Looper>();

	std::cout << "Starting looper" << std::endl;
	// To start and run
	looper->run();

	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	auto dispatcher = looper->getDispatcher();

	std::cout << "Adding tasks" << std::endl;

	for (uint32_t k = 0; k<500; ++k){
		auto const task = [k](){
			std::cout << "Invocation " << k
				<< ": Hello, I have been executed asynchronously on the looper for " << (k + 1)
				<< " times." << std::endl;
		};

		dispatcher->post(std::move(task));
	}

	std::cout << "Waiting 5 seconds for completion" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));

	std::cout << "Stopping looper" << std::endl;
	// To stop it and clean it up
	dispatcher = nullptr;
	looper->stop();
	looper = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////
EndlessLooper::EndlessLooper() : mRunning(false), mAbortRequested(false), mTaskLoaded(false), taskIndex(0){

}

EndlessLooper::~EndlessLooper(){
	abortAndJoin();
}

void EndlessLooper::run() {
	mThread = std::thread(&EndlessLooper::runFunc, this);
}

bool EndlessLooper::running() const {
	return mRunning.load();
}

void EndlessLooper::stop() {
	abortAndJoin();
}


void EndlessLooper::runFunc() {
	mRunning.store(true);

	while (false == mAbortRequested.load()) {
		std::function<void()> r = next();
		if (nullptr != r){
			r();
		}
	}

	mRunning.store(false);
}

void EndlessLooper::abortAndJoin() {
	mAbortRequested.store(true);
	if (mThread.joinable()) {
		mThread.join();
	}
}

bool EndlessLooper::addTask(std::function<void()> &&aRunnable){
	if (!running()){
		return false;
	}

	std::lock_guard<std::recursive_mutex> guard(mRunnablesMutex);
	mRunnables.push_back(std::move(aRunnable));
	return true;
}

std::function<void()> EndlessLooper::next() {
	std::lock_guard<std::recursive_mutex> guard(mRunnablesMutex);

	if (mRunnables.empty()) {
		
		return nullptr;
	}

	if (taskIndex < mRunnables.size() - 1) {
		taskIndex++;
	}else {
		taskIndex = 0;
	}

	return mRunnables[taskIndex];
}