#pragma once
#include"myiterator.h"
#include<type_traits>
namespace mystl {
	typedef bool rb_tree_color_type;
	static constexpr rb_tree_color_type rb_tree_red = false;
	static constexpr rb_tree_color_type rb_tree_black = true;

	template<class T> 
	struct rb_tree_node_base;
	template<class T>
	struct rb_tree_node;
	template<class T>
	struct rb_tree_iterator_base;
	template<class T>
	struct rb_tree_iterator;

	template<class T,bool>
	struct rb_tree_value_traits_impl {
		typedef typename T key_type;
		typedef typename T mapped_type;
		typedef typename T value_type;

		//模板中出现嵌套从属类型，最好还是加上typename，但是这里已经进行过定义了，所以不需要
		template<class Ty>
		static const key_type& get_key(const Ty& value) {
			return value;
		}

		template<class Ty>
		static const value_type& get_value(const Ty& value) {
			return value;
		}
	};

	template<class T>
	struct rb_tree_value_traits_impl<T, true> {
		typedef typename std::remove_cv<typename T::first_type>::type key_type;
		typedef typename T::second_type mapped_type;
		typedef typename T value_type;

		template<class Ty>
		static const key_type& get_key(const Ty& value) {
			return value.first;
		}

		template<class Ty>
		static const value_type& get_value(const Ty& value) {
			return value;
		}
	};

	template<class T>
	struct rb_tree_value_traits {
		static constexpr bool is_map = is_pair<T>::value;
		typedef rb_tree_value_traits_impl<T, is_map> value_traits_type;

		typedef typename value_traits_type::key_type key_type;
		typedef typename value_traits_type::mapped_type mapped_type;
		typedef typename value_traits_type::value_type value_type;

		template<class Ty>
		static const key_type& get_key(const Ty& value) {
			return value_traits_type::get_key(value);
		}

		template<class Ty>
		static const value_type& get_value(const Ty& value) {
			return value_traits_type::get_value(value);
		}
	};

	template<class T>
	struct rb_tree_node_traits {
		typedef rb_tree_color_type color_type;
		typedef rb_tree_value_traits value_traits;
		typedef typename value_traits::key_type key_type;
		typedef typename value_traits::mapped_type mapped_type;
		typedef typename value_traits::value_type value_type;

		typedef rb_tree_node_base<T>* base_ptr;
		typedef rb_tree_node<T>* node_ptr;
	};

	template<class T>
	struct rb_tree_node_base {
		typedef rb_tree_color_type color_type;
		typedef rb_tree_node_base<T>* base_ptr;
		typedef rb_tree_node<T>* node_ptr;

		base_ptr parent;
		base_ptr left;
		base_ptr right;
		color_type color;

		base_ptr get_base_ptr() {
			//return static_cast<base_ptr>(this);
			return &*this;	//是为了确保在派生类的情况，this是派生类的指针类型，那么通过&*this可以得到指向基类的指针（因为基类部分位于对象的开始部分）
		}
		node_ptr get_node_ptr() {
			return reinterpret_cast<node_ptr>(&*this);
		}

		node_ptr& get_node_ref() {
			return reinterpret_cast<node_ptr&>(&*this);
		}
	};

	template<class T>
	struct rb_tree_node :public rb_tree_node_base {
		typedef rb_tree_node_base<T>* base_ptr;
		typedef rb_tree_node<T>* node_ptr;

		T value;
		
		base_ptr get_base_ptr() {
			return static_cast<base_ptr>(&*this);
		}

		node_ptr get_node_ptr() {
			return &*this;
		}
		
	};

	template<class T>
	struct rb_tree_traits {
		typedef rb_tree_value_traits value_traits;
		typedef typename value_traits::key_type key_type;
		typedef typename value_traits::mapped_type mapped_type;
		typedef typename value_traits::value_type value_type;

		typedef value_type* pointer;
		typedef value_type& reference;
		typedef const value_type* const_pointer;
		typedef const value_type& const_reference;

		typedef rb_tree_node_base<T> base_type;
		typedef rb_tree_node<T> node_type;

		typedef base_type* base_ptr;
		typedef node_type* node_ptr;

	};

	template<class NodePtr>
	NodePtr rb_tree_min(NodePtr x) noexcept {
		while (x->left != nullptr) {
			x = x->left;
		}
		return x;
	}

	template<class NodePtr>
	NodePtr rb_tree_max(NodePtr x) noexcept {
		while (x->right != nullptr) {
			x = x->right;
		}
		return x;
	}

	template<class T>
	struct rb_tree_iterator_base:public myIterator<bidirectional_iterator_tag, T> {
		typedef typename rb_tree_node_base<T>* base_ptr;
		base_ptr node;
		rb_tree_iterator_base() :node(nullptr) {}

		void increment() {
			if (node->right != nullptr) {
				node = rb_tree_min(node->right);
			}
			else {
				base_ptr y = node->parent;
				while (y->right == node) {
					node = y;
					y = y->parent;
				}
				if (node->right != y) {
					node = y;
				}
			}
		}

		void decrement() {
			if (node->color == rb_tree_red && node->parent->parent == node) {
				node = node->right;
			}
			else if (node->left != nullptr) {
				node = rb_tree_max(node->left);
			}
			else {
				base_ptr y = node->parent;
				while (y->left == node) {
					node = y;
					y = y->parent;
				}
				node = y;
			}
		}

		bool operator==(const rb_tree_iterator_base& rhs) { return node == rhs.node; }
		bool operator!=(const rb_tree_iterator_base& rhs) { return node != rhs.node; }
	};

	template<class T>
	struct rb_tree_iterator :public rb_tree_iterator_base<T> {
		typedef rb_tree_traits<T> tree_traits;
		typedef typename tree_traits::value_type value_type;
		typedef typename tree_traits::pointer pointer;
		typedef typename tree_traits::reference reference;
		typedef typename tree_traits::const_pointer const_pointer;
		typedef typename tree_traits::const_reference const_reference;
		typedef typename tree_traits::base_ptr base_ptr;
		typedef typename tree_traits::node_ptr node_ptr;

		typedef rb_tree_iterator<T> self;
		typedef rb_tree_iterator<T> iterator;
		
		using rb_tree_iterator_base<T>::node;

		rb_tree_iterator(){}
		rb_tree_iterator(base_ptr x) { node = x; }
		rb_tree_iterator(node_ptr x) { node = x; }
		rb_tree_iterator(const iterator& rhs) { node = rhs.node; }
		
		reference operator*() const{
			return node->get_node_ptr()->value;
		}
		pointer operator->() const {
			return &(operator*());
		}

		self& operator++() {
			this->increment();
			return *this;
		}

		self operator++(int) {
			self tmp(*this);
			this->increment();
			return tmp;
		}

		self& operator--() {
			this->decrement();
			return *this;
		}

		self operator--(int) {
			self tmp(*this);
			this->decrement();
			return tmp;
		}
	};
}