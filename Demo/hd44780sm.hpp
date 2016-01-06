#pragma once
#include "../Lib/Sm/All.hpp"
#pragma warning(disable : 4503)
#pragma warning(disable : 4700)



constexpr struct Init {} *init{};
constexpr struct Print { const char*data_; } *print{};
constexpr struct SetCursorToPosition { unsigned char x, y; } *setCursorToPosition{};
constexpr struct Timer {} *timer{};

template<typename Config>
auto makeHD44780sm(Config config) {
	struct Uninitialized {} *uninitialized;
	struct Initialized {} *initialized;
	struct WaitingForCompletion {} *waitingForCompletion;
	struct Printing { const char* begin_; int end_; } *printing;
	using namespace Kvasir::Sm;

	auto setEnableWait = [](auto& context) {
		context.enableWait();
	};
	auto setResetWait = [](auto& context) {
		context.resetWait();
	};

	auto enableWait = chain(
		setEnableWait,
		guard % event == timer
		);
	auto resetWait = chain(
		setResetWait,
		guard % event == timer
		);

	auto toggleEnable = chain(
		enableWait,
		set(Config::enable),
		enableWait,
		clear(Config::enable),
		enableWait
		);

	auto reset = chain(
		//makeOutput(Config::port), 
		clear(Config::enable, Config::readWrite, Config::reset), 
		//write(Config::port, _3),
		toggleEnable,
		resetWait,
		guard % event == init,
		toggleEnable,
		resetWait,
		guard % event == init,
		toggleEnable,
		resetWait,
		guard % event == init);

	return make(
		config, //root state
		state % uninitialized,
		state % initialized,
		state % waitingForCompletion,
		//initialize
		transition( uninitialized, initialized,
			guard % event == init,
			reset,
			//write(Config::port, _2),
			toggleEnable,
			resetWait,
			//function set
			//write(Config::port, Config::functionSetH),
			toggleEnable,
			//write(Config::port, Config::functionSetL),
			toggleEnable,
			resetWait,
			//turn off display
			//write(Config::port, _0),
			toggleEnable,
			//write(Config::port, _8),
			toggleEnable,
			resetWait,
			//clear display
			//write(Config::port, _0),
			toggleEnable,
			//write(Config::port, _1),
			toggleEnable,
			resetWait,
			//entry mode
			//write(Config::port, Config::entryModeH),
			toggleEnable,
			//write(Config::port, Config::entryModeL),
			toggleEnable,
			resetWait,
			//turn on display
			//write(Config::port, _0),
			toggleEnable,
			//write(Config::port, _8),
			toggleEnable,
			resetWait
			),
		//set cursor to position
		transition(initialized, waitingForCompletion,
			guard % event == setCursorToPosition,
			clear(Config::reset,Config::readWrite),
			//makeOutput(Config::port),
			[](SetCursorToPosition* e) { /*apply(write(Config::port, (0x80 | (e->x * 0x40 + e->y)) >> 4));*/ },
			toggleEnable,
			[](SetCursorToPosition* e) { /*apply(write(Config::port, (0x80 | (e->x * 0x40 + e->y))));*/ },
			toggleEnable
			),
		//print
		transition(initialized, printing,
			guard % [](Print* p) { return p->data_[0] != '\0'; }	//only if event is print and buf is not empty
			),
		state % printing + [](auto& c, Print* p){
				c.me.begin_ = p->data_;
				c.me.end_ = p->data_ + strlen(p->data_);
				//apply(set(Config::reset),clear(Config::readWrite), makeOutput(Config::port));
				//apply(write(Config::port, *c.me.begin_ >> 4));
				c.enableWait();
			},
		transition(printing,printing,
			guard % [](auto& c, Timer*) {c.me.begin_ != c.me.end_; }

			)
		);
}