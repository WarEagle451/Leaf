// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Details/NullMutex.hpp>

namespace Leaf::Details
{
	struct ConsoleMutex
	{
		static std::mutex& Get()
		{
			static std::mutex s_Instance;
			return s_Instance;
		}
	};

	struct NullConsoleMutex
	{
		static NullMutex& Get()
		{
			static NullMutex s_Instance;
			return s_Instance;
		}
	};
}