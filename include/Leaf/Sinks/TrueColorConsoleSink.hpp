// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Sinks/Sink.hpp>
#include <Leaf/Details/Globals.hpp>

#include <iostream>

namespace Leaf::Sinks
{
	static const std::array<std::string, Severity::Levels> DefaultTrueColors{ "\033[38;2;230;230;230m", "\033[38;2;240;130;20m", "\033[38;2;50;150;40m", "\033[38;2;230;185;50m", "\033[38;2;200;50;50m", "\033[38;2;0;0;0;48;2;200;50;50m" };

	template<class Mutex> class TrueColorConsoleSink final : public Sink
	{
	public:
		TrueColorConsoleSink();

		void Log(const Details::Payload& payload) override
		{
			std::string output(_StrBuilder.BuildOutput(payload));

			std::lock_guard<Mutex> l(_Mutex);
			std::cout << _Colors[payload.Log.Level] << output << "\033[0m" << std::endl;
		}

		void SetColor(Severity severity, std::string_view& color)
		{
			std::lock_guard<Mutex> l(_Mutex);
			_Colors[severity] = color;
		}

		void SetColors(std::array<std::string, Severity::Levels>&& colors)
		{
			std::lock_guard<Mutex> l(_Mutex);
			_Colors = std::move(colors);
		}
	private:
		Mutex& _Mutex;
		std::array<std::string, Severity::Levels> _Colors;
	};

	template<> TrueColorConsoleSink<Details::NullMutex>::TrueColorConsoleSink() :
		_Mutex(Details::NullConsoleMutex::Get()),
		_Colors(DefaultTrueColors) {}
	template<> TrueColorConsoleSink<std::mutex>::TrueColorConsoleSink() :
		_Mutex(Details::ConsoleMutex::Get()),
		_Colors(DefaultTrueColors) {}

	using TrueColorConsoleSinkST = TrueColorConsoleSink<Details::NullMutex>;
	using TrueColorConsoleSinkMT = TrueColorConsoleSink<std::mutex>;
}