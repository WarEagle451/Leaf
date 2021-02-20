// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Leaf.hpp>
#include <Leaf/Sinks/BasicFileSink.hpp>

namespace Leaf::Examples
{
	static void CallExampleLogger()
	{
		std::vector<Leaf::SinkPtr> sinks;
		sinks.emplace_back(std::make_shared<Leaf::Sinks::BasicFileSinkST>("Example.log"));
		sinks[0]->SetPattern("[%T] [%S] %N: %M");

		std::shared_ptr<Leaf::Logger> logger = std::make_shared<Leaf::Logger>("Example Logger", sinks.begin(), sinks.end());

		Leaf::Register(logger);

		logger->Trace("Trace level example");
		logger->Debug("Debug level example");
		logger->Info("Info level example");
		logger->Warn("Warn level example");
		logger->Error("Error level example");
		logger->Fatal("Fatal level example");

		Leaf::Deregister("Example Logger");
	}
}