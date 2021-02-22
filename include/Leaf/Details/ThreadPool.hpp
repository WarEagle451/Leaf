// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Common.hpp>

#include <thread>
#include <future>
#include <functional>
#include <mutex>
#include <vector>
#include <queue>

#define LEAF_MAX_THREADS std::thread::hardware_concurrency()

namespace Leaf::Details
{
	class ThreadPool
	{
		using Job = std::function<void()>;
	private:
		ThreadPool() :
			_Shutdown(false) {}

		~ThreadPool()
		{
			{
				std::lock_guard<std::mutex> l(_Mutex);
				_Shutdown = true;
				_Condition.notify_all();
			}

			for (std::thread& thread : _Threads)
				thread.join();
		}
	public:
		static ThreadPool& Get()
		{
			static ThreadPool s_Instance;
			return s_Instance;
		}

		void AdjustPoolSize(size_t size)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			for (size_t i = 0; i < size; i++)
				_Threads.emplace_back(std::bind(&ThreadPool::Task, this));
		}

		void Queue(Job job)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			_JobQueue.emplace(std::move(job));
			_Condition.notify_one();
		}
	private:
		void Task()
		{
			Job job;

			while (true)
			{
				{
					std::unique_lock<std::mutex> l(_Mutex);

					while (!_Shutdown && _JobQueue.empty())
						_Condition.wait(l);

					if (_JobQueue.empty())
						return;

					job = std::move(_JobQueue.front());
					_JobQueue.pop();
				}

				job();
			}
		}
	private:
		std::vector<std::thread> _Threads;
		std::queue<Job> _JobQueue;
		std::mutex _Mutex;
		std::condition_variable _Condition;
		std::atomic<bool> _Shutdown;
	};
}