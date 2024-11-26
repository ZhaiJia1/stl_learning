#pragma once
#include<new>
#include<type_traits>
#include<vector>
#include"myiterator.h"
#include<iostream>
namespace mystl {
	
	template<class T>
	void construct(T* p) {
		::new((void*)p) T();	//new期望接受一个void*指针，虽然C++会进行隐式类型转化
	}

	template<class T1, class T2>
	void construct(T1* p, const T2& value) {
		::new((void*)p) T1(value);
	}

	template <class T>
	void destroy(T* p) {
		p->~T();
	}

	//有non-trivial destructor
	template<class ForwardIterator>
	void __destroy_aux(ForwardIterator first, ForwardIterator last, std::false_type) {
		for (; first < last; ++first) {
			destroy(&*first);
			std::cout << "here" << std::endl;
		}
	}
	//有trivial destructor,无关痛痒，不用做任何操作
	template<class ForwardIterator>
	void __destroy_aux(ForwardIterator first, ForwardIterator last, std::true_type) {

	}

	////判断元素的数值型别（value_type)是否有trivial_destructor
	//template<class ForwardIterator, class T>
	//void __destroy(ForwardIterator first, ForwardIterator last, T*) {
	//	typedef typename std::is_trivially_destructible<T> trivial_destructor;
	//	__destroy_aux(first, last, trivial_destructor());
	//}

	template<class ForwardIterator>
	void destroy(ForwardIterator first, ForwardIterator last) {
		typedef typename _iterator_traits<ForwardIterator>::value_type value_type;
		typedef typename std::is_trivially_destructible<value_type> trivial_destructor;
		__destroy_aux(first, last, trivial_destructor());
	}

}










