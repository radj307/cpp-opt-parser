#pragma once
#include <vector>
#include <string>

namespace opt {
	/**
	 * @brief Convert an array of c strings to a std::vector of std::strings
	 * @param size	- Array size
	 * @param arr	- Array
	 * @returns std::vector<std::string>
	 */
	inline std::vector<std::string> vectorize(const int size, char** arr)
	{
		std::vector<std::string> vec;
		vec.reserve(size); // reserve some space
		for (auto i{ 0 }; i < size; ++i)
			vec.emplace_back(arr[i]);
		vec.shrink_to_fit(); // shrink to size
		return vec;
	}
}