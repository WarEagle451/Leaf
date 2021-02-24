// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Common.hpp>
#include <Leaf/Details/Payload.hpp>

#include <ctime>
#include <mutex>

namespace Leaf::Details
{
	class StringBuilder
	{
	public:
		static void VerifyPattern(std::string_view pattern)
		{
			/// TODO: Implement asserting, want a way to send it somewhere first 
			//if (pattern.back() == '%')
			//	("Invalid sink pattern! '%' cannot be at the end of a pattern!");

			for (size_t i = 0; i < pattern.size(); i++)
				if (pattern[i] == '%')
					switch (pattern[++i])
					{
					case '%': break; // %
					case 'M': break; // Message
					case 'N': break; // Logger name
					case 'S': break; // Severity level name
					case 's': break; // Severity level inital
					case 'T': break; // Time
					///default: LEAF_THROW("Invalid Sink Pattern! '%' must be followed by a vaild character!");
					}
		}

		static std::string TimeToString()
		{
			time_t now{ std::time(0) };
			char time[10] = "";
			std::strftime(time, 10, "%T", std::localtime(&now));
			return time;
		}

		void SetPattern(std::string_view pattern)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			VerifyPattern(pattern);
			_Pattern = pattern;
		}

		std::string BuildOutput(const Payload& payload)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			std::string out{};
			for (size_t i = 0; i < _Pattern.size(); i++)
				if (_Pattern[i] == '%')
					switch (_Pattern[++i])
					{
					case '%': out += '%'; break;
					case 'M': out += payload.Log.Message; break;
					case 'N': out += payload.LoggerName; break;
					case 'S': out += SeverityToName(payload.Log.Level); break;
					case 's': out += SeverityToInitial(payload.Log.Level); break;
					case 'T': out += TimeToString(); break;
					}
				else out += _Pattern[i];
			return out;
		}
	private:
		std::string _Pattern{};
		std::mutex _Mutex;
	};
}