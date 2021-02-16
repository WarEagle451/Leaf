// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Details/Log.hpp>
#include <Leaf/Severity.hpp>

#include <memory>
#include <assert.h>

namespace Leaf::Details
{
	class Registry;
}

namespace Leaf::Sinks
{
	class Sink
	{
		friend class Details::Registry;
	public:
		Sink(bool multithreaded) :
			_Multithreaded(multithreaded),
			_LogLevel(Severity::Trace) {}

		virtual void Log(const Details::Log& log) = 0;
		//virtual Flush() = 0;
		void SetPattern(std::string_view pattern)
		{
			VerifyPattern(pattern);
			_Pattern = pattern;
		}
		//virtual void SetFormatter(std::unique_ptr<formatter> sinkFormatter) = 0;
		//void SetLevel(Severity lvl);
		//bool ShouldLog(Severity lvl);
	private:
		static void VerifyPattern(std::string_view pattern)
		{
			if (pattern.back() == '%')
				assert(!"Sink pattern invalid: Symbol must follow '%'!");

			for (size_t i = 0; i < pattern.size(); i++)
				if (pattern[i] == '%')
				{
					i++;
					switch (pattern[i])
					{
					case '%': break;
					//case 'H': break; // Hour
					//case 'M': break; // Minute
					case 'm': break; // Message
					case 'N': break; // Logger name
					case 'L': break; // Severity level
					//case 'S': break; // Seconds
					case 'T': break; // Time
					default: assert(!"Sink pattern invalid: Unknown symbol!");
					}
				}
		}
	protected:
		std::string _Pattern;
	private:
		Severity _LogLevel;
		bool _Multithreaded; // TODO: remove, find out how to detect mt without this
	};
}

namespace Leaf
{
	using SinkPtr = std::shared_ptr<Sinks::Sink>;
}