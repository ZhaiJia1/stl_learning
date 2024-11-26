#pragma once
#include<cstddef>
namespace mystl {
	
	template<class T>
	typename std::remove_reference<T>::type&& move(T&& arg)noexcept {
		return static_cast<typename std::remove_reference<T>::type&&>(arg);
	}

	template<class T>
	T&& forward(typename std::remove_reference<T>::type& arg)noexcept {
		return static_cast<T&&>(arg);
	}

	template <class T>
	T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
	{
		static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
		return static_cast<T&&>(arg);
	}
}