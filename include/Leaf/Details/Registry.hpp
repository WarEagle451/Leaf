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
			for (std::shared_ptr<Logger>& ptr : _Registry)
				if (ptr->_Name == logger->_Name)
					return;///("Duplicate logger name!"); // TODO: throw exception
			_Registry.emplace_back(logger);
		}

		void Deregister(std::string_view loggerName)
		{
			for (auto it = _Registry.begin(); it != _Registry.end(); it++)
				if ((*it)->_Name == loggerName)
				{
					_Registry.erase(it);
					return;
				}
		}

		std::shared_ptr<Logger>& GetLogger(std::string_view loggerName)
		{
			for (std::shared_ptr<Logger> ptr : _Registry)
				if (ptr->_Name == loggerName)
					return ptr;
		}

		void SetLevel(Severity severity)
		{
			for (std::shared_ptr<Logger>& ptr : _Registry)
				ptr->SetLevel(severity);
		}

		void SetPattern(std::string_view pattern)
		{
			for (std::shared_ptr<Logger>& ptr : _Registry)
				ptr->SetPattern(pattern);
		}

		void Shutdown()
		{
			_Registry.clear();
		}
	private:
		std::vector<std::shared_ptr<Logger>> _Registry;
	};
}