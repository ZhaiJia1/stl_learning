#pragma once
#include"myiterator.h"
#include"malloc_alloc_template.h"
namespace mystl {

#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif // !DEQUE_MAP_INIT_SIZE


	template<class T>
	struct deque_buf_size {
		static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
	};

	template<class T,class Ptr,class Ref>
	struct deque_iterator :public myIterator<_random_access_iterator_tag, T> {
		typedef deque_iterator<T, T*, T&> iterator;
		typedef deque_iterator self;
		typedef deque_iterator<T, const T*, const T&> const_iterator;

		typedef T value_type;
		typedef Ptr pointer;
		typedef Ref reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* value_pointer;
		typedef T** map_pointer;
		static const size_type buffer_size = deque_buf_size<T>::value;

		value_pointer cur;	//ָ�����ڻ������ĵ�ǰԪ��
		value_pointer first;	//ָ��ǰ��������ͷ��
		value_pointer last;	//ָ��ǰ��������β��
		map_pointer node;	//ָ��ǰ��������ڽڵ�

		//���캯��
		deque_iterator()noexcept :cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
		deque_iterator(value_pointer v, map_pointer m) :cur(v), first(*m), last(*m + buffer_size), node(m) {}
		deque_iterator(const iterator& rhs) :
			cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
		deque_iterator(iterator&& rhs)noexcept:
			cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {
			rhs.cur = nullptr;
			rhs.first = nullptr;
			rhs.last = nullptr;
			rhs.node = nullptr;
		}
		deque_iterator(const const_iterator& rhs) :
			cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

		//���������
		self& operator=(const deque_iterator& rhs) {
			if (this != &rhs) {
				cur = rhs.cur;
				first = rhs.first;
				last = rhs.last;
				node = rhs.node;
			}
			return *this;
		}
		//ת����һ��������
		void set_node(map_pointer m) {
			node = m;
			first = *node;
			last = *node + buffer_size;
		}

		reference operator*()const {
			return *cur;
		}
		pointer operator->()const {
			return cur;
		}

		difference_type operator-(const self& x)const {
			//return static_cast<difference_type>(buffer_size)*(node-x.node-1)+(cur-first)+(x.last-x.cur);
			return static_cast<difference_type>(buffer_size) * (node - x.node) + (cur - first) - (x.cur - 
				x.first);
		}

		self& operator++() {
			++cur;
			if (cur == last) {
				set_node(node + 1);
				cur = first;
			}
			return *this;
		}

		self operator++(int) {
			self tmp = *this;
			++*this;
			return tmp;
		}

		self& operator--() {
			if (cur == first) {
				set_node(node - 1);
				cur = last;
			}
			--cur;
			return *this;
		}
		self operator--(int) {
			self tmp = *this;
			--*this;
			return tmp;
		}

		self& operator+=(difference_type n) {
			difference_type offset = n + (cur - first);
			if (offset >= 0 && offset < static_cast<difference_type>(buffer_size)) {
				//���ڵ�ǰ��������
				cur += n;
			}
			else {
				difference_type node_offset = offset > 0 ? offset / static_cast<difference_type>(buffer_size) 
					: -static_cast<difference_type>((-offset - 1) / buffer_size) - 1;
				set_node(node + node_offset);
				cur = first + (offset - (node_offset) * static_cast<difference_type>(buffer_size));
			}
			return *this;
		}

		self operator+(difference_type n)const {
			self tmp = *this;
			return tmp += n;
		}

		self& operator-=(difference_type n) {
			return *this += -n;
		}

		self operator-(difference_type n)const {
			self tmp = *this;
			return tmp -= n;
		}

		reference operator[](difference_type n)const{
			return *(*this + n);//(*cur)
		}

