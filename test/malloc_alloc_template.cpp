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
	//���º������ڴ����ڴ治������
	static void* oom_malloc(size_t);
	static void* oom_realloc(void*, size_t);
	static void (* __malloc_alloc_oom_handler)();
	

public:
	static void* allocate(size_t n) {
		void* result = malloc(n);	//��һ����������ֱ��ʹ��alloc,��ͬ��ֱ��ʹ��realloc��free
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

	//���·���C++��set_new_handler(),����ͨ����ָ���Լ���out_of_memory handler
	//void(*...)()����һ���������õĽ�������ú������õĽ��������ֵ����һ��������ָ��
	static void (*set_malloc_handler(void (*f)()))(){	//set_malloc_handler����һ��ָ���޲���������Ϊvoid�ĺ�����ָ����Ϊ������������һ��ͬ�����͵ĺ���ָ��
		void (*old)() = __malloc_alloc_oom_handler;	//���浱ǰ������ָ��
		__malloc_alloc_oom_handler = f;	//�����µĴ�����ָ��
		return old;	//����֮ǰ�Ĵ�����ָ��
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
