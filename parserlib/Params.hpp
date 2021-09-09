/**
 * @file Params.hpp
 * @author radj307
 * @brief This is the main file to include if you want to use this lib.
 *\n_USAGE:_
 *\n	Simply define an instance of opt::Params() in main and pass the argc & argv parameters to it.
 *\n	It will then convert the arguments to a vector, and parse them using parseArgs function.
 *\n	You can also include an optional "capture list" (vector<string>).
 *\n	Include the name of options or flags here (excluding prefix delimiters) to indicate that they should "capture" the following parameter when possible.
 *\n_OTHER INFO:_
 *\n	This implementation uses 146% less memory than optlib.hpp
 */
#pragma once
#include "format-argv.hpp"
#include "parseArgs.hpp"

namespace opt {
	/**
	 * @class Params
	 * @brief Contains the list of arguments stored as variant types in a protected vector.
	 */
	class Params {
	protected:
		ContainerType _args;

	public:
		/**
		 * @brief Constructor that takes argc & argv directly from main, and a parse_config instance.
		 * @param argc			- Argument array size.
		 * @param argv			- Argument array.
		 * @param parse_config	- Parsing configuration values.
		 */
		explicit Params(const int argc, char** argv, const ParserConfig& parse_config) : _args{ parseArgs(vectorize(argc, argv), parse_config) } {}
		explicit Params(const int argc, char** argv, const std::vector<std::string>& capture_list) : _args{ parseArgs(vectorize(argc, argv), capture_list) } {}
		explicit Params(ContainerType cont) : _args{ std::move(cont) } {}

		auto begin() const { return _args.begin(); }
		auto rbegin() const { return _args.rbegin(); }
		auto end() const { return _args.end(); }
		auto rend() const { return _args.rend(); }
		auto at(const ContainerType::size_type& pos) const { return _args.at(pos); }
		auto empty() const { return _args.empty(); }

