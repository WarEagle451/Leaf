// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Sinks/Sink.hpp>
#include <Leaf/Sinks/TracingSink.hpp>

#include <vector>
#include <sstream>
#include <assert.h>
#include <deque>

namespace Leaf::Details
{
	template<typename T> static void RecursiveUnpack(std::vector<std::string>* unpackedStrings, std::ostringstream& outStream, const T& type)
	{
		outStream << std::setprecision(15) << type;
		unpackedStrings->emplace_back(outStream.str());
		outStream.str("");
	}

	template<typename First, typename... Args> static void RecursiveUnpack(std::vector<std::string>* unpackedStrings, std::ostringstream& outStream, First first, Args... args)
	{
		RecursiveUnpack(unpackedStrings, outStream, first);
		RecursiveUnpack(unpackedStrings, outStream, args...);
	}

	static bool IsNumber(char c)
	{
		switch (c)
		{
		case '0': return true;
		case '1': return true;
		case '2': return true;
		case '3': return true;
		case '4': return true;
		case '5': return true;
		case '6': return true;
		case '7': return true;
		case '8': return true;
		case '9': return true;
		default: return false;
		}
	}

	class Registry; // this is defined here because Logger needs to be a friend class of Registry
}

namespace Leaf
{
	class Logger
	{
		friend class Details::Registry;
	public:
		Logger(std::string&& name) :
			_Name(std::move(name)),
			_LogLevel(Severity::Trace) {}
		template<class It> Logger(std::string&& name, It begin, It end) :
			_Name(std::move(name)),
			_LogLevel(Severity::Trace),
			_Sinks(begin, end) {}
	private:
		void Submit(Severity lvl, std::string_view view)
		{
			if (lvl >= _LogLevel)
				for (SinkPtr sink : _Sinks)
					sink->Log(Details::Log(lvl, view, _Name));
		}

		template<class T> void Submit(Severity lvl, const T& type)
		{
			if (lvl >= _LogLevel)
			{
				_OSStream << std::setprecision(15) << type;
				for (SinkPtr sink : _Sinks)
					sink->Log(Details::Log(lvl, std::string_view(_OSStream.str()), _Name));
				_OSStream.str("");
			}
		}

		template<class... Args> void Submit(Severity lvl, std::string_view view, Args&&... args)
		{
			if (lvl >= _LogLevel)
			{
				Details::RecursiveUnpack(&_UnpackedStrings, _OSStream, args...);

				if (view.back() == '{')
					assert(!"Missing closing bracket!");

				for (size_t i = 0; i < view.size(); i++)
					if (view[i] == '{')
					{
						std::string num{};
						i++;
						while (Details::IsNumber(view[i]))
						{
							num += view[i];
							i++;
						}

						_OSStream << _UnpackedStrings[std::stoi(num)];

						if (view[i] != '}')
							assert(!"Mis-matched brackets!");
					}
					else _OSStream << view[i];

				_UnpackedStrings.clear();
				for (SinkPtr sink : _Sinks)
					sink->Log(Details::Log(lvl, std::string_view(_OSStream.str()), _Name));
				_OSStream.str("");
			}
		}
	public:
		void Trace(std::string_view view)											{ Submit(Severity::Trace, view); }
		template<class T> void Trace(const T& type)									{ Submit(Severity::Trace, type); }
		template<class... Args> void Trace(std::string_view format, Args&&... args)	{ Submit(Severity::Trace, format, std::forward<Args>(args)...); }

		void Debug(std::string_view view)											{ Submit(Severity::Debug, view); }
		template<class T> void Debug(const T& type)									{ Submit(Severity::Debug, type); }
		template<class... Args> void Debug(std::string_view format, Args&&... args)	{ Submit(Severity::Debug, format, std::forward<Args>(args)...); }

		void Info(std::string_view view)											{ Submit(Severity::Info, view); }
		template<class T> void Info(const T& type)									{ Submit(Severity::Info, type); }
		template<class... Args> void Info(std::string_view format, Args&&... args)	{ Submit(Severity::Info, format, std::forward<Args>(args)...); }

		void Warn(std::string_view view)											{ Submit(Severity::Warn, view); }
		template<class T> void Warn(const T& type)									{ Submit(Severity::Warn, type); }
		template<class... Args> void Warn(std::string_view format, Args&&... args)	{ Submit(Severity::Warn, format, std::forward<Args>(args)...); }

		void Error(std::string_view view)											{ Submit(Severity::Error, view); }
		template<class T> void Error(const T& type)									{ Submit(Severity::Error, type); }
		template<class... Args> void Error(std::string_view format, Args&&... args)	{ Submit(Severity::Error, format, std::forward<Args>(args)...); }

		void Fatal(std::string_view view)											{ Submit(Severity::Fatal, view); }
		template<class T> void Fatal(const T& type)									{ Submit(Severity::Fatal, type); }
		template<class... Args> void Fatal(std::string_view format, Args&&... args)	{ Submit(Severity::Fatal, format, std::forward<Args>(args)...); }

		void EnableTracingSink(std::string_view pattern, size_t size, bool multithreaded = true)
		{
			if (multithreaded)
			{
				std::shared_ptr<Sinks::TracingSinkMT> temp = std::make_shared<Sinks::TracingSinkMT>(size);
				_MessageBuffer = temp->GetBuffer();
				_Sinks.emplace_back(temp);
			}
			else
			{
				std::shared_ptr<Sinks::TracingSinkST> temp = std::make_shared<Sinks::TracingSinkST>(size);
				_MessageBuffer = temp->GetBuffer();
				_Sinks.emplace_back(temp);
			}

			_Sinks[_Sinks.size() - 1]->SetPattern(pattern);
		}

		const std::deque<Details::Token>& GetBuffer()	{ return *_MessageBuffer; }
		void ClearBuffer()								{ _MessageBuffer->clear(); }

		void SetLevel(Severity lvl) { _LogLevel = lvl; }

		const std::vector<SinkPtr>& GetSinks() { return _Sinks; }
	private:
		std::vector<SinkPtr> _Sinks;
		std::string _Name;
		Severity _LogLevel;

		std::deque<Details::Token>* _MessageBuffer = nullptr;

		// Unpack stuff // should this be universal? depends on multithreading
		std::vector<std::string> _UnpackedStrings;
		std::ostringstream _OSStream;
	};
}