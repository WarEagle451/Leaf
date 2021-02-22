// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Common.hpp>
#include <Leaf/Details/StringBuilder.hpp>

#include <deque>
#include <functional>
#include <mutex>

namespace Leaf::Details
{
	class Archive
	{
	public:
		Archive() :
			_Enabled(false),
			_StrBuilder(),
			_Capacity(0) {}

		void Enable(std::string_view pattern, size_t capacity)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			_Enabled = true;
			_StrBuilder.SetPattern(std::move(pattern));
			_Capacity = capacity;
		}

		void Disable()
		{
			std::lock_guard<std::mutex> l(_Mutex);
			_Enabled = false;
		}

		const bool Enabled()
		{
			return _Enabled;
		}

		void SetPattern(std::string_view pattern)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			_StrBuilder.SetPattern(pattern);
		}

		void SetCapacity(size_t capacity)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			_Capacity = capacity;
		}

		void Push(const Payload& payload)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			if (_Data.size() >= _Capacity)
				_Data.pop_front();
			_Data.push_back(LogMessage{ payload.Log.Level, _StrBuilder.BuildOutput(payload) });
		}

		void Clear()
		{
			std::lock_guard<std::mutex> l(_Mutex);
			_Data.clear();
		}

		const std::deque<LogMessage>& Data()
		{
			std::lock_guard<std::mutex> l(_Mutex);
			return _Data;
		}
	private:
		bool _Enabled;
		std::deque<LogMessage> _Data;
		StringBuilder _StrBuilder;
		std::mutex _Mutex;
		size_t _Capacity;
	};
}