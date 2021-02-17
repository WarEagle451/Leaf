// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Details/NullMutex.hpp>

#include <mutex>

namespace Leaf::Details
{
	struct NullConsoleMutex
	{
		static NullMutex& Get()
		{
			static NullMutex Instance;
			return Instance;
		}
	};

	struct ConsoleMutex
	{
		static std::mutex& Get()
		{
			static std::mutex Instance;
			return Instance;
		}
	};

	struct NullFileMutex
	{
		static NullMutex& Get()
		{
			static NullMutex Instance;
			return Instance;
		}
	};

	struct FileMutex
	{
		static std::mutex& Get()
		{
			static std::mutex Instance;
			return Instance;
		}
	};

	struct NullBufferMutex // Needed? probs need to be per tracing sink TODO:
	{
		static NullMutex& Get()
		{
			static NullMutex Instance;
			return Instance;
		}
	};

	struct BufferMutex
	{
		static std::mutex& Get()
		{
			static std::mutex Instance;
			return Instance;
		}
	};
}