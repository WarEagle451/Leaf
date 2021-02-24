// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <Leaf/Sinks/Sink.hpp>
#include <Leaf/Details/StringBuilder.hpp>
#include <Leaf/Details/NullMutex.hpp>

#include <fstream>

namespace Leaf::Sinks
{
	template<class Mutex> class BasicFileSink final : public Sink
	{
	public:
		BasicFileSink(std::string_view filePath);
		~BasicFileSink() { _Out.close(); }

		void Log(const Details::Payload& payload) override
		{
			if (payload.Log.Level < _Level)
				return;

			std::string output(_StrBuilder.BuildOutput(payload));
			output += '\n';

			std::lock_guard<Mutex> l(_Mutex);
			_Out.write(output.data(), output.size());
		}

		void SetPattern(std::string_view& pattern) override
		{
			_StrBuilder.SetPattern(pattern);
		}

		void OpenNewFilePath(std::string_view& filePath)
		{
			std::lock_guard<Mutex> l(_Mutex);
			_Out.close();
			_Out.open(filePath.data());
		}
	private:
		Mutex _Mutex;
		Details::StringBuilder _StrBuilder;
		std::ofstream _Out;
	};

	template<> BasicFileSink<Details::NullMutex>::BasicFileSink(std::string_view filePath) :
		_StrBuilder() { _Out.open(filePath.data()); }
	template<> BasicFileSink<std::mutex>::BasicFileSink(std::string_view filePath) :
		_StrBuilder() { _Out.open(filePath.data()); }

	using BasicFileSinkST = BasicFileSink<Details::NullMutex>;
	using BasicFileSinkMT = BasicFileSink<std::mutex>;
}