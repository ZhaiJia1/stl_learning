#pragma once
#include"malloc_alloc_template.h"
enum {__ALIGN = 8};
enum {__MAX_BYTES = 128};
enum {__NFREFFLISTS = __MAX_BYTES/__ALIGN};

template<bool threads,int inst>
class _default_alloc_template {
private:
	//将字节向上取整到边界的整数倍
	static size_t ROUND_UP(size_t bytes) {
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}
private:
	union obj {
		union obj* free_list_link; //指向obj联合体的指针，可以用来将多个obj连接在一起
		char* client_data[1];	//避免强制类型转换？使用内存时一般使用char*指针，
		//如果需要使用obj的地址，需要加上（char*）obj进行强制类型转换,现在可以直接使用obj->client_data
	};

private:
	static obj* volatile free_list[__NFREFFLISTS];
	//以下函数根据区块大小，决定使用第n号free_list
	static size_t FREELIST_INDEX(size_t bytes) {
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}
	//返回一个大小为n的对象，并可能加入大小为n的其他区块到free list
	static void* refill(size_t n);
	//配置一大块空间，可容纳nobjs个大小为size的区块
	//如果配置nobjs个区块有所不便，nobjs可能会降低
	static char* chunk_alloc(size_t size, int& nobjs);

	static char* start_free;
	static char* end_free;
	static size_t heap_size;

public:
	static void* allocate(size_t n) {
		
		obj* volatile* my_free_list;
		obj* result;
		if (n > (size_t)__MAX_BYTES) {
			return (malloc_alloc::allocate(n));
		}
		my_free_list = free_list + FREELIST_INDEX(n);
		result = *my_free_list;
		if (result == 0) {
			void* r = refill(ROUND_UP(n));
			return r;
		}
		*my_free_list = result->free_list_link;
		
		return (result);
	}
	static void deallocate(void* p, size_t n) {
		obj* q = (obj*)p;
		obj* volatile* my_free_list;
		if (n > (size_t)__MAX_BYTES) {
			malloc_alloc::deallocate(p, n);
		}
		my_free_list = free_list + FREELIST_INDEX(n);	//找到free_list对应的区块
		q->free_list_link = *my_free_list;	//要回收内存的区块的free_list_link指向找到的区块
		*my_free_list = q;	//把n对应的区块设置为回收的区块，加入到free_list中
	}
	static void* reallocate(void* p, size_t old_sz, size_t new_sz);
};

template<bool threads, int inst>
char* _default_alloc_template<threads, inst>::start_free = 0;

template<bool threads, int inst>
char* _default_alloc_template<threads, inst>::end_free = 0;

template<bool threads, int inst>
size_t _default_alloc_template<threads, inst>::heap_size = 0;

template<bool threads,int inst>
typename _default_alloc_template<threads, inst>::obj* volatile _default_alloc_template<threads, inst>::free_list[__NFREFFLISTS] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

template<bool threads, int inst>
inline void* _default_alloc_template<threads, inst>::refill(size_t n)
{
	int nobjs = 20;
	char* chunk = chunk_alloc(n, nobjs);//nobjs是引用传参
	obj* volatile* my_free_list;//一个指向obj* volatile的指针
	obj* result;
	obj* current_obj, * next_obj;
	if (nobjs == 1) return (chunk);
	my_free_list = free_list + FREELIST_INDEX(n);

	result = (obj*)chunk;
	//第一个chunk返回给客端使用
	//*my_free_list是一个obj*,解引用是从右往左的
	*my_free_list = next_obj = (obj*)(chunk + n);
	for (int i = 1; ; ++i) {
		current_obj = next_obj;
		next_obj = (obj*)((char*)next_obj + n);
		if (nobjs - 1 == i) {
			current_obj->free_list_link = 0;
			break;
		}
		else {
			current_obj->free_list_link = next_obj;
		}
	}
	return (result);

}

template<bool threads, int inst>
inline char* _default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs)
{
	char* result;
	size_t total_bytes = size * nobjs;
	size_t bytes_left = end_free - start_free;
	if (bytes_left >= total_bytes) {
		result = start_free;
		start_free += total_bytes;
		return (result);
	}
	else if (bytes_left >= size) {
		//不够分20个，但是够分一个以上的区块内存
		result = start_free;
		nobjs = (int)(bytes_left / size);
		total_bytes = size * nobjs;
		start_free += total_bytes;
		return (result);
	}
	else {
		//一块都不够分，先堆内存要
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
		//先看内存池中是否有碎块可以加到free_list中
		if (bytes_left > 0) {
			obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
			((obj*)start_free)->free_list_link = *my_free_list;
			*my_free_list = (obj*)start_free;
		}
		//配置heap空间，用来补充内存池
		start_free = (char*)malloc(bytes_to_get);
		if (start_free == 0) {
			//内存空间不足
			//找到“尚有未用区块，且区块够大”的free_list
			obj* volatile* my_free_list, * p;
			size_t i;
			//i从size开始，所以获取到的区块大小肯定是足够分配的
			for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (p != 0) {
					*my_free_list = p->free_list_link;
					start_free = (char*)p;
					end_free = start_free + i;
					return (chunk_alloc(size, nobjs));//从别的free_list区块中获取了一部分加给内存池，所以可以重新分配了

				}
			}
			end_free = 0;//实在没得分了,用第一级配置器试试（有oom机制）
			start_free = (char*)malloc_alloc::allocate(bytes_to_get);
		}
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;
		return chunk_alloc(size, nobjs);//从堆上获取到空间给内存池后，递归调用
	}
}

template<bool threads, int inst>
inline void* _default_alloc_template<threads, inst>::reallocate(void* p, size_t old_sz, size_t new_sz)
{
	if (old_sz > (size_t)__MAX_BYTES && new_sz > (size_t)__MAX_BYTES) {
		malloc_alloc::reallocate(p, new_sz);
	}
	void* result;
	size_t copy_sz;
	if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) { return p; }
	result = allocate(new_sz);
	copy_sz = new_sz > old_sz ? old_sz : new_sz;	//取最小容量
	memcpy(result, p, copy_sz);
	deallocate(p, old_sz);
	return (result);
}

typedef _default_alloc_template<false, 0> alloc;
