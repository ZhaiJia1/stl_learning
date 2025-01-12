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

		list(list&& rhs)noexcept :node(rhs.node) {}

		//=操作符重载
		list& operator=(const list& rhs) {
			if(this!=&rhs)	//通过判断地址是否相同来判断是否相等
				assign(rhs.begin(), rhs.end());
			return *this;
		}


		list& operator=(list&& rhs)noexcept {
			clear();
			splice(end(), rhs);
			return *this;
		}

		void fill_assign(size_type n, const value_type& value);
		template<class Iterator>
		void copy_assign(Iterator first, Iterator last);
		//调整容器相关操作
		void assign(size_type n, const value_type& value) {
			fill_assign(n, value);
		}
		
		//enable_if与typename和::type一起使用，以访问当条件为真时定义的类型，条件为真时是第二个模板参数
		template<class Iterator, typename std::enable_if < is_input_iterator<Iterator>::value,int >::type = 0 >
		void assign(Iterator first, Iterator last) {
			copy_assign(first, last);
		};

		void transfer(iterator position, iterator first, iterator last) {
			if (position != last) {
				last .node->prev->next = position.node;
				first.node->prev->next = last.node;
				position.node->prev->next = first.node;
				auto tmp = position.node->prev;
				position.node->prev = last.node->prev;
				last.node->prev = first.node->prev;
				first.node->prev = tmp;
				
			}
		}

		void splice(iterator position, list& x) {
			if(!x.empty())
				transfer(position, x.begin(), x.end());
		}

		//将i所指元素结合与position所指位置之前
		void splice(iterator position, list&, iterator i) {
			iterator j = i;
			++j;
			if (position == i || position == j) return;//如果i就是position本身或者i本来就在position前，则不做操作
			transfer(position, i, j);
		}

		//position不能位于first与last之间，是否要写一个函数确保一下   后注：不需要，因为在中间也可以，list的迭代器不会失效
		void splice(iterator position, list&, iterator first, iterator last) {
			if (first != last) {
				transfer(position, first, last);
			}
		}

		void merge(list& x);
		void reverse();

		//析构时把所有节点的空间释放，再释放node
		~list() {
			if (node) {
				clear();
				data_allocator::deallocate(node);
				node = nullptr;
			}
		}

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

		iterator insert(iterator position, iterator first, iterator last);

		iterator copy_insert(iterator position, size_type n, iterator first);

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
		template<class Compared>
		iterator sort(iterator first,iterator last,Compared comp);
		
		void sort() {
			sort(std::less<T>{});
		}
		template<class Compared>
		void sort(Compared comp) {
			sort(begin(), end(), comp);
		}

		template<class Compared>
		iterator merge_same(iterator first, iterator mid, iterator last, Compared comp);


	};

	template<class T>
	inline void list<T>::fill_assign(size_type n, const value_type& value) {
		auto first = begin();
		auto last = end();
		for (; n > 0 && first != last; --n, ++first) {
			*first = value;
		}
		if (n > 0) {
			while (n > 0) {
				insert(last, value);
				n--;
			}
		}
		else {
			erase(first, last);
		}
	}

	//在position前插入first到last的范围
	template<class T>
	typename mystl::list<T>::iterator list<T>::insert(iterator position, iterator first, iterator last){
		size_type n = distance(first, last);
		return copy_insert(position, n, first);
	}

	template<class T>
	typename mystl::list<T>::iterator list<T>::copy_insert(iterator position, size_type n, iterator first) {
		for (; n > 0; --n,++first) {
			insert(position, *first);
		}
		return position;
	}

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
	void list<T>::pop_front() {
		erase(begin());
	}

	template<class T>
	void list<T>::pop_back() {
		erase(--end());
	}
	
	template<class T>
	template<class Iterator>
	inline void list<T>::copy_assign(Iterator first, Iterator last) {
		auto f = begin();
		auto l = end();
		for (; f != l && first != last; ++f, ++first) {
			*f = *first;
		}
		if (f != l) {
			erase(f, l);
		}
		else {
			insert(l, first, last);	//需要重载
		}
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

	//将x合并到this上，两个list的内容都必须先经过递增排序
	template<class T>
	void mystl::list<T>::merge(mystl::list<T>& x) {
		iterator first1 = begin();
		iterator first2 = x.begin();
		iterator last1 = end();
		iterator last2 = x.end();
		while (first1 != last1 && first2 != last2) {
			if (*first1 > *first2) {
				transfer(first1, first2, ++first2);
}
			else {
				++first1;
			}
		}
		if (first2 != last2) {
			transfer(last1, first2, last2);
		}
	}

	//将链表倒转
	template<class T>
	void mystl::list<T>::reverse() {
		if (node->next == node || node->next->next == node) {
			return;	//size()==0||size()==1
		}
		iterator first = begin();
		++first;
		while (first != end()) {
			iterator old = first;
			++first;
			transfer(begin(), old, first);
		}
	}

	//并归排序，返回一个迭代器指向区间最小元素的位置
	template<class T>
	template<class Compared>
	typename mystl::list<T>::iterator mystl::list<T>::sort(iterator first,iterator last,Compared comp) {
		size_type n = distance(first, last);
		if (n == 0) return first;
		if (n == 1) return first;
		
		/*if (n == 2) {
			if (comp(*--last, *first)) {
				transfer(first, --last, last);
				return last;	
			}
			return first;
		}*/
		//auto n = size();
		auto mid = first;
		advance(mid, n / 2);
		//auto mid = advance(first, n / 2);
		first = sort(first, mid, comp);	//前半段的最小位置
		mid = sort(mid, last, comp);	//后半段的最小位置

		first = merge_same(first, mid, last, comp);
		return first;

	}
	template<class T>
	template<class Compared>
	typename mystl::list<T>::iterator list<T>::merge_same(iterator first, iterator mid, iterator last, Compared comp) {
		
		iterator new_first;
		if (comp(*mid, *first)) {
			new_first = mid;
		}
		else {
			new_first = first;
			do {
				++first;
				if (first == mid) {
					return new_first;
				}
			} while (!comp(*mid, *first));
		}

		for (;;) {
			auto run_start = mid;
			do {
				++mid;
			} while (mid != last && comp(*mid, *first));
			transfer(first, run_start, mid);
			if (mid == last) return new_first;

			// Reestablish _Pred(_Mid->_Myval, _First->_Myval) by skipping over elements from the first
			// range already in position.
			do {
				++first;
				if (first == mid) {
					return new_first;
				}
			} while (!comp(*mid, *first));
		}
	}
}

