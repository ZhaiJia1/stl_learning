#pragma once
#include "heap_algo.h"
#include"myvector.h"
namespace mystl {
	template<class T,class Sequence = mystl::vector<T>,class Compared = std::less<typename Sequence::value_type>>
	class priority_queue {
	public:
		typedef typename Sequence::value_type value_type;
		typedef typename Sequence::size_type size_type;
		typedef typename Sequence::reference reference;
		typedef typename Sequence::const_reference const_reference;
	protected:
		Sequence c;//容器
		Compared comp;//元素比较的方法
	public:
		priority_queue() :c() {}
		explicit priority_queue(const Compared& x) :c(), comp(x) {}

		template<class InputIterator>
		priority_queue(InputIterator first, InputIterator last) : c(first, last) { mystl::make_heap(c.begin(), c.end(), comp); }

		template<class InputIterator>
		priority_queue(InputIterator first, InputIterator last, const Compared& x) : c(first, last), comp(x) { mystl::make_heap(c.begin(), c.end(), comp); }

		bool empty() { return c.empty(); }
		size_type size() { return c.size(); }
		const_reference top() { return c.front(); }
		void push(const T& value) {
			try {
				c.push_back(value);
				mystl::push_heap(c.begin(), c.end(), comp);
			}
			catch (...) {
				c.clear();
			}
		}

		void pop() {
			try {
				mystl::pop_heap(c.begin(), c.end(), comp);
				c.pop_back();
			}
			catch (...) {
				c.clear();
			}
		}
	};
}