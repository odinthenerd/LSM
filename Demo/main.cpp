#include "stdafx.h"
#include "Chip\MKL27Z4.hpp"
#include <utility>
#include "hd44780sm.hpp"

namespace io = Kvasir::Io;
struct Config {
	static constexpr auto enable = makePinLocation(io::port0, io::pin0);
	static constexpr auto reset = makePinLocation(io::port0, io::pin1);
	static constexpr auto readWrite = makePinLocation(io::port0, io::pin2);
	//static constexpr auto port = makePort(
	//	makePinLocation(io::port0, io::pin3),
	//	makePinLocation(io::port0, io::pin4),
	//	makePinLocation(io::port0, io::pin5),
	//	makePinLocation(io::port0, io::pin6));
};

int main()
{
	auto i = makeHD44780sm(Config{});
}