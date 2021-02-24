// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Details/StringBuilder.hpp>

namespace Leaf::Sinks
{
	class Sink
	{
	public:
		virtual void Log(const Details::Payload& payload) = 0;

		void SetPattern(std::string_view pattern) { _StrBuilder.SetPattern(pattern); }
		void SetLevel(Severity severity) { _Level.store(severity, std::memory_order_relaxed); }
		bool Loggable(Severity severity) { return severity >= _Level.load(std::memory_order_relaxed); }
	protected:
		a_Severity _Level{ Severity::Trace };
		Details::StringBuilder _StrBuilder;
	};
}
namespace Leaf { using SinkPtr = std::shared_ptr<Sinks::Sink>; }