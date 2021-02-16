// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <Leaf/Sinks/Sink.hpp>
#include <Leaf/Details/Globals.hpp>
#include <Leaf/Details/ThreadPool.hpp>

#include <deque>

namespace Leaf::Details
{
	struct Token
	{
		Token(Severity lvl, std::string_view message) : // make this not in details
			Level(lvl),
			Message(message) {}
		
		Severity Level;
		std::string Message;
	};

	class Deque // redo this
	{
	public:
		Deque() = default;
		Deque(size_t capacity) :
			_Capacity(capacity) {}

		template<class... Args> void Emplace(Args&&... args)
		{
			if (_Data.size() >= _Capacity)
				_Data.pop_front();
			_Data.emplace_back(std::forward<Args>(args)...);
		}

		const std::deque<Token>& Data() { return _Data; }
		void Flush() { _Data.clear(); }

		//friend class MessageBufferSink; // why no work?
		std::deque<Token> _Data;
	private:
		size_t _Capacity;
	};
}

namespace Leaf::Sinks
{
	using MessageBuffer = Details::Deque; // make custom thing

	template<class Mutex> class TracingSink final : public Sink
	{
	public:
		TracingSink(size_t size) : // TODO: make it so that only logger can enable this thing
			_Buffer(size) { assert(!"Mutex has no specialization!"); }

		void Log(const Details::Log& log) final override
		{
			_Mutex.lock(); // waiting for lock to be available
			_Mutex.unlock();
			Details::ThreadPool::Get().QueueJob(std::bind(&Task, log, _Pattern, std::ref<Mutex>(_Mutex), std::ref<MessageBuffer>(_Buffer)));
		}

		std::deque<Details::Token>* GetBuffer()
		{
			return &_Buffer._Data;
		}
	private:
		static void Task(const Details::Log& log, std::string_view pattern, Mutex& mutex, MessageBuffer& buffer)
		{
			std::string message(Details::Log::BuildFinalMessage(log, pattern));

			mutex.lock();
			buffer.Emplace(log.Level, message);
			mutex.unlock();
		}
	private:
		Mutex& _Mutex;
		MessageBuffer _Buffer;
	};

	template<> TracingSink<Details::NullMutex>::TracingSink(size_t size) :
		Sink(false),
		_Mutex(Details::NullBufferMutex::GetInstance()),
		_Buffer(size) {}
	template<> TracingSink<std::mutex>::TracingSink(size_t size) :
		Sink(true),
		_Mutex(Details::BufferMutex::GetInstance()),
		_Buffer(size) {}

	using TracingSinkST = TracingSink<Details::NullMutex>;
	using TracingSinkMT = TracingSink<std::mutex>;
}