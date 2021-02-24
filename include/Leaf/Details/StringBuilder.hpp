// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Details/Payload.hpp>

#include <ctime>
#include <mutex>
#include <sstream>
#include <thread>

namespace Leaf::Details
{
	class StringBuilder
	{
	public:
		static void VerifyPattern(std::string_view pattern) /// TODO: should this even be a thing?
		{
			/// TODO: Implement asserting, want a way to send it somewhere first 
			//if (pattern.back() == '%')
			//	("Invalid sink pattern! '%' cannot be at the end of a pattern!");

			for (size_t i = 0; i < pattern.size(); i++)
				if (pattern[i] == '%')
					switch (pattern[++i])
					{
					///case '': break; // Function name
					///case '': break; // File
					///case '': break; // Line in the file the message came from
					case 'N': break; // Logger name
					case 'S': break; // Severity level name
					case 's': break; // Severity level inital
					case 'T': break; // Time
					case 't': break; // Thread ID
					case '?': break; // Message
					case '%': break; // %
					}
		}

		static std::string TimeToString()
		{
			char time[10] = "";
			time_t now{ std::time(0) };
			std::strftime(time, 10, "%T", std::localtime(&now));
			return time;
		}

		void SetPattern(std::string_view pattern)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			VerifyPattern(pattern);
			_Pattern = pattern;
		}

		std::string BuildOutput(const Payload& payload) /// TODO: should probaly use fmt instead and when premake and cmake supports C++20 use
		{
			std::lock_guard<std::mutex> l(_Mutex);
			for (size_t i = 0; i < _Pattern.size(); i++)
				if (_Pattern[i] == '%')
					switch (_Pattern[++i])
					{
					case 'N': _OSS << payload.LoggerName; break;
					case 'S': _OSS << SeverityToName(payload.Log.Level); break;
					case 's': _OSS << SeverityToInitial(payload.Log.Level); break;
					case 'T': _OSS << TimeToString(); break;
					case '%': _OSS << '%'; break;
					case '?': _OSS << payload.Log.Message; break;
					}
				else _OSS << _Pattern[i];

			std::string out(_OSS.str());
			_OSS.str("");
			return out;
		}
	private:
		std::string _Pattern{};
		std::mutex _Mutex;
		std::ostringstream _OSS{};
	};
}