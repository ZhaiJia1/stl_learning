#include<iostream>
template<typename T>
class RefCount {
public:
	RefCount():count_(1){}
	~RefCount() {
		std::cout << "ref_count destroyed" << std::endl;
	}
	void inc() {
		++count_;
	}
	bool dec_and_test() {
		return --count_ == 0;
	}
	size_t get_count() const{
		return count_.load();
	}
private:
	std::atomic<size_t> count_;
};

template<typename T>
class Shared_ptr
{
public:
	//构造函数
	Shared_ptr():pointer(nullptr),ref_count(nullptr){}
	explicit Shared_ptr(T* ptr):pointer(ptr),ref_count(new RefCount<T>()){
		if (!ptr) {
			ref_count->inc();
		}
	}

	//拷贝构造函数
	Shared_ptr(const Shared_ptr& other_) :pointer(other_.pointer), ref_count(other_.ref_count) {
		if(pointer)
			ref_count->inc();
	}

	//移动构造函数
	Shared_ptr(Shared_ptr&& other_) noexcept :pointer(other_.pointer), ref_count(other_.ref_count) {
		other_.pointer = nullptr;
		other_.ref_count = nullptr;
	}

	//拷贝赋值运算符重载
	Shared_ptr& operator = (const Shared_ptr& other_) {
		if (this != &other_) {
			if (ref_count->dec_and_test()) {
				delete pointer;
				delete ref_count;
			}
			pointer = other_.pointer;
			ref_count = other_.ref_count;
			if (ref_count) {
				ref_count->inc();
			}
			
		}
		return *this;
	}

	//移动赋值运算符重载
	Shared_ptr& operator = (Shared_ptr&& other_) noexcept {
		if (this != &other_) {
			pointer = other_.pointer;
			ref_count = other_.ref_count;
			other_.pointer = nullptr;
			other_.ref_count = nullptr;
		}
		return *this;
	}

	~Shared_ptr() {
		if (pointer != nullptr) {
			if (ref_count->dec_and_test()) {
				delete pointer;
				delete ref_count;
			}
		}
		pointer = nullptr;
		ref_count = nullptr;
		std::cout << "shared_ptr destroyed" << std::endl;
	}

	size_t use_count() const {
		return ref_count ? ref_count->get_count(): 0;
	}

	//解引用
	T& operator *() {
		return *pointer;
	}

	T* get() {
		return pointer;
	}

	T* operator ->() {
		return pointer;
	}

private:
	RefCount<T>* ref_count;
	T* pointer;
};

template<typename T, typename... Args>
Shared_ptr<T> make_shared(Args&&... args) {
	return Shared_ptr<T>(new T(std::forward<Args>(args)...));
}

/*int main() {
	//构造函数
	Shared_ptr<int> sp1 = make_shared<int>(5);
	std::cout << *sp1 << std::endl;
	std::cout << sp1.use_count() << std::endl;
	Shared_ptr<int> sp2 = make_shared<int>(2);
	sp2 = sp1;
	std::cout << sp1.use_count() << std::endl;
	std::cout << sp2.use_count() << std::endl;
	Shared_ptr<int> sp3 = std::move(sp2);
	std::cout << sp1.use_count() << std::endl;
	std::cout << sp2.use_count() << std::endl;
	std::cout << sp3.use_count() << std::endl;
	
	return 0;
}*/

