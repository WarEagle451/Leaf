// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Severity.hpp>

#include <string>
#include <assert.h>
#include <ctime>

namespace Leaf::Details
{
	class Log
	{
	public:
		Log(Severity lvl, std::string_view view, std::string_view loggerName) :
			Level(lvl),
			Message(view),
			LoggerName(loggerName) {}

		static std::string BuildFinalMessage(const Log& log, std::string_view pattern)
		{
			std::string out = "";
			for (size_t i = 0; i < pattern.size(); i++)
				if (pattern[i] == '%')
				{
					i++;
					switch (pattern[i])
					{
					case '%': out += '%'; break;
					//case 'H': break; // Hour
					//case 'M': break; // Minute
					case 'm': out += log.Message; break; // Message
					case 'N': out += log.LoggerName; break; // Logger name
					case 'L': out += SeverityString(log.Level); break; // Severity level
					//case 'S': break; // Seconds
					case 'T': out += TimeString(); break; // Time
					default: assert(!"Sink pattern invalid: Unknown symbol!");
					}
				}
				else out += pattern[i];
			return out;
		}
	private:
		static std::string SeverityString(Severity lvl) // should these return views?
		{
			switch (lvl)
			{
			case Severity::Trace:	return "Trace";
			case Severity::Debug:	return "Debug";
			case Severity::Info:	return "Info";
			case Severity::Warn:	return "Warn";
			case Severity::Error:	return "Error";
			case Severity::Fatal:	return "Fatal";
			case Severity::None:	assert(!"Severity level set to None!"); break;
			default:				assert(!"No severity level set in message!"); break;
			}

			return "!Severity Error!";
		}

		static std::string TimeString() // should these return views?
		{
			time_t now{ std::time(0) };
			char time[10] = "";
			std::strftime(time, 10, "%T", std::localtime(&now));
			return time;
		}
	public:
		Severity Level;
		std::string Message;
		std::string_view LoggerName;
	};
}