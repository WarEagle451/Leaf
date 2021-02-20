// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Common.hpp>

namespace Leaf
{
	struct LogMessage
	{
		Severity Level;
		std::string Message;
	};
}

namespace Leaf::Details
{
	struct Payload
	{
		LogMessage Log;
		std::string_view LoggerName;
	};
}