#pragma once
#include <Leaf/Leaf.hpp>

namespace Example
{
	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<Leaf::Logger>& GetCoreLogger()	{ return s_CoreLogger; }
	private:
		static Ref<Leaf::Logger> s_CoreLogger;
	};
}

// Core log macros

#define CORE_TRACE(...)	::Example::Log::GetCoreLogger()->Trace(__VA_ARGS__)
#define CORE_DEBUG(...)	::Example::Log::GetCoreLogger()->Debug(__VA_ARGS__)
#define CORE_INFO(...)	::Example::Log::GetCoreLogger()->Info(__VA_ARGS__)
#define CORE_WARN(...)	::Example::Log::GetCoreLogger()->Warn(__VA_ARGS__)
#define CORE_ERROR(...)	::Example::Log::GetCoreLogger()->Error(__VA_ARGS__)
#define CORE_FATAL(...)	::Example::Log::GetCoreLogger()->Fatal(__VA_ARGS__)