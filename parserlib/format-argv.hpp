/**
 * @file format-argv.hpp
 * @author radj307
 * @brief	Contains the vectorize function, which simply turns a C-String array into a C++ std::string vector.
 */
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
	inline std::vector<std::string> vectorize(const int size, char** arr, const int off = 1)
	{
		std::vector<std::string> vec;
		vec.reserve(size); // reserve some space
		for (auto i{ off }; i < size; ++i)
			vec.emplace_back(arr[i]);
		vec.shrink_to_fit(); // shrink to size
		return vec;
	}
}