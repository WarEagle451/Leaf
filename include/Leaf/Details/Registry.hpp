// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Logger.hpp>

#include <iostream>

// TODO: make this thread safe

namespace Leaf::Details
{
	class Registry
	{
	public:
		static Registry& Get()
		{
			static Registry Instance;
			return Instance;
		}
	private:
		Registry() {}
	public:
		Registry(const Registry&) = delete;
		void operator=(const Registry&) = delete;

		void Register(std::shared_ptr<Logger> logger)
		{
			for (std::shared_ptr<Logger> ptr : _Registry)
				if (ptr->_Name == logger->_Name)
					assert(!"Duplicate logger names!");
			_Registry.emplace_back(logger);
			
			size_t multithreadedSinks(0);
			for (SinkPtr sink : logger->_Sinks)
				if (multithreadedSinks >= LEAF_MAX_THREADS) // TODO make sure this wont interfere with Maple's threadpool
					break;
				else if (sink->_Multithreaded)
					multithreadedSinks++;
			ThreadPool::Get().CheckSize(multithreadedSinks);
		}
	private:
		std::vector<std::shared_ptr<Logger>> _Registry;
	};

}

namespace Leaf
{
	static void Register(std::shared_ptr<Logger> logger)
	{
		Details::Registry::Get().Register(logger);
	}
}