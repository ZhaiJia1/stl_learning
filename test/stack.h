#pragma once
#include"deque.h"

namespace mystl {
	template<class T,class Sequence = deque<T>>
	class stack {
	public:
		typedef typename Sequence::value_type value_type;
		typedef typename Sequence::pointer pointer;
		typedef typename Sequence::reference reference;
		typedef typename Sequence::size_type size_type;
	protected:
		Sequence c;
	public:
		stack() = default;

		explicit stack(size_type n) :c(n) {}

		stack(size_type n, const value_type value) :c(n, value) {}

		/*template<class IIter>
		stack(IIter first, IIter last) : c(first, last) {}*/
		
		stack(const stack& rhs) :c(rhs.c) {}

		bool empty();
		size_type size()const { return c.size(); }
		reference top();
		void push(const value_type& value);
		void pop();

		stack& operator=(const stack& rhs) {
			c = rhs.c;
			return *this;
		}

		~stack() = default;

	};
	template<class T, class Sequence>
	inline bool stack<T, Sequence>::empty() {
		return c.empty();
	}

	template<class T, class Sequence>
	typename mystl::stack<T, Sequence>::reference stack<T, Sequence>::top() {
		return c.back();
	}

	template<class T, class Sequence>
	inline void stack<T, Sequence>::push(const value_type& value) {
		c.push_back(value);
	}

	template<class T, class Sequence>
	inline void stack<T, Sequence>::pop() {
		c.pop_back();
	}

	
}
