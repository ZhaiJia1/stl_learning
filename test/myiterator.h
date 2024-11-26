#pragma once
#include<cstdlib>
//五种迭代器类型
struct _input_iterator_tag{};
struct _output_iterator_tag{};
struct _forward_iterator_tag:public _input_iterator_tag {};
struct _bidirectional_iterator_tag:public _forward_iterator_tag {};
struct _random_access_iterator_tag:public _bidirectional_iterator_tag {};

template<class Category,class T,class Distance = ptrdiff_t,class Pointer = T*,class Reference = T&>
struct myIterator{
	typedef Category iterator_category;
	typedef T value_type;
	typedef Distance difference_type;
	typedef Pointer pointer;
	typedef Reference reference;

};

template<class Iterator>
struct _iterator_traits {
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type value_type;
	typedef typename Iterator::difference_type difference_type;
	typedef typename Iterator::pointer pointer;
	typedef typename Iterator::reference reference;

};

template<class T>
struct _iterator_traits<T*> {
	typedef _random_access_iterator_tag iterator_category;
	typedef T value_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef T& reference;
};

template<class T>
struct _iterator_traits<const T*> {
	typedef _random_access_iterator_tag iterator_category;
	typedef T value_type;
	typedef ptrdiff_t difference_type;
	typedef const T* pointer;
	typedef const T& reference;
};

template <class Iterator>
typename _iterator_traits<Iterator>::iterator_category 
iterator_category(const Iterator&) {
	typedef typename _iterator_traits<Iterator>::iterator_category Category;
	return Category();
}

template<class Iterator>
typename _iterator_traits<Iterator>::difference_type* 
difference_type(const Iterator&) {
	return static_cast<typename _iterator_traits<Iterator>::difference_type*>(0);
}

template <class Iterator>
typename _iterator_traits<Iterator>::value_type* _value_type(const Iterator&) {
	return static_cast<typename _iterator_traits<Iterator>::value_type*>(0);
}

template<class InputIterator>
//这里加typename是否有影响
typename _iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, _input_iterator_tag) {
	typename _iterator_traits<InputIterator>::difference_type n = 0;
	while (first != last) {
		++first;
		++n;
	}
	return n;
}

template<class RandomAccessIterator>
typename _iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, _random_access_iterator_tag) {
	return last - first;
}

template<class InputIterator>
typename _iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last) {
	//typedef typename _iterator_traits<InputIterator>::iterator_category category;
	return __distance(first, last, iterator_category(first));
}

template<class InputIterator,class Distance>
void __advance(InputIterator& i, Distance n, _input_iterator_tag) {
	while (n--) {
		++i;
	}
}

template<class BidirectionalIterator,class Distance>
void __advance(BidirectionalIterator& i, Distance n, _bidirectional_iterator_tag) {
	if (n >= 0) {
		while (n--) ++i;
	}
	else {
		while (n++) --i;
	}
}

template<class RandomAccessIterator,class Distance>
void __advance(RandomAccessIterator& i, Distance n, _random_access_iterator_tag) {
	i += n;
}

template<class InputIterator,class Distance>
void advance(InputIterator& i, Distance n) {
	//typedef typename _iterator_traits<InputIterator>::iterator_category category;
	__advance(i, n, iterator_category(i));
}