// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Common.hpp>

#include <thread>
#include <future>
#include <vector>

/// TODO: actually do

namespace Leaf::Details
{
	class ThreadPool
	{
	public:
	private:
		std::vector<std::thread> _Threads;
	};
}