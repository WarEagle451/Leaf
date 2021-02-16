// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include "Log.h"

#include <Leaf/Sinks/ConsoleSink.hpp>
#include <Leaf/Sinks/BasicFileSink.hpp>

namespace Example
{
	Ref<Leaf::Logger> Log::s_CoreLogger;

	void Log::Init()
	{
		std::vector<Leaf::SinkPtr> sinks;
		sinks.emplace_back(std::make_shared<Leaf::Sinks::ConsoleSinkMT>());
		sinks.emplace_back(std::make_shared<Leaf::Sinks::BasicFileSinkMT>("Example.log"));

		sinks[0]->SetPattern("[%T] %N: %m");
		sinks[1]->SetPattern("[%T] [%L] %N: %m");

		s_CoreLogger = std::make_shared<Leaf::Logger>("Example", sinks.begin(), sinks.end());
		s_CoreLogger->SetLevel(Leaf::Severity::Trace);

		Leaf::Register(s_CoreLogger);
	}
}