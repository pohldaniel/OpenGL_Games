#pragma once

/// Pointer which takes ownership of an object and deletes it when the pointer goes out of scope. Ownership can be transferred to another pointer, in which case the source pointer becomes null.
template <class T> class AutoPtr
{
public:

	AutoPtr() :ptr(nullptr){
	}

	AutoPtr(const AutoPtr<T>& ptr_) : ptr(ptr_.ptr){
		const_cast<AutoPtr<T>&>(ptr_).ptr = nullptr;
	}


	AutoPtr(T* ptr_) : ptr(ptr_){
	}

	~AutoPtr(){
		delete ptr;
	}

	AutoPtr<T>& operator = (const AutoPtr<T>& rhs){
		delete ptr;
		ptr = rhs.ptr;
		const_cast<AutoPtr<T>&>(rhs).ptr = nullptr;
		return *this;
	}

	AutoPtr<T>& operator = (T* rhs){
		delete ptr;
		ptr = rhs;
		return *this;
	}

	T* Detach(){
		T* ret = ptr;
		ptr = nullptr;
		return ret;
	}

	void Reset(){
		*this = nullptr;
	}

	T* operator -> () const { return ptr; }
	T& operator * () const { return *ptr; }
	operator T* () const { return ptr; }

	T* Get() const { return ptr; }
	bool IsNull() const { return ptr == nullptr; }

private:
	T* ptr;
};

/// Pointer which takes ownership of an array allocated with new[] and deletes it when the pointer goes out of scope.
template <class T> class AutoArrayPtr {

public:

	AutoArrayPtr() :array(nullptr){
	}

	AutoArrayPtr(const AutoArrayPtr<T>& ptr) :array(ptr.array){
		const_cast<AutoArrayPtr<T>&>(ptr).array = nullptr;
	}

	AutoArrayPtr(T* array_) :array(array_){
	}

	~AutoArrayPtr(){
		delete[] array;
	}


	AutoArrayPtr<T>& operator = (AutoArrayPtr<T>& rhs){
		delete array;
		array = rhs.array;
		rhs.array = nullptr;
		return *this;
	}

	AutoArrayPtr<T>& operator = (T* rhs){
		delete array;
		array = rhs;
		return *this;
	}

	T* Detach(){
		T* ret = array;
		array = nullptr;
		return ret;
	}

	void Reset(){
		*this = nullptr;
	}

	T* operator -> () const { assert(array); return array; }
	T& operator * () const { assert(array); return *array; }
	operator T* () const { return Get(); }

	T* Get() const { return array; }
	bool IsNull() const { return array == nullptr; }

private:
	T* array;
};
