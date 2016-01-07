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