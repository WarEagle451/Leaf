// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <mutex>

namespace Leaf::Details
{
	struct NullMutex
	{
		void lock() {}
		void unlock() {}
		bool try_lock() { return true; }
	};
}