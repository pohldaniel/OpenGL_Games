#pragma once
#include <stack>

template<class T>
class IStateMachine {

public:

	IStateMachine() = default;
	virtual ~IStateMachine();

	T* addStateAtTop(T* state);
	void addStateAtBottom(T* state);
	void clearAndPush(T* state);

	const bool isRunning() const;
	std::stack<T*>& getStates();

protected:

	void clearStates();

	std::stack<T*> m_states;
	bool m_isRunning = true;
};

template<typename T>
IStateMachine<T>::~IStateMachine() {
	clearStates();
}

template<typename T>
T* IStateMachine<T>::addStateAtTop(T* state) {
	if (!m_states.empty())
		m_states.top()->m_isActive = false;

	m_states.push(state);
	state->m_isActive = true;
	return state;
}

template<typename T>
void IStateMachine<T>::addStateAtBottom(T* state) {
	if (m_states.empty()) {
		m_states.push(state);
		state->m_isActive = true;
	}else {
		T* temp = m_states.top();
		m_states.pop();
		addStateAtBottom(state);
		m_states.push(temp);
	}
}

template<typename T>
void IStateMachine<T>::clearAndPush(T* state) {
	clearStates();
	m_states.push(state);
	state->m_isActive = true;
}

template<typename T>
const bool IStateMachine<T>::isRunning() const {
	return m_isRunning;
}

template<typename T>
std::stack<T*>& IStateMachine<T>::getStates() {
	return m_states;
}

template<typename T>
void IStateMachine<T>::clearStates() {
	while (!m_states.empty()) {
		delete m_states.top();
		m_states.pop();
	}
}

template<class T>
class IState {

	friend class IStateMachine<T>;

public:

	IState() = default;
	virtual ~IState() = default;

	virtual void fixedUpdate() = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void resize(int deltaW, int deltaH) {};
	virtual void OnReEnter() {};

	const bool isRunning() const;
	const bool isActive() const;
	void stopState();

protected:

	bool m_isRunning = true;
	bool m_isActive = true;
};

template<typename T>
const bool IState<T>::isRunning() const {
	return m_isRunning;
}

template<typename T>
const bool IState<T>::isActive() const {
	return m_isActive;
}

template<typename T>
void IState<T>::stopState() {
	m_isRunning = false;
}