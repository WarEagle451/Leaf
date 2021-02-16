// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Sinks/Sink.hpp>
#include <Leaf/Details/Globals.hpp>
#include <Leaf/Details/ThreadPool.hpp>

#include <iostream>
#include <assert.h>

namespace Leaf::Sinks
{
	template<class Mutex> class ANSIConsoleSink final : public Sink
	{
	public:
		ANSIConsoleSink() { assert(!"Mutex has no specialization!"); }

		void Log(const Details::Log& log) final override
		{
			_Mutex.lock(); // waiting for lock to be available
			_Mutex.unlock();
			Details::ThreadPool::Get().QueueJob(std::bind(&Task, log, _Pattern, std::ref<Mutex>(_Mutex)));
		}
	private:
		static void Task(const Details::Log& log, std::string_view pattern, Mutex& mutex)
		{
			std::string message{};

			switch (log.Level)
			{
			case Severity::Trace:	message += "\033[37m"; break;
			case Severity::Debug:	message += "\033[94m"; break;
			case Severity::Info:	message += "\033[32m"; break;
			case Severity::Warn:	message += "\033[93m"; break;
			case Severity::Error:	message += "\033[31m"; break;
			case Severity::Fatal:	message += "\033[30;41m"; break;
			case Severity::None: break;
			default: assert(!"TrueColorConsoleSink error! Unknown severity level detected!");
			}

			message += Details::Log::BuildFinalMessage(log, pattern) + "\033[0m";

			mutex.lock();
			std::cout << message << std::endl;
			mutex.unlock();
		}
	private:
		Mutex& _Mutex;
	};

	template<> ANSIConsoleSink<Details::NullMutex>::ANSIConsoleSink() :
		Sink(false),
		_Mutex(Details::NullConsoleMutex::GetInstance()) {}
	template<> ANSIConsoleSink<std::mutex>::ANSIConsoleSink() :
		Sink(true),
		_Mutex(Details::ConsoleMutex::GetInstance()) {}

	using ANSIConsoleSinkST = ANSIConsoleSink<Details::NullBufferMutex>;
	using ANSIConsoleSinkMT = ANSIConsoleSink<std::mutex>;
}