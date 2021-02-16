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
	template<class Mutex> class BasicConsoleSink final : public Sink
	{
	public:
		BasicConsoleSink() { assert(!"Mutex has no specialization!"); }

		void Log(const Details::Log& log) final override
		{
			_Mutex.lock(); // waiting for lock to be available
			_Mutex.unlock();
			Details::ThreadPool::Get().QueueJob(std::bind(&Task, log, _Pattern, std::ref<Mutex>(_Mutex)));
		}
	private:
		static void Task(const Details::Log& log, std::string_view pattern, Mutex& mutex)
		{
			std::string message(Details::Log::BuildFinalMessage(log, pattern));

			mutex.lock();
			std::cout << message << std::endl;
			mutex.unlock();
		}
	private:
		Mutex& _Mutex;
	};

	template<> BasicConsoleSink<Details::NullMutex>::BasicConsoleSink() :
		Sink(false),
		_Mutex(Details::NullConsoleMutex::GetInstance()) {}
	template<> BasicConsoleSink<std::mutex>::BasicConsoleSink() :
		Sink(true),
		_Mutex(Details::ConsoleMutex::GetInstance()) {}

	using BasicConsoleSinkST = BasicConsoleSink<Details::NullBufferMutex>;
	using BasicConsoleSinkMT = BasicConsoleSink<std::mutex>;
}