// Copyright(c) 2021-present, Noah LeBlanc.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>

#define LEAF_MAX_THREADS std::thread::hardware_concurrency()

namespace Leaf::Details
{
	class ThreadPool
	{
		using Job = std::function<void()>;
	public:
		static ThreadPool& Get()
		{
			static ThreadPool Instance(0);
			return Instance;
		}
	private:
		ThreadPool(size_t amount) :
			_Shutdown(false)
		{
			_Threads.reserve(amount);
			for (size_t i = 0; i < amount; i++)
				_Threads.emplace_back(std::bind(&ThreadPool::ThreadEntry, this));
		}

		~ThreadPool()
		{
			{
				std::unique_lock<std::mutex> l(_JobMutex);
				_Shutdown = true;
				_Condition.notify_all();
			}

			for (std::thread& thread : _Threads)
				thread.join();
		}
	public:
		void QueueJob(Job job)
		{
			std::unique_lock<std::mutex> l(_JobMutex);
			_JobQueue.emplace(std::move(job));
			_Condition.notify_one();
		}

		void CheckSize(size_t i)
		{
			_Threads.reserve(i);
			while (i > _Threads.size())
				_Threads.emplace_back(std::bind(&ThreadPool::ThreadEntry, this));
		}
	private:
		void ThreadEntry()
		{
			Job job;

			while (true)
			{
				{
					std::unique_lock<std::mutex> l(_JobMutex);

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
		std::mutex _JobMutex;
		std::condition_variable _Condition;
		std::atomic<bool> _Shutdown;
		std::queue<Job> _JobQueue;
		std::vector<std::thread> _Threads;
	};
}