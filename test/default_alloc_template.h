#pragma once
#include"malloc_alloc_template.h"
enum {__ALIGN = 8};
enum {__MAX_BYTES = 128};
enum {__NFREFFLISTS = __MAX_BYTES/__ALIGN};

template<bool threads,int inst>
class _default_alloc_template {
private:
	//���ֽ�����ȡ�����߽��������
	static size_t ROUND_UP(size_t bytes) {
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}
private:
	union obj {
		union obj* free_list_link; //ָ��obj�������ָ�룬�������������obj������һ��
		char* client_data[1];	//����ǿ������ת����ʹ���ڴ�ʱһ��ʹ��char*ָ�룬
		//�����Ҫʹ��obj�ĵ�ַ����Ҫ���ϣ�char*��obj����ǿ������ת��,���ڿ���ֱ��ʹ��obj->client_data
	};

private:
	static obj* volatile free_list[__NFREFFLISTS];
	//���º������������С������ʹ�õ�n��free_list
	static size_t FREELIST_INDEX(size_t bytes) {
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}
	//����һ����СΪn�Ķ��󣬲����ܼ����СΪn���������鵽free list
	static void* refill(size_t n);
	//����һ���ռ䣬������nobjs����СΪsize������
	//�������nobjs�������������㣬nobjs���ܻή��
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
		my_free_list = free_list + FREELIST_INDEX(n);	//�ҵ�free_list��Ӧ������
		q->free_list_link = *my_free_list;	//Ҫ�����ڴ�������free_list_linkָ���ҵ�������
		*my_free_list = q;	//��n��Ӧ����������Ϊ���յ����飬���뵽free_list��
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
	char* chunk = chunk_alloc(n, nobjs);//nobjs�����ô���
	obj* volatile* my_free_list;//һ��ָ��obj* volatile��ָ��
	obj* result;
	obj* current_obj, * next_obj;
	if (nobjs == 1) return (chunk);
	my_free_list = free_list + FREELIST_INDEX(n);

	result = (obj*)chunk;
	//��һ��chunk���ظ��Ͷ�ʹ��
	//*my_free_list��һ��obj*,�������Ǵ��������
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
		//������20�������ǹ���һ�����ϵ������ڴ�
		result = start_free;
		nobjs = (int)(bytes_left / size);
		total_bytes = size * nobjs;
		start_free += total_bytes;
		return (result);
	}
	else {
		//һ�鶼�����֣��ȶ��ڴ�Ҫ
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
		//�ȿ��ڴ�����Ƿ��������Լӵ�free_list��
		if (bytes_left > 0) {
			obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
			((obj*)start_free)->free_list_link = *my_free_list;
			*my_free_list = (obj*)start_free;
		}
		//����heap�ռ䣬���������ڴ��
		start_free = (char*)malloc(bytes_to_get);
		if (start_free == 0) {
			//�ڴ�ռ䲻��
			//�ҵ�������δ�����飬�����鹻�󡱵�free_list
			obj* volatile* my_free_list, * p;
			size_t i;
			//i��size��ʼ�����Ի�ȡ���������С�϶����㹻�����
			for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
				my_free_list = free_list + FREELIST_INDEX(i);
				p = *my_free_list;
				if (p != 0) {
					*my_free_list = p->free_list_link;
					start_free = (char*)p;
					end_free = start_free + i;
					return (chunk_alloc(size, nobjs));//�ӱ��free_list�����л�ȡ��һ���ּӸ��ڴ�أ����Կ������·�����

				}
			}
			end_free = 0;//ʵ��û�÷���,�õ�һ�����������ԣ���oom���ƣ�
			start_free = (char*)malloc_alloc::allocate(bytes_to_get);
		}
		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;
		return chunk_alloc(size, nobjs);//�Ӷ��ϻ�ȡ���ռ���ڴ�غ󣬵ݹ����
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
	copy_sz = new_sz > old_sz ? old_sz : new_sz;	//ȡ��С����
	memcpy(result, p, copy_sz);
	deallocate(p, old_sz);
	return (result);
}

typedef _default_alloc_template<false, 0> alloc;
