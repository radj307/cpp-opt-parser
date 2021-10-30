#pragma once
#include <OPT_PARSER_LIB.h>
#include <strmanip.hpp>
#ifdef SHARED_LIB

namespace opt {
	/**
	 * @brief Convert a std::stringstream to a std::vector of std::strings.
	 * @param stream		- Stringstream rvalue.
	 * @param strip_lines	- When true, str::strip_line will be called on each parsed line.
	 * @param delims		- Delimiters to use when splitting stream between vector elements
	 * @param reserve_size	- Number of vector elements to reserve by default. If this value is exceeded, the vector will reallocate space each time another element is inserted.
	 * @returns std::vector<std::string>
	 */
	inline std::vector<std::string> vectorize(std::stringstream&& stream, const bool strip_lines, const std::string& delims, const size_t reserve_size = 50u)
	{
		std::vector<std::string> vec;
		vec.reserve(reserve_size);
		for (std::string ln{}; str::getline(stream, ln, delims); vec.emplace_back(ln))
			if (strip_lines)
				ln = str::strip_line(ln);
		vec.shrink_to_fit();
		return vec;
	}
	/**
	 * @brief Convert a std::stringstream to a std::vector of std::strings.
	 * @param stream		- Stringstream rvalue.
	 * @param strip_lines	- When true, str::strip_line will be called on each parsed line.
	 * @param delims		- Delimiters to use when splitting stream between vector elements
	 * @param reserve_size	- Number of vector elements to reserve by default. If this value is exceeded, the vector will reallocate space each time another element is inserted.
	 * @returns std::vector<std::string>
	 */
	inline std::vector<std::string> vectorize(std::stringstream&& stream, const std::string& delims = "\n", const size_t reserve_size = 50u)
	{
		return vectorize(std::forward<std::stringstream>(stream), false, delims, reserve_size);
	}
}
#else
#error vertorize-stream.hpp requires the shared library!
#endif