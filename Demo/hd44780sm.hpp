#pragma once
#include "../Lib/Sm/All.hpp"
#pragma warning(disable : 4503)
#pragma warning(disable : 4700)



struct Init {};
struct Print { const char*data_; };
struct SetCursorToPosition { unsigned char x, y; };
struct Timer {};

template<typename Config>
auto makeHD44780sm(Config config) {
	struct Printing { const char* begin_; int end_; };
	using namespace LSM;

	auto setEnableWait = [](auto& context) {
		context.enableWait();
	};
	auto setResetWait = [](auto& context) {
		context.resetWait();
	};

	auto enableWait = chain(
		setEnableWait,
		guard = event == timer
		);
	auto resetWait = chain(
		setResetWait,
		guard = event == timer
		);

	auto toggleEnable = chain(
		enableWait,
		set(Config::enable),
		enableWait,
		clear(Config::enable),
		enableWait
		);

	auto reset = chain(
		makeOutput(Config::port), 
		clear(Config::enable, Config::readWrite, Config::reset), 
		write(Config::port, 3_c),
		toggleEnable,
		resetWait,
		guard = event == init,
		toggleEnable,
		resetWait,
		guard = event == init,
		toggleEnable,
		resetWait,
		guard = event == init);

	return make(
		config, //root state
		"Uninitialized"_s,
		"Initialized"_s,
		"WaitingForCompletion"_s,
		//initialize
		transition( "Uninitialized"_s, "Initialized"_s,
			guard = event == init,
			reset,
			write(Config::port, 2_c),
			toggleEnable,
			resetWait,
			//function set
			write(Config::port, Config::functionSetH),
			toggleEnable,
			write(Config::port, Config::functionSetL),
			toggleEnable,
			resetWait,
			//turn off display
			write(Config::port, 0_c),
			toggleEnable,
			write(Config::port, 8_c),
			toggleEnable,
			resetWait,
			//clear display
			write(Config::port, 0_c),
			toggleEnable,
			write(Config::port, 1_c),
			toggleEnable,
			resetWait,
			//entry mode
			write(Config::port, Config::entryModeH),
			toggleEnable,
			write(Config::port, Config::entryModeL),
			toggleEnable,
			resetWait,
			//turn on display
			write(Config::port, 0_c),
			toggleEnable,
			write(Config::port, 8_c),
			toggleEnable,
			resetWait
			),
		//set cursor to position
		transition("Initialized"_s, "WaitingForCompletion"_s,
			guard = event == setCursorToPosition,
			clear(Config::reset,Config::readWrite),
			makeOutput(Config::port),
			[](SetCursorToPosition& e) { apply(write(Config::port, (0x80 | (e->x * 0x40 + e->y)) >> 4)); },
			toggleEnable,
			[](SetCursorToPosition& e) { apply(write(Config::port, (0x80 | (e->x * 0x40 + e->y)))); },
			toggleEnable
			),
		"Printing"_s(Printing(), entry = [](auto& c, Print* p){
				c.me.begin_ = p->data_;
				c.me.end_ = p->data_ + strlen(p->data_);
				apply(set(Config::reset),clear(Config::readWrite), makeOutput(Config::port));
				apply(write(Config::port, *c.me.begin_ >> 4));
				c.enableWait();
			}),
		//print
		transition("Initialized"_s, "Printing"_s,
			guard = [](Print* p) { return p->data_[0] != '\0'; }	//only if event is print and buf is not empty
			),
		transition("Printing"_s,guard = [](auto& c, Timer*) {c.me.begin_ != c.me.end_; }),
		transition("Printing"_s,"Initialized"_s,guard = [](auto& c, Timer*) {c.me.begin_ == c.me.end_; })
		);
}
