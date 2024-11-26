#pragma once
#if 0
#include<new>
#define _THROW_BAD_ALLOC throw bad_alloc
#elif !defined(_THROW_BAD_ALLOC)
#include<iostream>
#define _THROW_BAD_ALLOC std::cerr<<"out of memory"<<std::endl; exit(1)
#endif

#include"construct.h"
#include"util.h"
template<class T>
class _malloc_alloc_template {
private:
	//���º������ڴ����ڴ治������
	static T* oom_malloc(size_t);
	static T* oom_realloc(void*, size_t);
	static void (*__malloc_alloc_oom_handler)();


public:

	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	static T* allocate() {
		T* result = static_cast<T*>(::operator new(sizeof(T)));
		return result;
	}

	static T* allocate(size_t n) {
		if (n == 0) return nullptr;
		T* result = static_cast<T*>(::operator new(n * sizeof(T)));	//��һ����������ֱ��ʹ��alloc,��ͬ��ֱ��ʹ��realloc��free
		if (result == 0) result = oom_malloc(n);
		return result;
	}

	static void deallocate(T* p) {
		if (p == nullptr) return;
		::operator delete(p);
	}

	static void deallocate(T* p, size_t /* n */) {
		if (p == nullptr) return;
		::operator delete(p);
	}

	static T* reallocate(T* p, size_t /* old_sz */, size_t new_sz) {
		T* result = static_cast<T*>(realloc(p, new_sz));
		if (result == 0) result = oom_realloc;
		return result;
	}

	static void construct(T* ptr){
		mystl::construct(ptr);
	}

	static void construct(T* ptr, const T& value) {
		mystl::construct(ptr, value);
	}

	static void construct(T* ptr, T&& value) {
		mystl::construct(ptr, mystl::move(value));
	}

	template<class ...Args>
	static void construct(T* ptr, Args&& ...args) {
		mystl::construct(ptr, mystl::forward<Args>(args)...);
	}

	static void destroy(T* ptr) {
		mystl::destroy(ptr);
	}

	static void destroy(T* first, T* last) {
		mystl::destroy(first, last);
	}

	//���·���C++��set_new_handler(),����ͨ����ָ���Լ���out_of_memory handler
	//void(*...)()����һ���������õĽ�������ú������õĽ��������ֵ����һ��������ָ��
	static void (*set_malloc_handler(void (*f)()))(){	//set_malloc_handler����һ��ָ���޲���������Ϊvoid�ĺ�����ָ����Ϊ������������һ��ͬ�����͵ĺ���ָ��
		void (*old)() = __malloc_alloc_oom_handler;	//���浱ǰ������ָ��
		__malloc_alloc_oom_handler = f;	//�����µĴ�����ָ��
		return old;	//����֮ǰ�Ĵ�����ָ��
	}
};

template<class T>
void (*_malloc_alloc_template<T>::__malloc_alloc_oom_handler)() = 0;

template<class T>
T* _malloc_alloc_template<T>::oom_malloc(size_t n)
{
	void (*my_malloc_handler)();
	T* result;
	for (;;) {
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (my_malloc_handler == 0) { _THROW_BAD_ALLOC; }
		(*my_malloc_handler)();
		result = static_cast<T*>(::operator new(n * sizeof(T)));
		if (result) return (result);
	}
}

template<class T>
T* _malloc_alloc_template<T>::oom_realloc(void* p, size_t n)
{
	void (*my_malloc_handler)();
	T* result;
	for (;;) {
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (my_malloc_handler == 0) { _THROW_BAD_ALLOC; }
		(*my_malloc_handler)();
		result = (T*)realloc(p, n);
		if (result) return (result);
	}
}

