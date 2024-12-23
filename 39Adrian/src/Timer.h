#pragma once
#include <iostream>
#include <functional>

class Timer {

public:

	Timer();
	Timer& operator=(const Timer& rhs);
	Timer& operator=(Timer&& rhs);
	Timer(Timer const& rhs);
	Timer(Timer&& rhs);

	virtual ~Timer() = default;

	virtual void update(const float dt);
	void start(unsigned int milli, bool repeat = true, bool starOnce = false);
	void stop();
	const bool isActivated() const;
	void setOnTimerEnd(std::function<void()> fun);
	void setStartOnce(bool startOnce);

protected:

	float m_elapsedTime;
	float m_updateTime;
	bool m_repeate;
	bool m_activated;
	bool m_startOnce;

private:
	std::function<void()> OnTimerEnd;
};

enum CallBack {
	CALL_BACK_1,
	CALL_BACK_2,
	CALL_BACK_3,
	CALL_BACK_4
};

template <class U>
class CBTimer : public Timer {

public:

	CBTimer();
	CBTimer(U* receiver, CallBack callBack);
	CBTimer& operator=(const CBTimer& rhs);
	CBTimer& operator=(CBTimer&& rhs);
	CBTimer(CBTimer const& rhs);
	CBTimer(CBTimer&& rhs);

	~CBTimer();

	void update(const float dt) override;
	void setReceiver(U* receiver);
	void setCallBack(CallBack callBack);
	U* m_receiver;
private:

	
	CallBack m_callBack;
};

template <typename U>
CBTimer<U>::CBTimer() : Timer(), m_receiver(nullptr), m_callBack(CALL_BACK_1){

}

template <typename U>
CBTimer<U>::CBTimer(U* recipient, CallBack callBack) : Timer(), m_receiver(recipient), m_callBack(callBack) {

}

template <typename U>
CBTimer<U>::~CBTimer() {
	m_receiver = nullptr;
}

template <typename U>
CBTimer<U>::CBTimer(CBTimer const& rhs) : Timer(rhs), m_callBack(rhs.m_callBack) {
}

template <typename U>
CBTimer<U>::CBTimer(CBTimer&& rhs) : Timer(rhs),  m_callBack(rhs.m_callBack) {
}

template <typename U>
CBTimer<U>& CBTimer<U>::operator=(const CBTimer& rhs) {
	Timer::operator=(rhs);
	m_callBack = rhs.m_callBack;
	return *this;
}

template <typename U>
CBTimer<U>& CBTimer<U>::operator=(CBTimer&& rhs) {
	Timer::operator=(rhs);
	m_callBack = rhs.m_callBack;
	return *this;
}

template <typename U>
void CBTimer<U>::update(const float dt) {

	if (!m_activated)
		return;

	m_elapsedTime += dt;

	while (m_elapsedTime > m_updateTime) {
		m_receiver->OnCallBack(m_callBack);
		m_elapsedTime = m_repeate ? m_elapsedTime - m_updateTime : 0.0f;
		m_activated = m_repeate;
	}
}

template <typename U>
void CBTimer<U>::setReceiver(U* receiver) {
	m_receiver = receiver;
}

template <typename U>
void CBTimer<U>::setCallBack(CallBack callBack) {
	m_callBack = callBack;
}