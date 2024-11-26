#if 0
#include<new>
#define _THROW_BAD_ALLOC throw bad_alloc
#elif !defined(_THROW_BAD_ALLOC)
#include<iostream>
#define _THROW_BAD_ALLOC std::cerr<<"out of memory"<<std::endl; exit(1)
#endif

template<int inst>
class _malloc_alloc_template {
private:
	//以下函数用于处理内存不足的情况
	static void* oom_malloc(size_t);
	static void* oom_realloc(void*, size_t);
	static void (* __malloc_alloc_oom_handler)();
	

public:
	static void* allocate(size_t n) {
		void* result = malloc(n);	//第一级配置器，直接使用alloc,下同，直接使用realloc，free
		if (result == 0) result = oom_malloc(n);
		return result;
	}

	static void* deallocate(void* p, size_t /* n */) {
		free(p);
	}

	static void* reallocate(void* p, size_t /* old_sz */, size_t new_sz) {
		void* result = realloc(p, new_sz);
		if (result == 0) result = oom_realloc;
		return result;
	}

	//以下仿真C++的set_new_handler(),可以通过它指定自己的out_of_memory handler
	//void(*...)()这是一个函数调用的结果，而该函数调用的结果（返回值）是一个函数的指针
	static void (*set_malloc_handler(void (*f)()))(){	//set_malloc_handler接受一个指向无参数，返回为void的函数的指针作为参数，并返回一个同样类型的函数指针
		void (*old)() = __malloc_alloc_oom_handler;	//保存当前处理函数指针
		__malloc_alloc_oom_handler = f;	//设置新的处理函数指针
		return old;	//返回之前的处理函数指针
	}
};

template<int inst>
void (*_malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

//template<int inst>
//void* _malloc_alloc_template<inst>::oom_malloc(size_t n)
//{
//	void (*my_malloc_handler)();
//	void* result;
//	for (;;) {
//		my_malloc_handler = __malloc_alloc_oom_handler;
//		if (my_malloc_handler == 0) { _THROW_BAD_ALLOC; }
//		(*my_malloc_handler)();
//		result = malloc(n);
//		if (result) return (result);
//	}
//}

template<int inst>
void* _malloc_alloc_template<inst>::oom_realloc(void* p, size_t n)
{
	void (*my_malloc_handler)();
	void* result;
	for (;;) {
		my_malloc_handler = __malloc_alloc_oom_handler;
		if (my_malloc_handler == 0) { _THROW_BAD_ALLOC; }
		(*my_malloc_handler)();
		result = realloc(p, n);
		if (result) return (result);
	}
}

typedef _malloc_alloc_template<0> malloc_alloc;