		/**
		 * @brief Retrieve an iterator to the option or parameter with a given name.
		 * @param arg				- Argument name (or capture name if check_captures) to search for.
		 * @param off				- The position in the argument vector to begin searching at. Inclusive.
		 * @param check_captures	- When true, also checks capture values for options, and flags.
		 * @returns ContainerType::const_iterator
		 */
		ContainerType::const_iterator find(const std::string& arg, ContainerType::const_iterator off, const bool check_captures = false) const
		{
			bool check_flags{ false };
			if (arg.size() == 1u)
				check_flags = true;
			for (auto it{ off }; it != _args.end(); ++it) {
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
		ContainerType::const_iterator find(const std::string& arg, const bool check_captures = false) const
		{
			return find(arg, _args.begin(), check_captures);
		}
		ContainerType::const_iterator find(const std::string& arg, const size_t off, const bool check_captures = false) const
		{
			return find(arg, _args.begin() + off, check_captures);
		}

		/**
		 * @brief Retrieve an iterator to the flag with a given character name.
		 * @param arg				- Flag name to search for.
		 * @returns ContainerType::const_iterator
		 */
		ContainerType::const_iterator find(const char& arg, ContainerType::const_iterator off) const
		{
			for (auto it{ off }; it != _args.end(); ++it) {
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
		ContainerType::const_iterator find(const char& arg) const
		{
			return find(arg, _args.begin());
		}
		ContainerType::const_iterator find(const char& arg, const size_t off) const
		{
			return find(arg, _args.begin() + off);
		}

		std::vector<ContainerType::const_iterator> findAll(const std::string& arg, const ContainerType::const_iterator& off) const
		{
			std::vector<ContainerType::const_iterator> vec;
			vec.reserve(_args.size());
			for (auto it{ find(arg, off) }; it != _args.end(); it = find(arg, it + 1u))
				vec.emplace_back(it);
			vec.shrink_to_fit();
			return vec;
		}
		std::vector<ContainerType::const_iterator> findAll(const std::string& arg) const
		{
			std::vector<ContainerType::const_iterator> vec;
			vec.reserve(_args.size());
			for (auto it{ find(arg, _args.begin()) }; it != _args.end(); it = find(arg, it + 1u))
				vec.emplace_back(it);
			vec.shrink_to_fit();
			return vec;
		}
		std::vector<ContainerType::const_iterator> findAll(const char arg, const ContainerType::const_iterator& off) const
		{
			std::vector<ContainerType::const_iterator> vec;
			vec.reserve(_args.size());
			for (auto it{ find(arg, off) }; it != _args.end(); it = find(arg, it + 1u))
				vec.emplace_back(it);
			vec.shrink_to_fit();
			return vec;
		}
		std::vector<ContainerType::const_iterator> findAll(const char arg) const
		{
			std::vector<ContainerType::const_iterator> vec;
			vec.reserve(_args.size());
			for (auto it{ find(arg, _args.begin()) }; it != _args.end(); it = find(arg, it + 1u))
				vec.emplace_back(it);
			vec.shrink_to_fit();
			return vec;
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
		/**
		 * @brief Search for a given char in the list of non-captured arguments.
		 * @param arg	- Argument name to search for.
		 * @returns bool
		 */
		bool contains(const char& arg) const
		{
			return find(arg) != _args.end();
		}

		/**
		 * @brief Retrieve all arguments with a given type.
		 * @tparam T	- Type of argument to retrieve. Accepts Flag, Parameter, or Option.
		 * @returns std::vector<T>
		 */
		template<class T> std::vector<T> getAllWithType() const
		{
			std::vector<T> vec;
			vec.reserve(_args.size());
			for (auto& it : _args) {
				bool pushThis{ false };
				if constexpr (std::is_same_v<T, Flag>) {
					if (it == Type::FLAG)
						pushThis = true;
				}
				else if constexpr (std::is_same_v<T, Option>) {
					if (it == Type::OPTION)
						pushThis = true;
				}
				else if constexpr (std::is_same_v<T, Parameter>) {
					if (it == Type::PARAMETER)
						pushThis = true;
				}
				if (pushThis)
					vec.emplace_back(it.get<T>());
			}
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief Retrieve a vector containing all args of type Flag.
		 * @returns std::vector<Flag>
		 */
		std::vector<Flag> getAllFlags() const
		{
			return getAllWithType<Flag>();
		}
		/**
		 * @brief Retrieve a vector containing all args of type Option.
		 * @returns std::vector<Option>
		 */
		std::vector<Option> getAllOptions() const
		{
			return getAllWithType<Option>();
		}
		/**
		 * @brief Retrieve a vector containing all args of type Parameter.
		 * @returns std::vector<Parameter>
		 */
		std::vector<Parameter> getAllParameters() const
		{
			return getAllWithType<Parameter>();
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

		/**
		 * @brief Check if a given argument was included, regardless of its type.
		 * @param arg	- Argument name to search for, not including any prefix dashes if applicable.
		 * @returns bool
		 */
		bool check(auto&& arg) const
		{
			return contains(std::forward<decltype(arg)>(arg));
		}

		/**
		 * @brief Check if a given argument was included, and has type Option.
		 * @param opt	- Option name to search for, not including the prefix dashes.
		 * @returns bool
		 */
		bool check_opt(const std::string& opt) const
		{
			const auto it{ find(opt) };
			return it != _args.end() && *it == Type::OPTION;
		}

		/**
		 * @brief Check if a given argument was included, and has type Flag.
		 * @param flag	- Flag char to search for.
		 * @returns bool
		 */
		bool check_flag(const char flag) const
		{
			const auto it{ find(flag) };
			return it != _args.end() && *it == Type::FLAG;
		}

		/**
		 * @brief Check if a given argument was included, and has type Parameter.
		 * @param param	- Parameter name to search for.
		 * @returns bool
		 */
		bool check_param(const std::string& param) const
		{
			const auto it{ find(param) };
			return it != _args.end() && *it == Type::PARAMETER;
		}

		// Insert all arguments in the list into an output stream.
		friend std::ostream& operator<<(std::ostream& os, const Params& obj)
		{
			for (auto& it : obj._args)
				os << it;
			return os;
		}

		template<class T> std::vector<T> getAllWithTypeMatching(const std::string& name) const
		{
			std::vector<T> vec;
			vec.reserve(_args.size());
			for (auto& it : _args) {
				bool pushThis{ false };
				if constexpr (std::is_same_v<T, Flag>) {
					if (it == Type::FLAG && it.name() == name)
						pushThis = true;
				}
				else if constexpr (std::is_same_v<T, Option>) {
					if (it == Type::OPTION && it.name() == name)
						pushThis = true;
				}
				else if constexpr (std::is_same_v<T, Parameter>) {
					if (it == Type::PARAMETER && it.name() == name)
						pushThis = true;
				}
				if (pushThis)
					vec.emplace_back(it.get<T>());
			}
			vec.shrink_to_fit();
			return vec;
		}
	};
}