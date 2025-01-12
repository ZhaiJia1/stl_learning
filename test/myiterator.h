#pragma once
#include<cstdlib>
#include<cstddef>
#include<type_traits>

//type_traits���ֶ��壬�ȷ�������
template<class T, T v>
struct m_integral_constant {
	static constexpr T value = v;
};

template<bool b>
using m_bool_constant = m_integral_constant<bool, b>;
typedef m_bool_constant<true> m_true_type;
typedef m_bool_constant<false> m_false_type;

template<class T1,class T2>
struct m_pair;

template<class T>
struct is_pair :m_false_type {};

template<class T1,class T2>
struct is_pair<m_pair<T1, T2>> :m_true_type {};

//���ֵ���������
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

//iterator_traits
//��T��iterator_category��Աʱ����������ѡ�񷵻�char��test���أ����value������Ϊtrue
template<class T>
struct has_iterator_cat {
private:
	struct two { char a; char b; };
	template<class U>
	static two test(...);
	//�������U����iterator_category���ԣ�������ѡ�����������
	template<class U>
	static char test(typename U::iterator_category* = 0);
public:
	static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

template<class Iterator,bool>
struct iterator_traits_impl{};

//ģ���ػ�����boolΪtrueʱ��iterator_traits_impl���ػ�Ϊһ�������������ڲ����Ͷ���Ľṹ
template<class Iterator>
struct iterator_traits_impl<Iterator, true> {
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type value_type;
	typedef typename Iterator::difference_type difference_type;
	typedef typename Iterator::pointer pointer;
	typedef typename Iterator::reference reference;
};

template<class Iterator,bool>
struct iterator_traits_helper{};

//is_convertible<>����Ƿ���Խ��κ���������A��ʽת��Ϊ�κ���������B
//����ģ�����Iterator��iterator_category��_input����_output���͵������̳�iterator_traits_impl
template<class Iterator>
struct iterator_traits_helper<Iterator,true>:public iterator_traits_impl<Iterator,
	std::is_convertible<typename Iterator::iterator_category,_input_iterator_tag>::value||
	std::is_convertible<typename Iterator::iterator_category,_output_iterator_tag>::value>{};


//��ȡ������������
//helper��boolģ�������Iteratorģ������Ƿ���catgory����
//���գ�ֻ�е�Iteratorģ���������iterator_category,���ҿ��Ժ�input,output���͵������໥ת��
//�ſ��Ա��ػ�Ϊһ�������������ڲ����Ͷ���Ľṹ
template<class Iterator>
struct _iterator_traits :public iterator_traits_helper<Iterator,has_iterator_cat<Iterator>::value> {};

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

//���T�ĵ����������Ƿ��U�����໥ת��
template<class T,class U,bool = has_iterator_cat<_iterator_traits<T>>::value>
struct has_iterator_cat_of :public m_bool_constant<std::is_convertible<
	typename _iterator_traits<T>::iterator_category, U>::value> {};

//��ȡĳ�ֵ�����
template<class T,class U>
struct has_iterator_cat_of<T, U, false> :public m_false_type {};

template<class Iterator>
struct is_exactly_input_iterator :
	public m_bool_constant<has_iterator_cat_of<Iterator, _input_iterator_tag>::value &&
	!has_iterator_cat_of<Iterator, _forward_iterator_tag>::value> {};

template<class Iterator>
struct is_input_iterator :public has_iterator_cat_of<Iterator, _input_iterator_tag> {};

template<class Iterator>
struct is_output_iterator :public has_iterator_cat_of<Iterator, _output_iterator_tag> {};

template<class Iterator>
struct is_forward_iterator :public has_iterator_cat_of<Iterator, _forward_iterator_tag> {};

template<class Iterator>
struct is_bidirectional_iterator :public has_iterator_cat_of<Iterator, _bidirectional_iterator_tag> {};

template<class Iterator>
struct is_random_access_iterator :public has_iterator_cat_of<Iterator, _random_access_iterator_tag> {};

template<class Iterator>
struct is_iteraor :public m_bool_constant<is_input_iterator<Iterator>::value || is_output_iterator<Iterator>::value> {};



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
typename _iterator_traits<Iterator>::value_type* value_type(const Iterator&) {
	return static_cast<typename _iterator_traits<Iterator>::value_type*>(0);
}

template<class InputIterator>
//�����typename�Ƿ���Ӱ��
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