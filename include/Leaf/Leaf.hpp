// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Version.hpp>
#include <Leaf/Details/Registry.hpp>

#define LEAF_LOGGER

namespace Leaf
{
	static void Register(std::shared_ptr<Logger> logger)	{ Details::Registry::Get().Register(logger); }
	static void Deregister(std::string_view loggerName)		{ Details::Registry::Get().Deregister(loggerName); }

	static std::shared_ptr<Logger> Get(std::string_view loggerName)	{ Details::Registry::Get().GetLogger(loggerName); }
	static void SetPattern(std::string_view pattern)				{ Details::Registry::Get().SetPattern(pattern); }
	static void SetLevel(Severity severity)							{ Details::Registry::Get().SetLevel(severity); }
}