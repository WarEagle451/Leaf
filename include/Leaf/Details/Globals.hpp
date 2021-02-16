// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Details/NullMutex.hpp>

#include <mutex>

namespace Leaf::Details
{
	struct NullConsoleMutex
	{
		static NullMutex& GetInstance()
		{
			static NullMutex Instance;
			return Instance;
		}
	};

	struct ConsoleMutex
	{
		static std::mutex& GetInstance()
		{
			static std::mutex Instance;
			return Instance;
		}
	};

	struct NullFileMutex
	{
		static NullMutex& GetInstance()
		{
			static NullMutex Instance;
			return Instance;
		}
	};

	struct FileMutex
	{
		static std::mutex& GetInstance()
		{
			static std::mutex Instance;
			return Instance;
		}
	};

	struct NullBufferMutex
	{
		static NullMutex& GetInstance()
		{
			static NullMutex Instance;
			return Instance;
		}
	};

	struct BufferMutex
	{
		static std::mutex& GetInstance()
		{
			static std::mutex Instance;
			return Instance;
		}
	};
}