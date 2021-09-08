#pragma once
#include "format-argv.hpp"
#include "parser.hpp"

namespace opt {
	using ContainerType = std::vector<VariantArgument>;

	class Params {
		ContainerType _args;

	public:
		/**
		 * @brief Constructor that takes argc & argv directly from main, and a parse_config instance.
		 * @param argc			- Argument array size.
		 * @param argv			- Argument array.
		 * @param parse_config	- Parsing configuration values.
		 */
		explicit Params(const int argc, char** argv, const ParseConfig& parse_config) : _args{ parseArgs(vectorize(argc, argv), parse_config) } {}
		explicit Params(const int argc, char** argv, const std::vector<std::string>& capture_list) : _args{ parseArgs(vectorize(argc, argv), capture_list) } {}
		explicit Params(ContainerType cont) : _args{ std::move(cont) } {}

		auto begin() const { return _args.begin(); }
		auto rbegin() const { return _args.rbegin(); }
		auto end() const { return _args.end(); }
		auto rend() const { return _args.rend(); }
		auto at(const ContainerType::size_type& pos) const { return _args.at(pos); }

		/**
		 * @brief Retrieve an iterator to the option or parameter with a given name.
		 * @param arg				- Argument name (or capture name if check_captures) to search for.
		 * @param check_captures	- When true, also checks capture values for options, and flags.
		 * @returns ContainerType::const_iterator
		 */
		ContainerType::const_iterator find(const std::string& arg, const bool check_captures = false) const
		{
			bool check_flags{ false };
			if (arg.size() == 1u)
				check_flags = true;
			for (auto it{ _args.begin() }; it != _args.end(); ++it) {
				switch (it->type()) {
				case Type::PARAMETER:
					if (it->get<Parameter>() == arg)
						return it;
					break;
				case Type::OPTION:
					if (const auto val{ it->get<Option>() }; val.first == arg || ( check_captures && val.second.has_value() && val.second.value() == arg ))
						return it;
					break;
				case Type::FLAG:
					if (check_captures || check_flags) // only check flags if check_captures is true, as input type string cannot be a flag.
						if (const auto val{ it->get<Flag>() }; val.second.has_value() && val.second.value() == arg || (check_flags && val.first == arg.at(0u)))
							return it;
					break;
				default:
					break;
				}
			}
			return _args.end();
		}
		/**
		 * @brief Retrieve an iterator to the flag with a given character name.
		 * @param arg				- Flag name to search for.
		 * @returns ContainerType::const_iterator
		 */
		ContainerType::const_iterator find(const char& arg) const
		{
			for (auto it{ _args.begin() }; it != _args.end(); ++it) {
				switch (it->type()) {
				case Type::FLAG: {
					const auto [flag, cap] { it->get<Flag>() };
					if (flag == arg)
						return it;
					break;
				}
				default:
					break;
				}
			}
			return _args.end();
		}

		/**
		 * @brief Check if a given argument appears in the argument list.
		 * @param arg	- String to search for.
		 * @returns bool
		 */
		bool contains(const std::string& arg, const bool check_captures = false) const
		{
			return find(arg) != _args.end();
		}
		bool contains(const char& arg, const bool check_captures = false) const
		{
			return find(arg) != _args.end();
		}

		std::optional<std::string> getv(const std::string& opt) const
		{
			const auto it{ find(opt) };
			return it == _args.end() ? std::nullopt : it->getv<Option>();
		}

		std::optional<std::string> getv(const char flag) const
		{
			const auto it{ find(flag) };
			return it == _args.end() ? std::nullopt : it->getv<Option>();
		}

		bool check(auto&& arg) const
		{
			return contains(std::forward<decltype(arg)>(arg));
		}

		bool check_opt(const std::string& opt) const
		{
			const auto it{ find(opt) };
			return it != _args.end() && *it == Type::OPTION;
		}

		bool check_flag(const char flag) const
		{
			const auto it{ find(flag) };
			return it != _args.end() && *it == Type::FLAG;
		}

		bool check_param(const std::string& param) const
		{
			const auto it{ find(param) };
			return it != _args.end() && *it == Type::PARAMETER;
		}

		friend std::ostream& operator<<(std::ostream& os, const Params& obj)
		{
			for (auto& it : obj._args)
				os << it;
			return os;
		}

		operator ContainerType() { return _args; }
	};
}