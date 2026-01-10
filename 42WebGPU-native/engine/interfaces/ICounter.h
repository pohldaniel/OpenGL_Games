#pragma once

template <class T>
class ICounter {

public:

	ICounter() {
		Count++;
	}

	ICounter(const ICounter &rhs) {
		Count++;
	}

	ICounter(ICounter&& rhs) {
		Count++;
	}

	ICounter& operator=(const ICounter& rhs) {
		Count++;
		return *this;
	}

	ICounter& operator=(ICounter&& other) {
		Count++;
		return *this;
	}

	virtual ~ICounter() {
		Count--;
	}

	static int GetCount() {
		return Count;
	}

private:
	static int Count;
};

template<class T>
int ICounter<T>::Count = 0;
