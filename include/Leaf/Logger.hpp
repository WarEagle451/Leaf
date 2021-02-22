// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Details/Archive.hpp>
#include <Leaf/Sinks/Sink.hpp>
#include <Leaf/Details/Unpack.hpp>
#include <Leaf/Details/Payload.hpp>
#include <Leaf/Details/ThreadPool.hpp>

namespace Leaf
{
	namespace Details { class Registry; }
	class Logger
	{
		friend class Details::Registry;
	public:
		explicit Logger(std::string&& name) :
			_Name(std::move(name)),
			_Level(Severity::Trace),
			_Archive() {}
		template<typename It> Logger(std::string&& name, It begin, It end) :
			_Name(std::move(name)),
			_Level(Severity::Trace),
			_Sinks(begin, end),
			_Archive() {}
		Logger(std::string&& name, SinkPtr sink) :
			_Name(std::move(name)),
			_Level(Severity::Trace),
			_Sinks(1, sink),
			_Archive() {}

		template<typename T> void Log(Severity severity, const T& type)
		{
			if (severity < _Level)
				return;

			std::lock_guard<std::mutex> l(_Mutex);
			_OSS << std::setprecision(15) << type;
			Details::Payload payload{ LogMessage{ severity, _OSS.str() }, _Name };
			_OSS.str("");
			Submit(payload);
		}

		template<typename... Args> void Log(Severity severity, std::string_view format, const Args&... args)
		{
			if (severity < _Level)
				return;

			std::lock_guard<std::mutex> l(_Mutex);
			Details::Unpack(&_UnpackedStrings, _OSS, args...);
			
			for (size_t i = 0; i < format.size(); i++)
				if (format[i] == '{')
				{
					std::string num{};
					while (std::isdigit(format[++i]))
						num += format[i];
			
					if (format[i] != '}')
						; /// TODO: throw exception
					_OSS << _UnpackedStrings[std::stoi(num)];
				}
				else _OSS << format[i];
			
			Details::Payload payload{ LogMessage{ severity, _OSS.str() }, _Name };
			_UnpackedStrings.clear();
			_OSS.str("");
			Submit(payload);
		}

		template<typename T> void Trace(const T& type)									{ Log(Severity::Trace, type); }
		template<typename... Args> void Trace(std::string_view format, Args&&... args)	{ Log(Severity::Trace, format, args...); }
		template<typename T> void Debug(const T& type)									{ Log(Severity::Debug, type); }
		template<typename... Args> void Debug(std::string_view format, Args&&... args)	{ Log(Severity::Debug, format, args...); }
		template<typename T> void Info(const T& type)									{ Log(Severity::Info, type); }
		template<typename... Args> void Info(std::string_view format, Args&&... args)	{ Log(Severity::Info, format, args...); }
		template<typename T> void Warn(const T& type)									{ Log(Severity::Warn, type); }
		template<typename... Args> void Warn(std::string_view format, Args&&... args)	{ Log(Severity::Warn, format, args...); }
		template<typename T> void Error(const T& type)									{ Log(Severity::Error, type); }
		template<typename... Args> void Error(std::string_view format, Args&&... args)	{ Log(Severity::Error, format, args...); }
		template<typename T> void Fatal(const T& type)									{ Log(Severity::Fatal, type); }
		template<typename... Args> void Fatal(std::string_view format, Args&&... args)	{ Log(Severity::Fatal, format, args...); }

		void SetLevel(Severity severity)
		{
			_Level.store(severity);
		}

		void SetPattern(std::string_view pattern)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			for (SinkPtr& sink : _Sinks)
				sink->SetPattern(pattern);
		}

		std::vector<SinkPtr>& GetSinks()
		{
			std::lock_guard<std::mutex> l(_Mutex);
			return _Sinks;
		}

		void EnableArchive(std::string_view pattern, size_t capacity)	{ _Archive.Enable(pattern, capacity); }
		void DisableArchive()											{ _Archive.Disable(); }
		const std::deque<LogMessage>& GetArchiveData()					{ return _Archive.Data(); }
		void ClearArchive()												{ _Archive.Clear(); }
	private:
		void Submit(const Details::Payload& payload)
		{
			if (_Archive.Enabled())
				_Archive.Push(payload);
			if (payload.Log.Level >= _Level)
				for (SinkPtr& sink : _Sinks)
					if (sink->Loggable(payload.Log.Level))
						Details::ThreadPool::Get().Queue(std::bind(&Sinks::Sink::Log, sink, payload));
		}
	private:
		std::string _Name;
		a_Severity _Level;
		std::vector<SinkPtr> _Sinks;
		Details::Archive _Archive;

		std::ostringstream _OSS;
		std::vector<std::string> _UnpackedStrings;
		std::mutex _Mutex;
	};
}