#pragma once
#include <sstream>
#include <strmanip.hpp>
#include "VariantArgument.hpp"

namespace opt {
	using ContainerType = std::vector<VariantArgument>;

	struct ParseConfig {
		std::vector<std::string> _capture_list;
		std::string _type_delims;
		const bool _include_delims;

		ParseConfig(std::vector<std::string> capture_list, std::string delims = "-/", const bool include_delims = false) : _capture_list{ std::move(capture_list) }, _type_delims{ std::move(delims) }, _include_delims{ include_delims } {}
		ParseConfig() : ParseConfig({}, "-/", false) {}

		inline bool isDelim(const char c) const
		{
			return str::pos_valid(_type_delims.find(c));
		}

		inline size_t countPrefix(const std::string& str, const size_t off = 0u, const size_t max = 2u) const
		{
			size_t count{ 0u };
			for (auto i{ 0u }; i < str.size() && i < max; ++i)
				count += isDelim(str.at(i));
			return count;
		}

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
		 * @brief Check if a given char is present in a given list of strings.
		 * @param c				- Char to search for.
		 * @param capture_list	- Vector to search.
		 * @returns bool
		 *\n		true	- Char is present.
		 *\n		false	- Char is not present.
		 */
		inline bool allowCapture(const char c) const
		{
			return allowCapture(std::string(1u, c));
		}

	};

	/**
	 * @brief Parse a list of strings into a variant container type.
	 * @param args			 - argv as a vector
	 * @param capture_list	 - List of argument names that can capture arguments.
	 * @param delims		 - String containing all valid opt prefixes (default: '-' or '/')
	 * @param include_delims - When true, includes prefix delimiters in the argument name
	 * @returns ContainerType
	 */
	inline ContainerType parseArgs(const std::vector<std::string>& args, const ParseConfig& cfg)
	{
		ContainerType cont;
		cont.reserve(args.size());

		for (auto it{ args.begin() }; it != args.end(); ++it) {
			const auto dashCount{ cfg.countPrefix(*it) };

			switch (dashCount) {
			case 2u: { // Option
				if (it + 1u != args.end()
					&& cfg.allowCapture(*it)
					&& !cfg.isDelim((it + 1u)->at(0u))
				) {
					std::string here{ cfg._include_delims ? *it : it->substr(dashCount) };
					cont.emplace_back(std::make_pair(here, *++it));
				}
				else
					cont.emplace_back(std::make_pair(cfg._include_delims ? *it : it->substr(dashCount), std::nullopt)); // opt without capture
				break;
			}
			case 1u: { // Flag
				std::string arg{ *it };
				for (auto ch{ arg.begin() + dashCount }; ch != arg.end(); ++ch) {
					if (it + 1u < args.end()
						&& cfg.allowCapture(*ch)
						&& !cfg.isDelim((it + 1u)->at(0u))
					)
						cont.emplace_back(std::make_pair(*ch, *++it));
					else
						cont.emplace_back(std::make_pair(*ch, std::nullopt)); // flag without capture
				}
				break;
			}
			case 0u: { // Parameter
				cont.emplace_back(*it);
				break;
			}
			default: // shouldn't be possible
				break;
			}
		}

		cont.shrink_to_fit();
		return cont;
	}
}