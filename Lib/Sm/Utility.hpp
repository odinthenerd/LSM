/*==================================================================================================
Copyright (c) 2016 Odin Holmes and Carlos van Rooijen
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
=================================================================================================**/

#pragma once
#include "Types.hpp"

namespace Kvasir {
	namespace Sm {
		namespace Detail {
			template<typename... T>
			void sink(T...) {}
			template<typename...T, typename M, typename E>
			typename T::IndexType onEvent(brigand::list<T...>, M* machine, E* event) {
				sink(T{}(machine, event)...);
			}
		}
	}
}
