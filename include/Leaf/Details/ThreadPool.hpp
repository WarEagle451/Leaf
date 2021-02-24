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
			_Shutdown(false),
			_TasksRunning(0) {}

		~ThreadPool()
		{
			{
				std::unique_lock<std::mutex> l(_Mutex);
				_Shutdown = true;
				cv_Task.notify_all();
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
			_JobQueue.emplace(std::forward<Job>(job));
			cv_Task.notify_one();
		}

		void Wait()
		{
			std::unique_lock<std::mutex> l(_Mutex);
			cv_Finished.wait(l, [this](){ return _JobQueue.empty() && _TasksRunning == 0; }); /// Why yeeting this destorys the logger
		}

		size_t Size()
		{
			return _Threads.size();
		}
	private:
		void Task()
		{
			Job job;

			while (true)
			{
				std::unique_lock<std::mutex> l(_Mutex);
				cv_Task.wait(l, [this](){ return _Shutdown || !_JobQueue.empty(); });
			
				if (!_JobQueue.empty())
				{
					_TasksRunning++;
					job = std::move(_JobQueue.front());
					_JobQueue.pop();
			
					l.unlock();
					job();
					l.lock();
			
					_TasksRunning--;
					cv_Finished.notify_one();
				}
				else if (_Shutdown)
					break;
			}
		}
	private:
		std::vector<std::thread> _Threads;
		std::queue<Job> _JobQueue;
		std::mutex _Mutex;
		std::condition_variable cv_Task, cv_Finished;
		std::atomic<bool> _Shutdown;
		std::atomic<uint32_t> _TasksRunning;
	};
}