// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Sinks/Sink.hpp>
#include <Leaf/Details/Globals.hpp>
#include <Leaf/Details/ThreadPool.hpp>

#include <fstream>

namespace Leaf::Sinks
{
	template<class Mutex> class BasicFileSink final : public Sink
	{
	public:
		BasicFileSink(std::string_view filePath) { assert(!"Mutex has no specialization!"); }
		~BasicFileSink()
		{
			_OutStream.close();
		}

		void Log(const Details::Log& log) final override
		{
			_Mutex.lock(); // waiting for lock to be available
			_Mutex.unlock();
			Details::ThreadPool::Get().QueueJob(std::bind(&Task, log, _Pattern, std::ref<Mutex>(_Mutex), std::ref<std::ofstream>(_OutStream)));
		}
	private:
		static void Task(const Details::Log& log, std::string_view pattern, Mutex& mutex, std::ofstream& out)
		{
			std::string message(Details::Log::BuildFinalMessage(log, pattern)); // TODO: find a way to make this a string_view instead of string
			message += '\n';

			mutex.lock();
			out.write(message.data(), message.size());
			mutex.unlock();
		}
	private:
		Mutex& _Mutex;
		std::ofstream _OutStream;
	};

	template<> BasicFileSink<Details::NullMutex>::BasicFileSink(std::string_view filePath) :
		Sink(false),
		_Mutex(Details::NullFileMutex::GetInstance()) { _OutStream.open(filePath); }
	template<> BasicFileSink<std::mutex>::BasicFileSink(std::string_view filePath) :
		Sink(true),
		_Mutex(Details::FileMutex::GetInstance()) { _OutStream.open(filePath); }

	using BasicFileSinkST = BasicFileSink<Details::NullMutex>;
	using BasicFileSinkMT = BasicFileSink<std::mutex>;
}