		bool operator==(const self& rhs) {
			return cur == rhs.cur;
		}
		bool operator<(const self& rhs) {
			return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node);
		}
		bool operator!=(const self& rhs) {
			return !(*this == rhs);
		}
		bool operator>(const self& rhs) {
			return rhs<*this;
		}
		bool operator<=(const self& rhs) {
			return !(*this > rhs);
		}
		bool operator>=(const self& rhs) {
			return !(*this < rhs);
		}
	};

	template<class T>
	class deque {
	public:
		typedef _malloc_alloc_template<T> data_allocator;
		typedef _malloc_alloc_template<T*> map_allocator;
		typedef deque_iterator<T, T*, T&> iterator;
		typedef typename iterator::value_type value_type;
		typedef typename iterator::pointer pointer;
		typedef typename iterator::reference reference;
		typedef typename iterator::size_type size_type;
		typedef typename iterator::difference_type difference_type;
		typedef pointer* map_pointer;
		static const size_type buffer_size = deque_buf_size<T>::value;
	
	private:
		iterator start_;	//ָ���һ���ڵ�
		iterator end_;	//ָ�����һ���ڵ�
		map_pointer map_;	//ָ��һ��map��map�е�ÿ��Ԫ�ض���һ��ָ�룬ָ��һ��������
		size_type map_size;	//map��ָ�������

	public:
		void create_map_and_nodes(size_type n) {
			size_type num_nodes = n / buffer_size + 1;	//��Ҫ����������
			map_size = max(num_nodes + 2, DEQUE_MAP_INIT_SIZE);
			try {
				map_ = create_map(map_size);
			}
			catch (...) {
				map_ = nullptr;
				map_size = 0;
				throw;
			}

			map_pointer nstart = map_ + (map_size - num_nodes) / 2;
			map_pointer nend = nstart + num_nodes-1;
			map_pointer cur;
			try {
				for (cur = nstart; cur <= nend; ++cur) {
					*cur = data_allocator::allocate(buffer_size);
				}
			}
			catch (...) {
				map_allocator::deallocate(map_, map_size);
				map_ = nullptr;
				map_size = 0;
				throw;
			}
			start_.set_node(nstart);
			end_.set_node(nend);
			start_.cur = start_.first;
			end_.cur = end_.first + (n % buffer_size);
		}

		

		void fill_init(size_type n, const value_type& value) {
			create_map_and_nodes(n);
			map_pointer cur;
			for (cur = start_.node; cur < end_.node; ++cur) {
				std::uninitialized_fill(*cur, *cur + buffer_size, value);
			}
			std::uninitialized_fill(end_.first, end_.cur, value);
		}
		template<class IIter>
		void copy_init(IIter, IIter, _input_iterator_tag);
		template<class FIter>
		void copy_init(FIter, FIter, _forward_iterator_tag);

		//���캯��
		deque() {
			fill_init(0, value_type());
		}
		deque(size_type n, const value_type& value) {
			fill_init(n, value);
		}
		explicit deque(size_type n) {
			fill_init(n, value_type());
		}
		deque(const deque& rhs) {
			copy_init(rhs.start_, rhs.end_, _forward_iterator_tag());
		}

		deque(deque&& rhs)noexcept:start_(mystl::move(rhs.start_))
									,end_(mystl::move(rhs.end_))
									,map_(rhs.map_)
									,map_size(rhs.map_size){
			rhs.map_ = nullptr;
			rhs.map_size = 0;
		}


		~deque() {
			if (map_ != nullptr) {
				clear();
				//clear֮��ֻʣ��һ��������������ֻ��Ҫ����һ���������ռ��ͷŵ��Ϳ���
				data_allocator::deallocate(*start_.node, buffer_size);
				*start_.node = nullptr;
				map_allocator::deallocate(map_, map_size);
				map_ = nullptr;
			}
		}

		iterator begin()noexcept { return start_; }
		iterator end()noexcept { return end_; }
		size_type size()const noexcept {
			return end_ - start_;
		}
		size_type max_size()const noexcept {
			return static_cast<size_type>(-1);
		}

		bool empty() {
			return begin() == end();
		}
		reference front() {
			return *start_;
		}
		reference back() {
			return *(end_ - 1);
		}
		reference operator[](size_type n) {
			return start_[n];
		}

		

		void push_back(const value_type& value) {
			if (end_.cur != end_.last - 1) {
				//ֱ����cur��λ�ù����½ڵ�
				data_allocator::construct(end_.cur, value);
				++end_.cur;
			}
			else {
				require_capacity(1, false);
				data_allocator::construct(end_.cur, value);
				++end_;
			}
		}

		void push_front(const value_type& value) {
			if (start_.cur != start_.first) {
				data_allocator::construct(start_.cur - 1, value);
				--start_.cur;
			}
			else {
				//��push_back��ͬ���ǣ�end_.curָ��������һ��Ԫ�ص���һ��λ�ã����Կ���ֱ��������
				//���죬���¼��˿ռ�󣬵�ǰҲ����ԭ��������λ�ã���Ȼ����ֱ�ӹ���
				//����start_.curָ��ľ��ǵ�һ��Ԫ�أ��������¼��˿ռ����Ҫ��ǰ�ƶ�һ��Ԫ�أ�
				//�϶�������value_type��ԭ��ָ��ȥ�ƶ�������Ҫ����start_-1,iterator�Ѿ�������--������
				require_capacity(1, true);
				try {
					--start_;
					data_allocator::construct(start_.cur, value);
				}
				catch (...) {
					++start_;
					throw;
				}
			}
		}


		void pop_back() {
			if (empty()) { perror("deque is empty!"); return; }
			if (end_.cur != end_.first) {
				--end_.cur;
				data_allocator::destroy(end_.cur);
			}
			else {
				--end_;
				data_allocator::destroy(end_.cur);
				destroy_buffer(end_.node + 1, end_.node + 1);
			}
		}
		void pop_front() {
			if (empty()) { perror("deque is empty!"); return; }
			if (start_.cur != start_.last) {
				data_allocator::destroy(start_.cur);
				++start_.cur;
			}
			else {
				data_allocator::destroy(start_.cur);
				++start_;
				destroy_buffer(start_.node - 1, start_.node - 1);
			}
		}

		void shrink_to_fit();
		void clear();
		iterator erase(iterator pos);
		iterator erase(iterator first, iterator last);

		map_pointer create_map(size_type n);
		void create_buffer(map_pointer nstart,map_pointer nend);
		void destroy_buffer(map_pointer first, map_pointer last);
		
		void require_capacity(size_type n, bool front);
		
		void reallocate_map_at_back(size_type need_buffer);
		void reallocate_map_at_front(size_type need_buffer);


	};

	//��С������������������ͷ��������
	template<class T>
	inline void deque<T>::shrink_to_fit() {
		for (auto cur = map_; cur < start_.node; ++cur) {
			data_allocator::deallocate(*cur, buffer_size);
			*cur = nullptr;
		}
		for (auto cur = end_.node + 1; cur < map_ + map_size; ++cur) {
			data_allocator::deallocate(*cur, buffer_size);
			*cur = nullptr;
		}
	}

	template<class T>
	inline void deque<T>::clear() {
		for (map_pointer cur = start_.node + 1; cur < end_.node; ++cur) {
			data_allocator::destroy(*cur, *cur + buffer_size);
		}
		if (start_.node == end_.node) {
			data_allocator::destroy(start_.cur, end_.cur);
		}
		else {
			data_allocator::destroy(start_.cur, start_.last);
			data_allocator::destroy(end_.first, end_.cur);
		}
		shrink_to_fit();
		end_ = start_;
	}

	template<class T>
	typename mystl::deque<T>::iterator deque<T>::erase(iterator pos) {
		iterator next = pos;
		++next;
		const size_type elems_before = pos - start_;
		if (elems_before < (size() / 2)) {
			std::copy_backward(start_, pos, next);
			pop_front();
		}
		else {
			std::copy(next, end_, pos);
			pop_back();
		}
		return start_ + elems_before;
	}

	template<class T>
	typename mystl::deque<T>::iterator deque<T>::erase(iterator first, iterator last) {
		if (first == start_ && last == end_) {
			clear();
			return end_;
		}
		else {
			const size_type len = last - first;
			const size_type elems_before = first - start_;
			if (elems_before < ((size() - len) / 2)) {
				std::copy_backward(start_, first, last);
				auto new_start = start_ + len;
				data_allocator::destroy(start_.cur, new_start.cur);
				start_ = new_start;
			}
			else {
				std::copy(last, end_, first);
				auto new_end = end_ - len;
				data_allocator::destroy(new_end.cur, end_.cur);
				end_ = new_end;
			}
			return start_ + elems_before;
		}
	}

	//�����˴�СΪn��map,��map�е�ÿ����������Ϊ��,n*sizeof(value_type*)
	template<class T>
	typename mystl::deque<T>::map_pointer deque<T>::create_map(size_type n){
		map_pointer mp = nullptr;
		mp = map_allocator::allocate(n);
		for (size_type i = 0; i < n; ++i) {
			*(mp + i) = nullptr;
		}
		return mp;
	}

	//�������ڵ��е����л�������ÿ������������buffer_size*sizeof(value_type)
	template<class T>
	void deque<T>::create_buffer(map_pointer nstart, map_pointer nend) {
		map_pointer cur;
		try {
			for (cur = nstart; cur <= nend; ++cur) {
				*cur = data_allocator::allocate(buffer_size);
			}
		}
		catch (...) {
			while (cur != nstart) {
				--cur;
				data_allocator::deallocate(*cur, buffer_size);
				cur = nullptr;
			}
			throw;
		}
	}

	template<class T>
	inline void deque<T>::destroy_buffer(map_pointer first, map_pointer last) {
		for (map_pointer cur = first; cur <= last; ++cur) {
			data_allocator::deallocate(*cur, buffer_size);
			*cur = nullptr;
		}
	}

	template<class T>
	void deque<T>::require_capacity(size_type n, bool front) {
		if (front && (static_cast<size_type>(start_.cur - start_.first) < n)) {
			const size_type need_buffer = (n - (start_.cur - start_.first)) / buffer_size + 1;
			if (need_buffer > static_cast<size_type>(start_.node - map_)) {
				reallocate_map_at_front(need_buffer);
				return;
			}
			create_buffer(start_.node - need_buffer, start_.node - 1);
		}
		else if (!front && (static_cast<size_type>(end_.last - end_.cur - 1) < n)) {
			const size_type need_buffer = (n-(end_.last-end_.cur-1))/buffer_size+1;
			if (need_buffer > static_cast<size_type>((map_ + map_size) - end_.node - 1)) {
				reallocate_map_at_back(need_buffer);
				return;
			}
			create_buffer(end_.node + 1, end_.node + need_buffer);
		}
	}

	template<class T>
	inline void deque<T>::reallocate_map_at_back(size_type need_buffer) {
		const size_type new_map_size = max(map_size << 1, map_size + need_buffer + DEQUE_MAP_INIT_SIZE);
		map_pointer new_map = create_map(new_map_size);
		const size_type old_buffer = end_.node - start_.node + 1;
		const size_type new_buffer = old_buffer + need_buffer;
		auto begin = new_map + (new_map_size - new_buffer) / 2;
		auto mid = begin + old_buffer;
		auto end = begin + new_buffer;
		for (auto begin1 = begin, begin2 = start_.node; begin1 != mid; ++begin1, ++begin2) {
			*begin1 = *begin2;
		}
		create_buffer(mid, end-1);
		map_allocator::deallocate(map_, map_size);
		map_ = new_map;
		map_size = new_map_size;
		start_ = iterator(*begin + (start_.cur - start_.first), begin);
		end_ = iterator(*(mid - 1) + (end_.cur - end_.first), mid - 1);
	}

	template<class T>
	void deque<T>::reallocate_map_at_front(size_type need_buffer) {
		const size_type new_map_size = max(map_size << 1, map_size + need_buffer + DEQUE_MAP_INIT_SIZE);
		map_pointer new_map = create_map(new_map_size);
		const size_type old_buffer = end_.node - start_.node + 1;//��buffer������
		const size_type new_buffer = old_buffer + need_buffer;	//��buffer������

		auto begin = new_map + (new_map_size - new_buffer) / 2;	//�������м�
		auto mid = begin + need_buffer;	//Ҫ�¼ӵ�buffer
		auto end = mid + old_buffer;
		create_buffer(begin, mid - 1);	//�����¼ӵ�buffer
		//��ԭ����buffer���Ƶ�mid֮��
		for (auto begin1 = mid, begin2 = start_.node; begin1 != end; ++begin1, ++begin2)
			*begin1 = *begin2;
		map_allocator::deallocate(map_, map_size);
		map_ = new_map;
		map_size = new_map_size;
		//begin_��end_��curָ���λ��Ϊԭλ�ã�����ָ���node����
		start_ = iterator(*mid + (start_.cur - start_.first), mid);
		end_ = iterator(*(end - 1) + (end_.cur - end_.first), end - 1);
	}

	template<class T>
	template<class IIter>
	inline void deque<T>::copy_init(IIter first, IIter last, _input_iterator_tag) {
		const size_type n = distance(first, last);
		create_map_and_nodes(n);
		for (; first != last; ++first) {
			emplace_back(*first);
		}
	}

	template<class T>
	template<class FIter>
	inline void deque<T>::copy_init(FIter first, FIter last, _forward_iterator_tag) {
		const size_type n = distance(first, last);
		create_map_and_nodes(n);
		for (auto cur = start_.node; cur < end_.node; ++cur) {
			auto next = first;
			advance(next, buffer_size);
			std::uninitialized_copy(first, next, *cur);//next���Ʋ�������������ֱ����first=next������
			first = next;
		}
		std::uninitialized_copy(first, last, end_.first);
	}

}