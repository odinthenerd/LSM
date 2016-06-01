/*==================================================================================================
Copyright (c) 2016 Odin Holmes and Carlos van Rooijen
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
=================================================================================================**/

#pragma once
#include "Mpl\brigand.hpp"

namespace LSM {
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


	template<typename T>
	struct Machine {};
}

