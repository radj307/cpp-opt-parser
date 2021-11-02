#pragma once
#include <ParamsAPI.hpp>
#include <str.hpp>

namespace opt {
	/**
	 * @struct Argument
	 * @brief Represents one argument, which can have an opt, flag, or both.
	 */
	struct Argument {
		const std::optional<std::string> opt;	///< @brief Option name
		const std::optional<char> flag;			///< @brief Flag name
		const std::optional<std::string> doc;	///< @brief Documentation

		constexpr Argument(char flagName, std::string optName) : opt{ std::move(optName) }, flag{ std::move(flagName) } {}
		constexpr Argument(std::string optName) : opt{ std::move(optName) }, flag{ std::nullopt } {}
		constexpr Argument(char flagName) : opt{ std::nullopt }, flag{ std::move(flagName) } {}
	};

	/// @brief Macro for an ArgumentDocumentationPair
	using ARG = Argument;

	/**
	 * @struct ArgDocPrinter
	 * @brief Prints an argument pair with the correct indentation.
	 */
	struct ArgDocPrinter {
		const std::streamsize margin_width;
		const Argument argpr;

		ArgDocPrinter(std::streamsize margin_width, Argument argpr) : margin_width{ std::move(margin_width) }, argpr{ std::move(argpr) } {}

		explicit constexpr operator const std::string() const
		{
			std::string args;
			if (argpr.flag.has_value())
				args += "-" + argpr.flag.value();
			if (argpr.opt.has_value())
				args += "  --" + argpr.opt.value();
			return str::stringify(args, str::VIndent(margin_width, args.size()), argpr.doc);
		}

		friend std::ostream& operator<<(std::ostream& os, const ArgDocPrinter& printer)
		{
			os << printer.operator const std::string();
			return os;
		}
	};

	/**
	 * @brief Check if a given argument was included on the commandline. This function directly accepts ARG variables to assist .
	 * @param inst		- A ParamsAPI reference.
	 * @param argument	- An ARG instance.
	 * @returns bool
	 */
	inline bool check_arg(const opt::ParamsAPI& inst, const ARG& argument)
	{
		return (argument.flag.has_value() ? inst.check_flag(argument.flag.value()) : false)
			|| (argument.opt.has_value() ? inst.check_opt(argument.opt.value()) : false);
	}

	/** EXAMPLE IMPLEMENTATION
	 * inline const ARG
	 *		HELP{ 'h', "help", "Shows the help display." },
	 *		DRY_RUN{ 'd', "dry", "Prevents any actual modifications to the filesystem, instead just simulating them." };
	 *
	 * inline void print_help()
	 * {
	 *	 const std::streamsize margin_width{ 20ll };
	 *	 for (auto& it : std::vector<ARG>{
	 *		HELP,
	 *		DRY_RUN
	 *	 }) {
	 *		std::cout << ArgDocPrinter(margin_width, it) << '\n';
	 *	}
	 * }
	 *
	 * int main(const int argc, char** argv)
	 * {
	 *	 opt::ParamsAPI args{ argc, argv };
	 *
	 * }
	 */
}