#pragma once
#include"myiterator.h"
#include"malloc_alloc_template.h"
namespace mystl {
	//节点
	template<class T>
	struct __list_node {
		typedef __list_node<T>* pointer;
		pointer prev;	//也可以设置为__list_node<T>*
		pointer next;
		T data;
	};

	//迭代器
	template<class T>
	struct __list_iterator:public myIterator<_bidirectional_iterator_tag,T> {
		typedef __list_iterator<T> iterator;
		typedef __list_iterator<T> self;

		typedef	T		value_type;
		typedef	T&		reference;
		typedef	T*		pointer;
		typedef __list_node<T>* link_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		link_type node;//指向list节点
		
		//construct
		__list_iterator(){}
		__list_iterator(link_type x):node(x){}
		__list_iterator(const iterator& x):node(x.node){}

		//operator
		bool operator==(const self& x)const { return node == x.node; }
		bool operator!=(const self& x)const { return node != x.node; }
		reference operator*() const { return (*node).data; }//取节点数据值
		pointer operator->()const { return &(operator*()); }

		self& operator++() {
			node = (link_type)((*node).next);
			return *this;
		}

		//后置++运算符重载需要一个int参数来和前置进行区分，而且返回的是对象的一个副本
		self operator++(int) {
			self tmp = *this;
			++*this;
			return tmp;
		}

		self& operator--() {
			node = (link_type)((*node).prev);
			return *this;
		}

		self operator--(int) {
			self tmp = *this;
			--*this;
			return tmp;
		}

		

	};

	//链表
	template<class T>
	class list {
	protected:
		typedef __list_iterator<T> iterator;
		

		typedef typename iterator::value_type value_type;
		typedef typename iterator::reference reference;
		typedef typename iterator::pointer pointer;
		typedef typename iterator::size_type size_type;
		typedef typename iterator::link_type link_type;
		typedef typename iterator::difference_type difference_type;
		
		typedef _malloc_alloc_template<__list_node<T>> data_allocator;
		link_type node;
		
		link_type get_node() {
			return data_allocator::allocate(1);
		}

		link_type create_node(const T& value) {
			link_type p = get_node();
			mystl::construct(&p->data, value);
			return p;
		}

	public:

		void empty_initialize() {
			node = get_node();
			node->next = node;
			node->prev = node;
		}

		list(){
			empty_initialize();
		}
		
		list(const list& rhs):node(rhs.node){}


		iterator begin() {
			return node->next;
		}

		iterator end() {
			return node;
		}

		bool empty(){
			return node->next = node;
		}

		size_type size(){
			size_type result = distance(begin(), end());
			return result;
		}

		reference front() {
			return *begin();
		}
		
		reference back(){
			return *(--end());
		}

		iterator insert(iterator position, const T& value) {
			link_type tmp = create_node(value);
			tmp->next = position.node;
			tmp->prev = position.node->prev;
			(position.node->prev)->next = tmp;
			position.node->prev = tmp;
			return tmp;
		}

		void push_back(const T& value) {
			insert(end(), value);
		}

	};
}
