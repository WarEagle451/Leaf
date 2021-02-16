// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

namespace Leaf::Details
{
	struct NullMutex
	{
		void lock() {}
		void unlock() {}
		bool try_lock() { return true; }
	};
}