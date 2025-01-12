#pragma once
#include"myiterator.h"

#include"malloc_alloc_template.h"
#include"construct.h"
#include<memory>
#include<climits>
namespace mystl {
	template<class T>
	class vector {
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef value_type& reference;
		typedef value_type* iterator;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef const value_type& const_reference;
	protected:
		typedef _malloc_alloc_template<T> data_allocator;
		iterator start_;
		iterator end_;
		iterator cap_;

		void insert_aux(iterator position, const T& x);
		void deallocate() {
			if (start_) {
				data_allocator::deallocate(start_, cap_ - start_);
			}
		}
		
		void fill_initialize(size_type n, const T& value){
			start_ = allocate_and_fill(n, value);
			end_ = start_ + n;
			cap_ = end_;
		}

		iterator allocate_and_fill(size_type n, const T& value) {
			iterator result =  (iterator)data_allocator::allocate(n);
			std::uninitialized_fill_n(result, n, value);//先用标准库的
			return result;
		}
		
		template<class InputIterator>
		void range_init(InputIterator first, InputIterator last) {
			difference_type len = distance(first, last);
			iterator result = (iterator)data_allocator::allocate(len);
			std::uninitialized_copy(first, last, result);
			start_ = result;
			end_ = result + len;
			cap_ = end_;
		}

	public:
		iterator begin() { return start_; }
		iterator end() { return end_; }
		size_type size() { return (size_type)(end() - begin()); }
		size_type capacity()const {
			return size_type(cap_ - begin());
		}
		bool empty()const {
			return end() == begin();
		}
		reference operator[](size_type n) {
			return *(begin() + n);
		}

		//构造
		vector():start_(0),end_(0),cap_(0){}
		vector(size_type n, const T& value) {
			fill_initialize(n, value);
		}
		vector(int n, const T& value) {
			fill_initialize(n, value);
		}
		vector(long n, const T& value) {
			fill_initialize(n, value);
		}
		explicit vector(size_type n) {
			fill_initialize(n, T());
		}
		template<class InputIterator>
		vector(InputIterator first, InputIterator last) {
			range_init(first, last);
		}

		~vector() {
			mystl::destroy(start_, end_);
			deallocate();
		}

		reference  front() { return *begin(); }
		reference back() { return *(end() - 1); }
		void push_back(const T& value) {
			if (end_ != cap_) {
				mystl::construct(end_, value);
				++end_;
			}
			else {
				insert_aux(end(), value);
			}
		}
		void pop_back() {
			--end_;
			mystl::destroy(end_);
		}

		iterator erase(iterator position) {
			//清除某个位置上的元素
			if (position + 1 != end_) {
				std::copy(position + 1, end_, position);//copy暂时先用标准库的，后续再定义
			}
			--end_;
			mystl::destroy(end_);
			return position;
		}

		//清除[first,last)间的所有元素
		iterator erase(iterator first, iterator last) {
			iterator i = std::copy(last, end_, first);
			mystl::destroy(i, end_);
			size_type n = last - first;
			end_ -= n;
			return first;
		}

		void resize(size_type new_sz, const T& x) {
			if (new_sz < size()) {
				erase(begin() + new_sz, end());
			}
			else {
				insert(end(), new_sz-size(), x);
			}
		}

		void resize(size_type new_sz) {
			resize(new_sz, T());
		}

		void clear() {
			erase(begin(), end());
		}

		void insert(iterator position, size_type n, const T& x);
		
		//reallocate
		template<class... Args>
		void reallocate_emplace(iterator position, Args&& ...args);

		template<class... Args>
		void emplace_back(Args&& ...args);

	};
	
	//没有备用空间
	template<class T>
	inline void vector<T>::insert_aux(iterator position, const T& x)
	{
		if (end_ != cap_) {	//还有别的函数调用insert_aux
			construct(end_,*(end_-1));
			++end_;
			T x_copy = x;
			std::copy_backward(position, end_ - 2, end_ - 1);
			*position = x_copy;
		}
		else {
			const size_type old_size = size();
			const size_type len = old_size != 0 ? 2 * old_size : 1;
			iterator new_start = data_allocator::allocate(len);
			iterator new_end = new_start;
			try {
				new_end = std::uninitialized_copy(start_, position, new_start);
				mystl::construct(new_end, x);
				++new_end;
				new_end = std::uninitialized_copy(position, end_, new_end);
			}
			catch (const std::exception& e) {
				mystl::destroy(new_start,new_end);
				data_allocator::deallocate(new_start, len);
				throw e;
			}
			mystl::destroy(begin(), end());
			deallocate();
			start_ = new_start;
			end_ = new_end;
			cap_ = start_ + len;
			
		}
	}

	template<class T>
	inline void vector<T>::insert(iterator position, size_type n, const T& x)
	{
		if (n != 0) {
			//备用空间大于待插入的空间
			if (static_cast<size_type>(cap_ - end_) >= n) {
				T x_copy = x;//参数是引用类型，考虑到覆盖？
				//计算插入点之后的元素数量
				const size_type elems_after = end_ - position;
				iterator old_end = end_;
				//插入点后的元素个数大于新增元素个数
				if (elems_after > n) {
					std::uninitialized_copy(end_ - n, end_, end_);
					end_ += n;
					std::copy_backward(position, old_end - n, old_end);
					std::fill(position, position + n, x_copy);
				}
				else {
					std::uninitialized_fill_n(end_, n - elems_after, x_copy);
					end_ += n - elems_after;
					//std::uninitialized_copy(position, old_end+elems_after, end_);
					std::uninitialized_copy(position, old_end, end_);
					end_ += elems_after;
					std::fill(position, old_end, x_copy);
				}
			}
			//空间不足，配置新空间
			else {
				const size_type old_size = size();

				const size_type len = old_size + std::max(old_size,n);
				iterator new_start = data_allocator::allocate(len);
				iterator new_end = new_start;
				try {
					new_end = std::uninitialized_copy(start_, position, new_start);
					new_end = std::uninitialized_fill_n(new_end, n, x);
					new_end = std::uninitialized_copy(position, end_, new_end);
				}
				catch (...) {
					//rollback
					mystl::destroy(new_start, new_end);
					data_allocator::deallocate(new_start, len);
					throw ;
				}
				//释放旧空间
				mystl::destroy(start_, end_);
				deallocate();
				start_ = new_start;
				end_ = new_end;
				cap_ = new_start + len;
			}
		}
	}

	//重新分配空间并在position处构造一个新元素
	template<class T>
	template<class ...Args>
	inline void vector<T>::reallocate_emplace(iterator position, Args && ...args)
	{
		size_type old_size = size();
		size_type len = old_size != 0 ? 2 * old_size : 1;
		iterator new_start = data_allocator::allocate(len);
		iterator new_end = new_start;

		try {
			new_end = std::uninitialized_copy(start_, position, new_start);
			mystl::construct(new_end, std::forward<Args>(args)...);
			++new_end;
			new_end = std::uninitialized_copy(position, end_, new_end);
		}
		catch (const std::exception& e) {
			data_allocator::deallocate(new_start, new_end - new_start);
			throw e;
		}
		mystl::destroy(start_, end_);
		deallocate();
		start_ = new_start;
		end_ = new_end;
		cap_ = new_start + len;//后续优化
	}

	template<class T>
	template<class ...Args>
	inline void vector<T>::emplace_back(Args && ...args)
	{
		if (end_ != cap_) {
			
			mystl::construct(end_, std::forward<Args>(args)...);
			++end_;
		}
		else {
			
			reallocate_emplace(end_, std::forward<Args>(args)...);
		}
	}

}
