#include <iostream>
#include<type_traits>
//#include"default_alloc_template.h"
//#include"3mylist.h"
//#include"myiterator.h"
#include"myvector.h"
#include<vector>
#include"construct.h"
#include<windows.h>
#include"list.h"
#include"deque.h"
#include"stack.h"
#include"queue.h"
#include"heap_algo.h"
#include"priority_queue.h"
#include"rb_tree.h"
using namespace std;

class A {
    int i;
};

class B {
public:
    B() {};
    B(int value):i(value){}
    int i;
    ~B() {};
};


//template<class T>
//void func(const T& t) {
//    typedef typename std::is_trivially_constructible<T> trivial_constructor;
//    func_aux(t, trivial_constructor());
//}
//template<class T>
//void func_aux(const T& t, std::false_type) {
//    cout << "false type" << endl;
//}
//template<class T>
//void func_aux(const T& t, std::true_type) {
//    cout << "true_type" << endl;
//}

//template<class T1, class T2>
//void construct(T1* p, const T2& value) {
//    cout << "construct" << endl;
//}
//
//template <class T>
//void destroy(T* p) {
//    p->~T();
//    cout << "here" << endl;
//}
//
//
////有non-trivial destructor
//template<class l>
//void __destroy_aux(l first, std::false_type) {
//    cout << "destroy(false_type)" << endl;
//    destroy(first);
//}
////有trivial destructor,无关痛痒，不用做任何操作
//template<class l>
//void __destroy_aux(l first, std::true_type) {
//    cout << "destroy(true_type)" << endl;
//}
//
//
//////判断元素的数值型别（value_type)是否有trivial_destructor
////template<class l, class T>
////void __destroy(l first, T*) {
////	typedef typename std::is_trivially_destructible<T> trivial_destructor;
////	__destroy_aux(first, trivial_destructor());
////}
//
//
//
//template<class l>
//void destroy(l first,l last) {
//    typedef typename _iterator_traits<l>::value_type value_type;
//    typedef typename std::is_trivially_destructible<value_type> trivial_destructor;
//    __destroy_aux(first, trivial_destructor());
//}


template<class T, T v>
struct test_tem {
    static constexpr T value = v;
};

template<int x>
using m_int_test = test_tem<int, x>;
int main() {
    cout << m_int_test<5>::value << endl;
    
	return 0;
}

