#include "Thread.h"

std::thread::id mainThreadId = std::this_thread::get_id();

bool ThreadUtil::IsMainThread() {
	return std::this_thread::get_id() == mainThreadId;
}

unsigned ThreadUtil::CPUCount() {
	return std::thread::hardware_concurrency();
}

Looper::Dispatcher::Dispatcher(Looper &aLooper) : m_assignedLooper(aLooper){
}

bool Looper::Dispatcher::post(Looper::Runnable &&aRunnable){
	return m_assignedLooper.post(std::move(aRunnable));
}


Looper::Looper()
	: m_running(false),
	m_abortRequested(false),
	m_runnables(),
	m_runnablesMutex(),
	m_dispatcher(std::shared_ptr<Dispatcher>(new Dispatcher(*this))){ }


Looper::~Looper(){
	abortAndJoin();
}

bool Looper::running() const{
	return m_running.load();
}

bool Looper::run(){
	try
	{
		m_thread = std::thread(&Looper::runFunc, this);
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
	return m_dispatcher;
}

void Looper::runFunc(){
	m_running.store(true);

	while (false == m_abortRequested.load()){
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

	m_running.store(false);
}

void Looper::abortAndJoin(){
	m_abortRequested.store(true);
	if (m_thread.joinable()){
		m_thread.join();
	}
}

// Runnables
Looper::Runnable Looper::next(){
	std::lock_guard<std::recursive_mutex> guard(m_runnablesMutex);

	if (m_runnables.empty()){
		return nullptr;
	}
	Runnable runnable = m_runnables.front();
	m_runnables.pop();

	return runnable;
}

bool Looper::post(Looper::Runnable &&aRunnable){
	if (!running()){
		return false;
	}

	try{
		std::lock_guard<std::recursive_mutex> guard(m_runnablesMutex);
		m_runnables.push(std::move(aRunnable));
	}
	catch (...) {
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
EndlessLooper::EndlessLooper() : m_running(false), m_abortRequested(false), m_taskIndex(0){

}

EndlessLooper::~EndlessLooper(){
	abortAndJoin();
}

void EndlessLooper::run() {
	m_thread = std::thread(&EndlessLooper::runFunc, this);
	m_running.store(true);
}

bool EndlessLooper::running() const {
	return m_running.load();
}

void EndlessLooper::stop() {
	abortAndJoin();
}


void EndlessLooper::runFunc() {
	m_running.store(true);

	while (false == m_abortRequested.load()) {
		std::function<void()> r = next();
		if (nullptr != r){
			r();
		}
	}

	m_running.store(false);
}

void EndlessLooper::abortAndJoin() {
	m_abortRequested.store(true);
	if (m_thread.joinable()) {
		m_thread.join();
	}
}

bool EndlessLooper::addTask(std::function<void()> &&aRunnable){
	if (!running()){
		return false;
	}

	std::lock_guard<std::recursive_mutex> guard(m_runnablesMutex);
	m_runnables.push_back(std::move(aRunnable));
	return true;
}

std::function<void()> EndlessLooper::next() {
	std::lock_guard<std::recursive_mutex> guard(m_runnablesMutex);

	if (m_runnables.empty()) {
		
		return nullptr;
	}

	if (m_taskIndex < m_runnables.size() - 1) {
		m_taskIndex++;
	}else {
		m_taskIndex = 0;
	}

	return m_runnables[m_taskIndex];
}