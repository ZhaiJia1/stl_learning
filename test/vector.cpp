#include<stdlib.h>
#include<iostream>

namespace mystl {
	template<class T>
	class vector {
	public:
		typedef T value_type;
	private:
		value_type* begin_;
		value_type* end_;
		value_type* cap_;
	public:
		inline value_type* begin() {
			return begin_;
		}
		inline value_type* end() {
			return end_;
		}
		size_t size() const {
			return end_ - begin_;
		}

	public:
		void init_copy(value_type* begin, value_type* end, value_type* target);
		void fill_init(size_t n, const value_type& value);
		void try_init();
		void range_init(value_type* begin, value_type* end);
		
		/*
		无参，vector(size),vector(size,value),拷贝，移动构造函数
		*/
		
		vector() noexcept {
			try_init();
		}

		explicit vector(size_t n) {
			fill_init(n,value_type());
		}

		vector(size_t n, value_type value) {
			fill_init(n, value);
		}

		
		vector(const vector& rhs) {
			range_init(rhs.begin_, rhs.end_);
		}

		vector(vector&& rhs) noexcept:begin_(rhs.begin_),end_(rhs.end_),cap_(rhs.cap_) {
			rhs.begin_ = nullptr;
			rhs.end_ = nullptr;
			rhs.cap_ = nullptr;
		}

		vector& operator=(const vector& rhs);
		vector& operator=(vector&& rhs);
		
		
		
	};

	

	template<class T>
	void vector<T>::init_copy(value_type* begin, value_type* end, value_type* target) {
		auto cur = target;
		for (auto x = begin; x != end; ++x) {
			*cur = *x;
			cur++;
		}
		return;
	}

	template<class T>
	void vector<T>::fill_init(size_t n, const value_type& value) {
		bool expand = false;
		value_type* ptr = nullptr;
		if (n < 16) {
			ptr = (value_type*)malloc(16 * sizeof(value_type));
		}
		else {
			ptr = (value_type*)malloc(n * 2 * sizeof(value_type));
			expand = true;

		}
		if (ptr == nullptr) {
			perror("内存分配失败");
			return;
		}
		begin_ = ptr;
		end_ = ptr + n;
		for (value_type* v = begin_; v != end_; v++) {
			*v = value;
		}
		if (expand) {
			cap_ = begin_ + n * 2;
		}
		else cap_ = begin_ + 16;
		return;
	}
	

	template<class T>
	void vector<T>:: try_init() {
		value_type* ptr = (value_type*)malloc(16 * sizeof(value_type));
		if (ptr == nullptr) {
			perror("内存分配失败");
			return;
		}
		begin_ = ptr;
		end_ = ptr;
		cap_ = ptr + 16;
		return;
	}

	template<class T>
	void vector<T>::range_init(value_type* begin, value_type* end) {
		size_t len = end - begin;
		size_t init_size = len > static_cast<value_type>(16) ? len : static_cast<value_type>(16);
		value_type* ptr = (value_type*)malloc(init_size);
		if (ptr == nullptr) {
			perror("内存分配失败");
			return;
		}
		init_copy(begin, end, ptr);
		begin_ = ptr;
		end_ = ptr + len;
		cap_ = ptr + init_size;
		return;
	}

	template<class T>
	vector<T>& vector<T>::operator=(const vector& rhs) {
		if (this == std::addressof(rhs)) {
			return *this;
		}
		auto begin = begin_;
		auto rbegin = rhs.begin_;
		size_t len = size();
		size_t rlen = rhs.size();
		//size_t rlen = rhs.end_ - rhs.begin_;
		size_t capacity = cap_-begin_;
		
		vector tmp(rhs);
		auto orign = begin;
		begin_ = tmp.begin_;
		end_ = tmp.end_;
		cap_ = tmp.cap_;
		delete[] orign;
		std::cout << "赋值运算符重载" << std::endl;
		return *this;
	}

	template<class T>
	vector<T>& vector<T>::operator=(vector&& rhs) {
		if (this == std::addressof(rhs)) {
			return *this;
		}
		vector tmp(rhs);
		auto orign = begin_;
		begin_ = tmp.begin_;
		end_ = tmp.end_;
		cap_ = tmp.cap_;
		delete[] orign;
		rhs.begin_ = nullptr;
		rhs.end_ = nullptr;
		rhs.cap_ = nullptr;
		
		std::cout << "移动赋值运算符重载" << std::endl;
		return *this;
	}
	
	

	
}


