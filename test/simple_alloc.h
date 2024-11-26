#pragma once
#include"default_alloc_template.h"
#include"malloc_alloc_template.h"

template<class T,class Alloc>
class simple_alloc {
public:
	static T* allocate(size_t);
	static T* allocate(void);
	static void deallocate(T*, size_t);
	static void deallocate(T*);
};

template<class T, class Alloc>
inline T* simple_alloc<T, Alloc>::allocate(size_t n)
{
	//分配n个T类型空间
	return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
}

template<class T, class Alloc>
inline T* simple_alloc<T, Alloc>::allocate(void)
{
	return (T*)Alloc::allocate(sizeof(T));
}

template<class T, class Alloc>
inline void simple_alloc<T, Alloc>::deallocate(T* i, size_t n)
{
	if (0 != n) Alloc::deallocate(i, n * sizeof(T));
}

template<class T, class Alloc>
inline void simple_alloc<T, Alloc>::deallocate(T* i)
{
	Alloc::deallocate(i,sizeof(T));
}



