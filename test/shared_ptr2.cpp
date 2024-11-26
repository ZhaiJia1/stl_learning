#include<iostream>
template<typename T>
class RefCount {
private:
	std::atomic<size_t> refcount_;
public:
	RefCount():refcount_(1){}
	~RefCount(){}

	void inc() {
		++refcount_;
	}
	bool dec_and_test() {
		return --refcount_ = 0;
	}

	size_t getcount() {
		return refcount_.load();
	}
};

template<typename T>
class m_shared_ptr {
private:
	T* ptr_;
	RefCount<T>* refcount_;

public:
	m_shared_ptr() :ptr_(nullptr), refcount_(nullptr) {}
	m_shared_ptr(T* ptr) :ptr_(ptr), refcount_(new RefCount<T>()) {
		if (!ptr) {
			refcount_->inc();
		}
	}
	m_shared_ptr(const m_shared_ptr& ptr) :ptr_(ptr->ptr_), refcount_(ptr->refcount_) {
		if (ptr != nullptr) {
			refcount_->inc();
		}
	}
	m_shared_ptr& operator=(const m_shared_ptr& ptr) {
		if (ptr != this) {
			if (refcount_->dec_and_test()) {
				delete ptr_;
				delete refcount_;
			}

			ptr_ = ptr.ptr_;
			refcount_ = ptr.refcount_;

			if (refcount_) {
				refcount_->inc();
			}
		}
		return *this;
	}

	m_shared_ptr(m_shared_ptr&& ptr) :ptr_(ptr->ptr_), refcount_(ptr->refcount_) {
		ptr.ptr_ = nullptr;
		ptr.refcount_ = nullptr;
	}

	m_shared_ptr& operator=(m_shared_ptr&& ptr) {
		if (&ptr != this) {
			if (refcount_->dec_and_test()) {
				ptr_ = nullptr;
				refcount_ = nullptr;
			}
			ptr_ = ptr.ptr_;
			refcount_ = ptr.refcount_;
			ptr.ptr_ = nullptr;
			ptr.refcount_ = nullptr;
		}
		return *this;
	}

	~m_shared_ptr() {
		if(&ptr_!=nullptr)
		{
			if (refcount_->dec_and_test()) {
				delete ptr_;
				delete refcount_;
			}
			
		}
		ptr_ = nullptr;
		refcount_ = nullptr;
	}
};