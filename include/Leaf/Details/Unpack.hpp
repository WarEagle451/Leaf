// Copyright(c) 2021-present, Noah LeBlanc.

#pragma once
#include <string>
#include <vector>
#include <sstream>

namespace Leaf::Details
{
	template<typename T> static void Unpack(std::vector<std::string>* unpacked, std::ostringstream& oss, const T& type)
	{
		oss << std::setprecision(15) << type;
		unpacked->emplace_back(oss.str());
		oss.str("");
	}

	template<typename First, typename... Args> static void Unpack(std::vector<std::string>* unpacked, std::ostringstream& oss, First first, Args... args)
	{
		Unpack(unpacked, oss, first);
		Unpack(unpacked, oss, args...);
	}
}