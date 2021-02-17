#pragma once
#include <vector>
#include <sstream>
#include <string>

namespace Leaf::Details
{
	template<typename T> static void RecursiveUnpack(std::vector<std::string>* unpackedStrings, std::ostringstream& outStream, const T& type)
	{
		outStream << std::setprecision(15) << type;
		unpackedStrings->emplace_back(outStream.str());
		outStream.str("");
	}

	template<typename First, typename... Args> static void RecursiveUnpack(std::vector<std::string>* unpackedStrings, std::ostringstream& outStream, First first, Args... args)
	{
		RecursiveUnpack(unpackedStrings, outStream, first);
		RecursiveUnpack(unpackedStrings, outStream, args...);
	}
}