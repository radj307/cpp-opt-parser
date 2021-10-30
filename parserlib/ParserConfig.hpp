/**
 * @file ParserConfig.hpp
 * @author radj307
 * @brief	Contains the ParserConfig struct, which exposes configuration options for the parseArgs function.
 */
#pragma once
#include <vector>
#include <string>
#include <strmanip.hpp>

namespace opt {
	inline static const std::string _DEFAULT_OPT_DELIMITERS{ "-" };
	/**
	 * @struct ParserConfig
	 * @brief Contains variables and methods required by the parseArgs function.
	 */
	struct ParserConfig {
		std::vector<std::string> _capture_list; ///< @brief Any strings present on this list will be able to capture arguments the occur directly after them, so long as they are not arguments as well. Both flags (as single character strings) and opts can be specified here.
		std::string _type_delims;	///< @brief By default, '-' is accepted as a prefix argument. 1 dash is a short-opt, or flag, and each character is parsed individually. 2 dashes is a long-opt and is treated as a single argument.
		bool _allow_negative_numbers{ true }; ///< @brief When true, if an argument prefixed with '-' is entirely digits and/or '.' characters, it is treated as a Parameter, not a flag.

		ParserConfig() : ParserConfig({}, _DEFAULT_OPT_DELIMITERS) {}
		ParserConfig(std::vector<std::string> capture_list, std::string delims = _DEFAULT_OPT_DELIMITERS) : _capture_list{std::move(capture_list)}, _type_delims{std::move(delims)} {}

		/**
		 * @brief Check if a given char is a valid delimiter.
		 * @param c	- Input Char.
		 * @returns bool
		 */
		inline bool isDelim(const char c) const
		{
			return str::pos_valid(_type_delims.find(c));
		}

		/**
		 * @brief Count the number of preceeding delimiters in a string.
		 * @param str	- Input string.
		 * @param off	- Pos to start at.
		 * @param max	- Max counter value before returning, even if there are more delimiters.
		 * @returns size_t
		 */
		inline size_t countPrefix(const std::string& str, const size_t off = 0u, const size_t max = 2u) const
		{
			size_t count{ 0u };
			for (auto i{ 0u }; i < str.size() && i < max; ++i)
				count += isDelim(str.at(i));
			return count;
		}

		/**
		 * @brief Check if a given string is present on the capture list, indicating that it should capture the following argument.
		 * @param str		- string to search for.
		 * @returns bool
		 *\n		true	- Char is present.
		 *\n		false	- Char is not present.
		 */
		inline bool allowCapture(std::string str) const
		{
			if (str.empty() || _capture_list.empty())
				return false;
			str = str.substr(countPrefix(str));
			for (auto& it : _capture_list)
				if (it == str)
					return true;
			return false;
		}
		/**
		 * @brief Check if a given char is present on the capture list, indicating that it should capture the following argument.
		 * @param c				- Char to search for.
		 * @returns bool
		 *\n		true	- Char is present.
		 *\n		false	- Char is not present.
		 */
		inline bool allowCapture(const char c) const
		{
			return allowCapture(std::string(1u, c));
		}
	};

}