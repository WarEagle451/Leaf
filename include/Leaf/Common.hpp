// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <string>
#include <memory>
#include <atomic>
#include <exception>
#include <array>

namespace Leaf
{
	using a_Severity = std::atomic<int>;
	enum Severity
	{
		Trace = 0,
		Debug,
		Info,
		Warn,
		Error,
		Fatal,
		None,
		Levels = None
	};

	static std::array<std::string_view, Severity::Levels> SeverityLevelName{ "Trace", "Debug", "Info", "Warn", "Error", "Fatal" };
	static const std::array<char, Severity::Levels> SeverityLevelNameShort{ 'T', 'D', 'I', 'W', 'E', 'F' };

	static std::string_view& SeverityToName(Severity severity)	noexcept { return SeverityLevelName[severity]; }
	static const char SeverityToInitial(Severity severity)		noexcept { return SeverityLevelNameShort[severity]; }
}