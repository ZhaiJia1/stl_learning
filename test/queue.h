#pragma once
#include"deque.h"

namespace mystl {
	template<class T, class Sequence = deque<T>>
	class queue {
	public:
		typedef typename Sequence::value_type value_type;
		typedef typename Sequence::pointer pointer;
		typedef typename Sequence::reference reference;
		typedef typename Sequence::size_type size_type;

	protected:
		Sequence c;
	public:
		queue() = default;
		explicit queue(const size_type n):c(n){}
		queue(const size_type n,const value_type& value):c(n,value){}
		queue(const queue& rhs):c(rhs.c){}

		bool empty() { return c.empty(); }
		size_type size()const { return c.size(); }
		reference front() { return c.front(); }
		reference back() { return c.back(); }
		void push(const value_type& value) { c.push_back(value); }
		void pop() { c.pop_front(); }

		reference operator=(const queue& rhs) {
			c = rhs.c;
			return *this;
		}
		

	};
}