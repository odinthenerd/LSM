/*==================================================================================================
Copyright (c) 2016 Odin Holmes and Carlos van Rooijen
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
=================================================================================================**/
#pragma once
#include "Types.hpp"

namespace LSM {

	template<typename T>
	Guard<T> operator %(const GuardTag&, T& rhs) { return Guard<T>{rhs}; }

	IsEventGuardTag operator %(const GuardTag&, const EventTag&) { return{}; }

	template<typename T>
	Guard<IsEventGuard<T>> operator ==(const IsEventGuardTag&, const T*) { return{}; }

	template<typename T>
	State<T> operator %(const StateTag&, const T&) {
		return{};
	}

	template<typename T, typename...U, typename V>
	State<T, U..., Entry<V>> operator +(const State<T, U...>& s, const V e) {
		return State<T, U..., Entry<V>>{s, e};
	}

	template<typename T, typename...U, typename V>
	State<T, U..., Exit<V>> operator -(const State<T, U...>& s, const V e) {
		return State<T, U..., Exit<V>>{s, e};
	}

	template<typename... T, typename Ret = Chain<brigand::list<T...>>>
	Ret chain(T...args) {
		return Ret{ args... };
	}

	template<typename From, typename To, typename...T>
	Transition < From, To, Chain<brigand::list<T...>>> transition(From*, To*, T...args) {
		return Chain < brigand::list<T...>>{args...};
	}

	template<typename R, typename...T>
	Machine<R> make(R r, T...args) {
		return{};
	}
}
