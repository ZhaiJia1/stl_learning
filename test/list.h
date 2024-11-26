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
		typedef _malloc_alloc_template<T> node_allocator;
		link_type node;
		
		//get,put,create,destory操作应该是对应的，先获取node的空间，再构造node的data
		//destroy时，先把node的data析构，再释放node的空间
		link_type get_node() {
			return data_allocator::allocate(1);
		}
		void put_node(link_type p) {
			data_allocator::deallocate(p);
		}

		link_type create_node(const T& value) {
			link_type p = get_node();
			node_allocator::construct(&p->data, value);
			return p;
		}

		void destroy_node(link_type p) {
			node_allocator::destroy(&p->data);
			put_node(p);
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

		void push_back(const value_type& value) {
			insert(end(), value);
		}

		void push_front(const value_type& value);
		iterator erase(iterator position);
		iterator erase(iterator first, iterator last);
		void pop_front();
		void pop_back();
		void clear();

		template<class UnaryPredicate>
		void remove_if(UnaryPredicate pred);

		void remove(const value_type& value) {
			remove_if([&](const value_type& v) {return v == value; });
		}
		//后续写functional再改进
		void unqiue();
		
		void transfer(iterator position, iterator first, iterator last);


	};

	template<class T> 
	void mystl::list<T>::push_front(const value_type& value) {
		insert(begin(), value);
	}

	//移除position节点，返回的迭代器指向position后一个节点
	template<class T>
	typename list<T>::iterator list<T>::erase(iterator position) {
		if (position != end()) {
			auto n = position.node;
			auto next = n->next;
			n->prev->next = next;
			next->prev = n->prev;
			destroy_node(n);
			return iterator(next);
		}
		return iterator();
	}

	template<class T>
	typename list<T>::iterator list<T>::erase(iterator first, iterator last)
	{
		if (first != last) {
			first.node->prev->next = last.node;
			last.node->prev = first.node->prev;
			while (first != last) {
				auto cur = first.node;
				++first;
				destroy_node(cur);
			}
		}
		return iterator(last.node);
	}

	template<class T>
	void list<T>::clear() {
		erase(begin(), end());
	}

	//移除数值相同的连续元素
	template<class T>
	inline void list<T>::unqiue() {
		auto first = begin();
		auto last = end();
		auto cur = first;
		++cur;
		while (cur != last) {
			if (first.node->data == cur.node->data) {
				cur = erase(cur);
			}
			else {
				++cur;
			}
			first = cur;
			++cur;
		}
	}

	template<class T>
	inline void list<T>::transfer(iterator position, iterator first, iterator last) {

	}

	template<class T>
	void list<T>::pop_front() {
		erase(begin());
	}

	template<class T>
	void list<T>::pop_back() {
		erase(--end());
	}
	template<class T>
	template<class UnaryPredicate>
	inline void list<T>::remove_if(UnaryPredicate pred) {
		iterator first = begin();
		iterator last = end();
		while (first != last) {
			if (pred(*first)) {
				first = erase(first);
			}
			else {
				++first;
			}
		}
	}
}
