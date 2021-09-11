/**
 * @file parseArgs.hpp
 * @author radj307
 * @brief	Contains the parseArgs function, used for parsing commandline arguments.
 */
#pragma once
#include <sstream>
#include <strmanip.hpp>
#include "VariantArgument.hpp"
#include "ParserConfig.hpp"

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
	inline ContainerType parseArgs(const std::vector<std::string>& args, const ParserConfig& cfg)
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