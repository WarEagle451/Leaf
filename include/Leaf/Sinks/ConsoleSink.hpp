// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Sinks/ANSIConsoleSink.hpp>
#include <Leaf/Sinks/BasicConsoleSink.hpp>
#include <Leaf/Sinks/TrueColorConsoleSink.hpp>

namespace Leaf::Sinks
{
	// TODO: FIND OUT WHAT IS SUPPORTED PER COMPUTER FIND OUT HOW WITH PLATFORM DEFINES

	//using ConsoleSinkST = BasicConsoleSinkST;
	//using ConsoleSinkMT = BasicConsoleSinkMT;

	//using ConsoleSinkST = ANSIConsoleSinkST;
	//using ConsoleSinkMT = ANSIConsoleSinkMT;

	using ConsoleSinkST = TrueColorConsoleSinkST;
	using ConsoleSinkMT = TrueColorConsoleSinkMT;
}