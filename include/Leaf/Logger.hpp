// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Sinks/Sink.hpp>
#include <Leaf/Sinks/TracingSink.hpp>
#include <Leaf/Details/RecursiveUnpack.hpp>

#include <assert.h>
#include <deque>

namespace Leaf
{
	class Details::Registry;
	class Logger
	{
		friend class Details::Registry;
	public:
		Logger(std::string_view name) :
			_Name(name),
			_LogLevel(Severity::Trace) {}
		template<class It> Logger(std::string_view name, It begin, It end) :
			_Name(name),
			_LogLevel(Severity::Trace),
			_Sinks(begin, end) {}
	private:
		void Submit(Severity lvl, std::string_view view)
		{
			if (lvl < _LogLevel)
				return;

			for (SinkPtr sink : _Sinks)
				sink->Log(Details::Log(lvl, view, _Name));
		}

		template<class T> void Submit(Severity lvl, const T& type)
		{
			if (lvl < _LogLevel)
				return;

			_OSS << std::setprecision(15) << type;
			for (SinkPtr sink : _Sinks)
				sink->Log(Details::Log(lvl, std::string_view(_OSS.str()), _Name));
			_OSS.str("");
		}

		template<class... Args> void Submit(Severity lvl, std::string_view view, Args&&... args)
		{
			if (lvl < _LogLevel)
				return;

			Details::RecursiveUnpack(&_UnpackedStrings, _OSS, args...);

			if (view.back() == '{')
				assert(!"Missing closing bracket!");

			for (size_t i = 0; i < view.size(); i++)
				if (view[i] == '{')
				{
					std::string num{};
					i++;
					while (std::isdigit(view[i]))
					{
						num += view[i];
						i++;
					}

					_OSS << _UnpackedStrings[std::stoi(num)];

					if (view[i] != '}')
						assert(!"Mis-matched brackets!");
				}
				else _OSS << view[i];

			_UnpackedStrings.clear();
			for (SinkPtr sink : _Sinks)
				sink->Log(Details::Log(lvl, std::string_view(_OSS.str()), _Name));
			_OSS.str("");
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
	protected:
		std::vector<SinkPtr> _Sinks;
		std::string _Name;
		Severity _LogLevel;

		std::deque<Details::Token>* _MessageBuffer = nullptr;

		// Unpack stuff // should this be universal? depends on multithreading
		std::vector<std::string> _UnpackedStrings;
		std::ostringstream _OSS;
	};
}