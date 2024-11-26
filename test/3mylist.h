#pragma once

template<class T>
class ListItem {
	
public:
	ListItem<T>() {};
	ListItem<T>(T value) :value_(value), next_(nullptr) {};
	ListItem* next_;
	T value_;
	T value() { return value_; }
	ListItem* next() { return next_; }
};

template<class T>
class List {
public:
	List<T>():front_(nullptr),end_(nullptr),size_(0) {};

	void insert_front(T value) {
		ListItem<T>* item = new ListItem<T>(value);
		if (front_ == nullptr) {
			front_ = item;
			end_ = item;
		}
		else {
			item->next_ = front_;
			front_ = item;
		}
	}

	void insert_end(T value) {
		ListItem<T>* item = new ListItem<T>(value);
		end_->next_ = item;
		end_ = end_->next_;
	}

	void display(std::ostream& os = std::cout)const {
		ListItem<T>* cur = front_;
		while (cur != nullptr) {
			os << cur->value_ << " ";
			cur = cur->next_;
		}
		os << std::endl;
	}
private:
	ListItem<T>* front_;
	ListItem<T>* end_;
	long size_;
};

