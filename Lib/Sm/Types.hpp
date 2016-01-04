#pragma once
#include "Mpl\brigand.hpp"

namespace Kvasir {
	namespace Sm {
		namespace Detail {
			struct DoNothing {
				template<typename T>
				void operator()(T&){}
			};
		}
		template<typename TTag, typename TEntry, typename TExit>
		struct InputState : TEntry, TExit {
			InputState(TEntry a, TExit b) :TEntry{ a }, TExit{ b } {}
		};
		template<typename...Ts>
		struct SubStates : Ts... {
			SubStates(Ts...args) :Ts{ args }...{}
		};
	}
}