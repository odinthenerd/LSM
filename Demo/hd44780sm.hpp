#pragma once


constexpr struct Init {} *init{};
constexpr struct Print { const char*data_; } *print{};
constexpr struct SetCursorToPosition { unsigned char x, y; } *setCursorToPosition{};
constexpr struct Timer {} *timer{};

template<typename Config>
auto makeHD44780sm(Config config) {
	struct Uninitialized {} *uninitialized;
	struct Initialized {} *initialized;
	struct WaitingForCompletion {} *waitingForCompletion;
	using namespace Kvasir::Sm;

	auto setEnableWait = [](auto& context) {
		context.enableWait();
	};
	auto setResetWait = [](auto& context) {
		context.resetWait();
	};

	auto enableWait = chain(
		enableWait,
		guard % event = timer
		);
	auto resetWait = chain(
		resetWait,
		guard % event = timer
		);

	auto toggleEnable = chain(
		enableWait,
		set(Config::pin),
		enableWait,
		clear(Config::pin),
		enableWait
		);

	auto reset = chain(
		makeOutput(Config::port), 
		clear(Config::enable, Config::readWrite, Config::reset), 
		write(Config::port, _3),
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
			write(Config::port, _2),
			toggleEnable,
			resetWait,
			//function set
			write(Config::port, Config::functionSetH),
			togggleEnable,
			write(Config::port, Config::functionSetL),
			toggleEnable,
			resetWait,
			//turn off display
			write(Config::port, _0),
			togggleEnable,
			write(Config::port, _8),
			toggleEnable,
			resetWait,
			//clear display
			write(Config::port, _0),
			togggleEnable,
			write(Config::port, _1),
			toggleEnable,
			resetWait,
			//entry mode
			write(Config::port, Config::entryModeH),
			togggleEnable,
			write(Config::port, Config::entryModeL),
			toggleEnable,
			resetWait,
			//turn on display
			write(Config::port, _0),
			togggleEnable,
			write(Config::port, _8),
			toggleEnable,
			resetWait
			)
		//set cursor to position
		transition(initialized,waitForCompletion,  
			guard % event == setCursorToPosition,
			clear(Config::reset,Config::readWrite),
			makeOutput(Config::port),
			[](SetCursorToPosition* e) { apply(write(Config::port, (0x80 | (e->x * 0x40 + e->y)) >> 4)); },
			toggleEnable,
			[](SetCursorToPosition* e) { apply(write(Config::port, (0x80 | (e->x * 0x40 + e->y)))); },
			toggleEnable,
			)
		);
}