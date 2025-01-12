#pragma once
#include"myiterator.h"

namespace mystl {
	template<class RandomIterator,class Distance,class T>
	void push_heap_aux(RandomIterator first, Distance holeIndex, Distance topIndex, T value) {
		auto parentIndex = (holeIndex - 1) / 2;
		while (holeIndex > topIndex && *(first + parentIndex) < value) {
			*(first + holeIndex) = *(first + parentIndex);
			holeIndex = parentIndex;
			parentIndex = (holeIndex - 1) / 2;
		}
		*(first + holeIndex) = value;
	}

	template<class RandomIterator,class Distance>
	void push_heap_d(RandomIterator first, RandomIterator last, Distance*) {
		push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
	}

	template<class RandomIterator>
	void push_heap(RandomIterator first, RandomIterator last) {
		push_heap_d(first, last, difference_type(first));
	}


	template<class RandomIterator,class Distance,class T,class Compared>
	void push_heap_aux(RandomIterator first, Distance holeIndex, Distance topIndex, T value, Compared comp) {
		auto parentIndex = (holeIndex - 1) / 2;
		while (holeIndex > topIndex && comp(*(first + parentIndex), value)) {
			*(first + holeIndex) = *(first + parentIndex);
			holeIndex = parentIndex;
			parentIndex = (holeIndex - 1) / 2;
		}
		*(first + parentIndex) = value;
	}

	template<class RandomIterator,class Distance,class Compared>
	void push_heap_d(RandomIterator first, RandomIterator last, Distance*, Compared comp) {
		push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0),*(last-1), comp);
	}

	template<class RandomIterator,class Compared>
	void push_heap(RandomIterator first, RandomIterator last, Compared comp) {
		push_heap_d(first, last, difference_type(first), comp);
	}

	template<class RandomIterator,class Distance,class T>
	void adjust_heap(RandomIterator first, Distance holeIndex, Distance len, T value) {
		auto topIndex = holeIndex;
		auto rchild = 2 * holeIndex + 2;
		while (rchild < len) {
			if (*(first + rchild) < *(first + rchild - 1)) {
				--rchild;
			}
			*(first + holeIndex) = *(first + rchild);
			holeIndex = rchild;
			rchild = 2 * holeIndex + 2;
		}
		if (rchild == len) {
			*(first + holeIndex) = *(first + (rchild - 1));
			holeIndex = rchild - 1;
		}
		mystl::push_heap_aux(first, holeIndex, topIndex, value);
	}


	template<class RandomIterator,class Distance,class T>
	void pop_heap_aux(RandomIterator first, RandomIterator last,RandomIterator result,T value,Distance*){
		*result = *first;
		mystl::adjust_heap(first, static_cast<Distance>(0), last - first, value);
	}

	template<class RandomIterator>
	void pop_heap(RandomIterator first, RandomIterator last) {
		mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), difference_type(first));
	}

	template<class RandomIterator,class T,class Distance,class Compared>
	void adjust_heap(RandomIterator first, Distance holeIndex, Distance len, T value, Compared comp) {
		auto topIndx = holeIndex;
		auto rchild = 2 * holeIndex + 2;
		while (rchild<len){
			if (comp(*(first + rchild), *(first + rchild - 1))) {
				--rchild;
			}
			*(first + holeIndex) = *(first + rchild);
			holeIndex = rchild;
			rchild = 2*holeIndex+2;
		}
		if (rchild == len) {
			*(first + holeIndex) = *(first + rchild - 1);
			holeIndex = rchild-1;
		}
		mystl::push_heap_aux(first, holeIndex, topIndx, value, comp);
	}


	template<class RandomIterator,class T,class Distance,class Compared>
	void pop_heap_aux(RandomIterator first, RandomIterator last, RandomIterator result, T value, Distance*, Compared comp) {
		*result = *first;
		mystl::adjust_heap(first, static_cast<Distance>(0), last - first, value, comp);
	}

	template<class RandomIterator,class Compared>
	void pop_heap(RandomIterator first, RandomIterator last, Compared comp) {
		mystl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), difference_type(first), comp);
	}

	template<class RandomIterator>
	void sort_heap(RandomIterator first, RandomIterator last) {
		while (last - first > 1) {
			pop_heap(first, last--);
		}
	}

	template<class RandomIterator,class Compared>
	void sort_heap(RandomIterator first, RandomIterator last, Compared comp) {
		while (last - first > 1) {
			pop_heap(first, last, comp);
		}
	}

	template<class RandomIterator,class T,class Distance>
	void make_heap_aux(RandomIterator first, RandomIterator last, T*, Distance*) {
		if (last - first < 2) return;
		Distance len = last - first;
		auto holeIndex = (len - 2) / 2;
		while (true) {
			adjust_heap(first, holeIndex, len, *(first + holeIndex));
			if (holeIndex == 0) return;
			--holeIndex;
		}
	}

	template<class RandomIterator>
	void make_heap(RandomIterator first, RandomIterator last) {
		make_heap_aux(first, last, value_type(first), difference_type(first));
	}

	template<class RandomIterator,class T,class Distance,class Compared>
	void make_heap_aux(RandomIterator first, RandomIterator last, T*, Distance*, Compared comp) {
		if (last - first < 2) return;
		Distance len = last - first;
		auto holeIndex = (len - 2) / 2;
		while (true) {
			adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
			if (holeIndex == 0) return;
			--holeIndex;
		}
	}

	template<class RandomIterator,class Compared>
	void make_heap(RandomIterator first, RandomIterator last, Compared comp) {
		make_heap_aux(first, last, value_type(first), difference_type(first),comp);
	}

	

}