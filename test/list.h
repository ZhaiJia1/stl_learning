#pragma once
#include"myiterator.h"
#include"malloc_alloc_template.h"
namespace mystl {
	//�ڵ�
	template<class T>
	struct __list_node {
		typedef __list_node<T>* pointer;
		pointer prev;	//Ҳ��������Ϊ__list_node<T>*
		pointer next;
		T data;
	};

	//������
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
		link_type node;//ָ��list�ڵ�
		
		//construct
		__list_iterator(){}
		__list_iterator(link_type x):node(x){}
		__list_iterator(const iterator& x):node(x.node){}

		//operator
		bool operator==(const self& x)const { return node == x.node; }
		bool operator!=(const self& x)const { return node != x.node; }
		reference operator*() const { return (*node).data; }//ȡ�ڵ�����ֵ
		pointer operator->()const { return &(operator*()); }

		self& operator++() {
			node = (link_type)((*node).next);
			return *this;
		}

		//����++�����������Ҫһ��int��������ǰ�ý������֣����ҷ��ص��Ƕ����һ������
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

	//����
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
		
		//get,put,create,destory����Ӧ���Ƕ�Ӧ�ģ��Ȼ�ȡnode�Ŀռ䣬�ٹ���node��data
		//destroyʱ���Ȱ�node��data���������ͷ�node�Ŀռ�
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

		//=����������
		list& operator=(const list& rhs) {
			if(this!=&rhs)	//ͨ���жϵ�ַ�Ƿ���ͬ���ж��Ƿ����
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
		//����������ز���
		void assign(size_type n, const value_type& value) {
			fill_assign(n, value);
		}
		
		//enable_if��typename��::typeһ��ʹ�ã��Է��ʵ�����Ϊ��ʱ��������ͣ�����Ϊ��ʱ�ǵڶ���ģ�����
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

		//��i��ָԪ�ؽ����position��ָλ��֮ǰ
		void splice(iterator position, list&, iterator i) {
			iterator j = i;
			++j;
			if (position == i || position == j) return;//���i����position�������i��������positionǰ����������
			transfer(position, i, j);
		}

		//position����λ��first��last֮�䣬�Ƿ�Ҫдһ������ȷ��һ��   ��ע������Ҫ����Ϊ���м�Ҳ���ԣ�list�ĵ���������ʧЧ
		void splice(iterator position, list&, iterator first, iterator last) {
			if (first != last) {
				transfer(position, first, last);
			}
		}

		void merge(list& x);
		void reverse();

		//����ʱ�����нڵ�Ŀռ��ͷţ����ͷ�node
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
		//����дfunctional�ٸĽ�
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

	//��positionǰ����first��last�ķ�Χ
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

	//�Ƴ�position�ڵ㣬���صĵ�����ָ��position��һ���ڵ�
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

	//�Ƴ���ֵ��ͬ������Ԫ��
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
			insert(l, first, last);	//��Ҫ����
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

	//��x�ϲ���this�ϣ�����list�����ݶ������Ⱦ�����������
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

	//������ת
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

	//�������򣬷���һ��������ָ��������СԪ�ص�λ��
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
		first = sort(first, mid, comp);	//ǰ��ε���Сλ��
		mid = sort(mid, last, comp);	//���ε���Сλ��

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

