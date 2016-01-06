#pragma once
#include "Mpl\brigand.hpp"

namespace Kvasir {
	namespace Sm {
		struct GuardTag {};
		constexpr GuardTag guard{};
		struct StateTag {};
		constexpr StateTag state{};
		struct EventTag {};
		constexpr EventTag event {};

		template<typename T>
		struct Guard {
			Guard(T&) {}
			Guard() = default;
		};

		template<typename T>
		struct Entry {};

		template<typename T>
		struct Exit {};

		template<typename T, typename...U>
		struct State {
			State() = default;
			template<typename T, typename...U>
			State(T, U...) {}
		};

		struct IsEventGuardTag {};

		template<typename T>
		struct IsEventGuard {
			bool operator()(T*) {
				return true;
			}
		};

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



		template<typename L>
		struct Chain;
		template<typename...T>
		struct Chain<brigand::list<T...>> {
			Chain(T...args) {}
		};
		template<typename From, typename To, typename C>
		struct Transition {
			Transition(C&) {}
		};
		template<typename... T, typename Ret = Chain<brigand::list<T...>>>
		Ret chain(T...args) {
			return Ret{ args... };
		}
		template<typename From, typename To, typename...T>
		Transition < From, To, Chain<brigand::list<T...>>> transition(From*, To*, T...args) {
			return Chain < brigand::list<T...>>{args...};
		}

		template<typename T>
		struct Machine {};

		template<typename R, typename...T>
		Machine<R> make(R r, T...args) {
			return{};
		}
	}
}