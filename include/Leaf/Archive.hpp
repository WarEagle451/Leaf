// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Details/NullMutex.hpp>
#include <Leaf/Details/StringBuilder.hpp>

#include <deque>

namespace Leaf
{
	class ArchiveBase
	{
	public:
		virtual void Store(const Details::Payload& payload) = 0;
		virtual void SetCapacity(size_t capacity) = 0;
		virtual void Clear() = 0;

		const std::deque<LogMessage>& GetBuffer() const { return _Buffer; }

		void SetPattern(std::string_view pattern) { _StrBuilder.SetPattern(pattern); }
	public:
		std::atomic<bool> Enabled{ true };
	protected:
		std::deque<LogMessage> _Buffer;
		Details::StringBuilder _StrBuilder{};
	};

	template<class Mutex> class Archive final : public ArchiveBase
	{
	public:
		Archive(std::string_view pattern, size_t capacity) {}

		void Store(const Details::Payload& payload) override
		{
			std::string output(_StrBuilder.BuildOutput(payload));
			std::lock_guard<Mutex> l(_Mutex);
			if (_Buffer.size() >= _Capacity)
				_Buffer.pop_front();
			_Buffer.push_back(LogMessage{ payload.Log.Level, _StrBuilder.BuildOutput(payload) });
		}

		void SetCapacity(size_t capacity) override
		{
			std::lock_guard<Mutex> l(_Mutex);
			_Capacity = capacity;
		}

		void Clear() override
		{
			std::lock_guard<Mutex> l(_Mutex);
			_Buffer.clear();
		}
	private:
		Mutex _Mutex;
		size_t _Capacity;
	};

	template<> Archive<Details::NullMutex>::Archive(std::string_view pattern, size_t capacity) :
		_Capacity(capacity) { _StrBuilder.SetPattern(pattern); }
	template<> Archive<std::mutex>::Archive(std::string_view pattern, size_t capacity) :
		_Capacity(capacity) { _StrBuilder.SetPattern(pattern); }

	using ArchiveST = Archive<Details::NullMutex>;
	using ArchiveMT = Archive<std::mutex>;
}