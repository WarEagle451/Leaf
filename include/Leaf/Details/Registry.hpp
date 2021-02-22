// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Logger.hpp>

namespace Leaf::Details
{
	class Registry
	{
	private:
		Registry() = default;
	public:
		static Registry& Get()
		{
			static Registry s_Instance;
			return s_Instance;
		}

		Registry(const Registry&) = delete;
		Registry &operator=(const Registry&) = delete;

		void Register(std::shared_ptr<Logger> logger)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			for (std::shared_ptr<Logger>& ptr : _Registry)
				if (ptr->_Name == logger->_Name)
					return;///("Duplicate logger name!"); // TODO: throw exception

			ThreadPool::Get().AdjustPoolSize(logger->_Sinks.size());

			_Registry.emplace_back(std::move(logger));
		}

		void Deregister(std::string_view loggerName)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			for (auto it = _Registry.begin(); it != _Registry.end(); it++)
				if ((*it)->_Name == loggerName)
				{
					_Registry.erase(it);
					return;
				}
		}

		std::shared_ptr<Logger>& GetLogger(std::string_view loggerName)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			for (std::shared_ptr<Logger> ptr : _Registry)
				if (ptr->_Name == loggerName)
					return ptr;
		}

		void SetLevel(Severity severity)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			for (std::shared_ptr<Logger>& ptr : _Registry)
				ptr->SetLevel(severity);
		}

		void SetPattern(std::string_view pattern)
		{
			std::lock_guard<std::mutex> l(_Mutex);
			for (std::shared_ptr<Logger>& ptr : _Registry)
				ptr->SetPattern(pattern);
		}

		void Shutdown()
		{
			std::lock_guard<std::mutex> l(_Mutex);
			_Registry.clear();
		}
	private:
		std::vector<std::shared_ptr<Logger>> _Registry;
		std::mutex _Mutex;
	};
}