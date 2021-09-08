#pragma once
#include <vector>
#include <string>

namespace opt {
	inline std::vector<std::string> vectorize(const int size, char** arr)
	{
		std::vector<std::string> vec;
		vec.reserve(size);
		for (auto i{ 0 }; i < size; ++i)
			vec.emplace_back(arr[i]);
		vec.shrink_to_fit();
		return vec;
	}
}