/**
 * @file parseArgs.hpp
 * @author radj307
 * @brief	Contains the parseArgs function, used for parsing commandline arguments.
 */
#pragma once
#include <sstream>
#include <VariantArgument.hpp>
#include <ParserConfig.hpp>

namespace opt {
	using ContainerType = std::vector<VariantArgument>;

	/**
	 * @brief Parse a list of strings into a variant container type.
	 * @param args			 - argv as a vector
	 * @param capture_list	 - List of argument names that can capture arguments.
	 * @param delims		 - String containing all valid opt prefixes (default: '-' or '/')
	 * @param include_delims - When true, includes prefix delimiters in the argument name
	 * @returns ContainerType
	 */
	inline ContainerType parseArgs(const std::vector<std::string>& args, const ParserConfig& cfg = {})
	{
		ContainerType cont;
		cont.reserve(args.size()); // reserve enough space for all arguments should no captures occur.

		for (auto it{ args.begin() }; it != args.end(); ++it) {
			const auto dashCount{ cfg.countPrefix(*it) };
			switch (dashCount) {
			case 2u: { // Option
				if (it + 1u != args.end()
					&& cfg.allowCapture(*it)
					&& !cfg.isDelim((it + 1u)->at(0u))
				) { // capture next argument
					std::string here{ it->substr(dashCount) };
					cont.emplace_back(std::make_pair(here, *++it)); // opt with capture
				}
				else
					cont.emplace_back(std::make_pair(it->substr(dashCount), std::nullopt)); // opt without capture
				break;
			}
			case 1u: { // Flag
				std::string arg{ *it };
				// if not a negative number & not a negative hexadecimal number, parse as a flag
				if (const bool hex_prefix{ arg.substr(dashCount, 2ull) == "0x" }; !hex_prefix && !std::all_of(arg.begin() + dashCount + (hex_prefix ? 2ull : 0ull), arg.end(), [](auto&& ch) { return isdigit(ch) || ch == '.'; })) {
					for (auto ch{ arg.begin() + dashCount }; ch != arg.end(); ++ch) {
						if (it + 1u < args.end()
							&& cfg.allowCapture(*ch)
							&& !cfg.isDelim((it + 1u)->at(0u))
							)
							cont.emplace_back(std::make_pair(*ch, *++it)); // flag with capture
						else
							cont.emplace_back(std::make_pair(*ch, std::nullopt)); // flag without capture
					}
					break;
				}
				[[fallthrough]]; // if arg was a negative number or negative hexadecimal number
			}
			case 0u: { // Parameter
				cont.emplace_back(*it); // parameter
				break;
			}
			default: // shouldn't be possible
				break;
			}
		}

		cont.shrink_to_fit(); // reduce capacity to fit, as some arguments may have been captured.
		return cont;
	}
}