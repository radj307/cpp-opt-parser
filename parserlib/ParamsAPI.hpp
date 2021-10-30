/**
 * @file ParamsAPI.hpp
 * @author radj307
 * @brief Contains the ParamsAPI class, as well as concepts & functions used to abstract input types.
 */
#pragma once
#include <concepts>
#include <vectorize.hpp>
#include <var.hpp>
#include <parseArgs.hpp>

namespace opt {
	// Concept that only allows std::string/char* or char
	template<class T> concept ValidInputType = requires( T type )
	{
		std::is_same_v<T, std::string> || std::is_same_v<T, char> || std::is_same_v<T, char*>;
	};

	/** @brief Resolve ValidInputType::char -> std::string */
	template<ValidInputType Ty> requires std::is_same_v<Ty, char> static constexpr const std::string to_string(const Ty& ch) { return std::string(1u, ch); }
	/** @brief Resolve other ValidInputType -> std::string */
	template<ValidInputType Ty> requires std::convertible_to<Ty, std::string> static constexpr const std::string to_string(const Ty& str) { return {str}; }

	/**
	 * @class ParamsAPI
	 * @brief Cleaner, more optimized implementation of the Params class.
	 */
	class ParamsAPI {
	public:
		using IteratorContainerT = std::vector<ContainerType::const_iterator>; ///< @brief Macro for a vector of ContainerType::const_iterators

	private:
		std::optional<std::string> _arg0; ///< @brief Contains argv[0], if it could be found during initialization.
		ContainerType _args; ///< @brief Internal container for holding arguments as VariantArgument types.

	public:
		/**
		 * @brief Default/Empty Constructor.
		 */
		explicit ParamsAPI() : _arg0{ std::nullopt } {}
		/**
		 * @brief Constructor that accepts arguments directly from main(), and parses them automatically.
		 * @param argc			- Argument Array Size
		 * @param argv			- Argument Array
		 * @param parser_cfg	- Parser Config Instance, if std::nullopt is received, uses the default ParserConfig instance.
		 */
		explicit ParamsAPI(const int argc, char** argv, std::optional<ParserConfig> parser_cfg = std::nullopt) : _arg0{ argv[0] }, _args{ parseArgs(vectorize(argc, argv), parser_cfg.value_or(ParserConfig{})) } {}

		/**
		 * @brief Variadic Constructor that accepts flag/option names that capture additional arguments.
		 * @tparam VT...	- Variadic Template of ValidInputType. (std::string, char*, char)
		 * @param argc		- Argument Array Size
		 * @param argv		- Argument Array
		 * @param captures	- Capturing arguments passed to Parser Config.
		 */
		template<ValidInputType... VT> requires (sizeof...(VT) > 0)
		explicit ParamsAPI(const int argc, char** argv, VT... captures) :
			_arg0{ argv[0] },
			_args{ parseArgs(vectorize(argc, argv),
				ParserConfig{
						var::variadic_accumulate<std::string>(to_string(captures)...)
				})
			}
		{}

		/**
		 * @brief Constructor that takes the rvalue ref of a vector of strings, and parses them automatically.
		 * @param args			- Argument Vector
		 * @param parser_cfg	- Parser Config Instance, if std::nullopt is received, uses the default ParserConfig instance.
		 * @param arg0			- Optional Argument 0.
		 */
		explicit ParamsAPI(std::vector<std::string>&& args, std::optional<ParserConfig> parser_cfg = std::nullopt, std::optional<std::string> arg0 = std::nullopt) : _arg0{ std::move(arg0) }, _args{ parseArgs(args, parser_cfg.value_or(ParserConfig{})) } {}
		/**
		 * @brief Container-Move Constructor.
		 * @param arg_container	- Container of arguments to move into this instance.
		 * @param arg0			- Optional argument 0 override.
		 */
		explicit ParamsAPI(ContainerType&& arg_container, std::optional<std::string> arg0 = std::nullopt) : _arg0{ std::move(arg0) }, _args{ std::move(arg_container) } {}

		[[nodiscard]] auto begin() const { return _args.begin(); }					///< @brief Forward ContainerType::begin()	@returns ContainerType::const_iterator
		[[nodiscard]] auto end() const { return _args.end(); }						///< @brief Forward ContainerType::end()	@returns ContainerType::const_iterator
		[[nodiscard]] auto rbegin() const { return _args.rbegin(); }				///< @brief Forward ContainerType::rbegin()	@returns std::reverse_iterator<ContainerType::const_iterator>
		[[nodiscard]] auto rend() const { return _args.rend(); }					///< @brief Forward ContainerType::rend()	@returns std::reverse_iterator<ContainerType::const_iterator>
		[[nodiscard]] auto front() const { return _args.front(); }					///< @brief Forward ContainerType::front()	@returns VariantArgument
		[[nodiscard]] auto back() const { return _args.back(); }					///< @brief Forward ContainerType::back()	@returns VariantArgument
		[[nodiscard]] auto at(const size_t& pos) const { return _args.at(pos); }	///< @brief Forward ContainerType::at()		@returns VariantArgument
		[[nodiscard]] auto empty() const { return _args.empty(); }					///< @brief Forward ContainerType::empty()	@returns bool

		/**
		 * @brief Get an argument from the container.
		 * @tparam T	- std::string / char* / char
		 * @param arg	- Argument name to search for.
		 * @param off	- Position in the container to begin searching at.
		 * @returns std::optional<VariantArgument>
		 */
		template<ValidInputType T> [[nodiscard]] std::optional<VariantArgument> get(const T& arg, ContainerType::const_iterator off) const
		{
			if ( const auto pos{ find(to_string(arg), off) }; pos != _args.end() )
				return *pos;
			return std::nullopt;
		}
		/**
		 * @brief Get an argument from the container.
		 * @param arg	- Argument name to search for.
		 * @returns std::optional<VariantArgument>
		 */
		[[nodiscard]] std::optional<VariantArgument> get(auto&& arg) const
		{
			return get(std::forward<decltype(arg)>(arg), _args.begin());
		}
		/**
		 * @brief Get an argument with a specific type from the container.
		 * @tparam SearchTy	- Parameter / Option / Flag
		 * @tparam T		- std::string / char* / char
		 * @param arg		- Argument name to search for.
		 * @param off		- Position in the container to begin searching at.
		 * @returns std::optional<VariantArgument>
		 */
		template<ValidArgumentType SearchTy, ValidInputType T>
		[[nodiscard]] std::optional<VariantArgument> get(const T& arg, ContainerType::const_iterator off) const
		{
			if ( const auto pos{ find<SearchTy>(to_string(arg), off)}; pos != _args.end())
				return *pos;
			return std::nullopt;
		}
		/**
		 * @brief Get an argument with a specific type from the container.
		 * @tparam SearchTy	- Parameter / Option / Flag
		 * @param arg		- Argument name to search for.
		 * @returns std::optional<VariantArgument>
		 */
		template<ValidArgumentType SearchTy>
		[[nodiscard]] std::optional<VariantArgument> get(auto&& arg) const
		{
			return get<SearchTy>(std::forward<decltype(arg)>(arg));
		}
		/**
		 * @brief Get the captured value of an argument from the container.
		 * @tparam T		- std::string / char* / char
		 * @param arg		- Argument name to search for.
		 * @param off		- Position in the container to begin searching at.
		 * @returns std::optional<std::string>
		 */
		template<ValidInputType T>
		[[nodiscard]] std::optional<std::string> getv(const T& arg, ContainerType::const_iterator off) const
		{
			static_assert( ValidInputType<T>, "Invalid input type! Must be std::string, char*, or char!" );
			if ( const auto pos{ find(to_string(arg), off) }; pos != _args.end() && pos->hasv() )
				return pos->getv();
			return std::nullopt;
		}
		/**
		 * @brief Get the captured value of an argument from the container.
		 * @param arg		- Argument name to search for.
		 * @returns std::optional<std::string>
		 */
		[[nodiscard]] std::optional<std::string> getv(auto&& arg) const
		{
			return getv(std::forward<decltype(arg)>(arg), _args.begin());
		}
		/**
		 * @brief Get the captured value of an argument with a specific type from the container.
		 * @tparam SearchTy	- Parameter / Option / Flag
		 * @tparam T		- std::string / char* / char
		 * @param arg		- Argument name to search for.
		 * @param off		- Position in the container to begin searching at.
		 * @returns std::optional<std::string>
		 */
		template<class SearchTy, ValidInputType T> requires std::is_same_v<SearchTy, Option> || std::is_same_v<SearchTy, Flag>
		[[nodiscard]] std::optional<std::string> getv(const T& arg, ContainerType::const_iterator off) const
		{
			if ( const auto pos{ find<SearchTy>(to_string(arg), off) }; pos != _args.end() && pos->hasv() )
				return pos->getv();
			return std::nullopt;
		}
		/**
		 * @brief Get the captured value of an argument with a specific type from the container.
		 * @tparam SearchTy	- Parameter / Option / Flag
		 * @param arg		- Argument name to search for.
		 * @returns std::optional<std::string>
		 */
		template<class SearchTy> requires std::is_same_v<SearchTy, Option> || std::is_same_v<SearchTy, Flag>
		[[nodiscard]] std::optional<std::string> getv(auto&& arg) const
		{
			return getv<SearchTy>(std::forward<decltype(arg)>(arg), _args.begin());
		}

		/**
		 * @brief Retrieve the value of argv[0], if it was found during initialization. (Any constructor that accepts argc/argv)
		 * @returns std::optional<std::string>
		 */
		[[nodiscard]] auto arg0() const { return _arg0; }

		/**
		 * @brief Retrieve an iterator to an argument in the container.
		 * @tparam T		- std::string / char* / char
		 * @param arg		- Argument name to search for.
		 * @param off		- Position in the container to begin searching at.
		 * @returns ContainerType::const_iterator
		 */
		template<ValidInputType T>
		[[nodiscard]] auto find(const T& arg, ContainerType::const_iterator off) const
		{
			const auto argstr{ to_string(arg) };
			return std::find_if(off, _args.end(), [&argstr](const VariantArgument& elem) {
				return elem.name() == argstr;
				});
		}
		/**
		 * @brief Retrieve an iterator to an argument in the container.
		 * @param arg		- Argument name to search for.
		 * @returns ContainerType::const_iterator
		 */
		[[nodiscard]] auto find(auto&& arg) const
		{
			return find(std::forward<decltype(arg)>(arg), _args.begin());
		}
		/**
		 * @brief Retrieve an iterator to an argument with a specific type in the container.
		 * @tparam SearchTy	- Parameter / Option / Flag
		 * @tparam T		- std::string / char* / char
		 * @param arg		- Argument name to search for.
		 * @param off		- Position in the container to begin searching at.
		 * @returns ContainerType::const_iterator
		 */
		template<ValidArgumentType SearchTy, ValidInputType T> [[nodiscard]] auto find(const T& arg, ContainerType::const_iterator off) const
		{
			Type target_type{ determineVariantType<SearchTy>() };
			const auto argstr{ to_string(arg) };
			return std::find_if(off, _args.end(), [&target_type, &argstr](const VariantArgument& elem) {
				return elem.type() == target_type && elem.name() == argstr;
				});
		}
		/**
		 * @brief Retrieve an iterator to an argument with a specific type in the container.
		 * @tparam SearchTy	- Parameter / Option / Flag
		 * @param arg		- Argument name to search for.
		 * @returns ContainerType::const_iterator
		 */
		template<ValidArgumentType SearchTy> [[nodiscard]] auto find(auto&& arg) const
		{
			return find<SearchTy>(std::forward<decltype(arg)>(arg), _args.begin());
		}

		// Return a copy of the container
		[[nodiscard]] ContainerType getAll() const { return _args; }

		template<ValidArgumentType SearchTy, class RT> requires std::is_same_v<RT, IteratorContainerT>
		[[nodiscard]] RT getWithType(ContainerType::const_iterator first, ContainerType::const_iterator last) const
		{
			IteratorContainerT vec;
			vec.reserve(_args.size());
			for (auto it{ first }; it != last; ++it) {
				if constexpr (std::is_same_v<SearchTy, Parameter>) {
					if (it->type() == Type::PARAMETER)
						vec.emplace_back(it);
				}
				else if constexpr (std::is_same_v<SearchTy, Option>) {
					if (it->type() == Type::OPTION)
						vec.emplace_back(it);
				}
				else if constexpr (std::is_same_v<SearchTy, Flag>) {
					if (it->type() == Type::FLAG)
						vec.emplace_back(it);
				}
			}
			vec.shrink_to_fit();
			return vec;
		}
		template<ValidArgumentType SearchTy, class RT> requires std::is_same_v<RT, std::vector<SearchTy>>
		[[nodiscard]] RT getWithType(ContainerType::const_iterator first, ContainerType::const_iterator last) const
		{
			std::vector<SearchTy> vec;
			vec.reserve(_args.size());
			for (auto it{ first }; it != last; ++it) {
				if constexpr (std::is_same_v<SearchTy, Parameter>) {
					if (it->type() == Type::PARAMETER)
						vec.emplace_back(static_cast<Parameter>(*it));
				}
				else if constexpr (std::is_same_v<SearchTy, Option>) {
					if (it->type() == Type::OPTION)
						vec.emplace_back(static_cast<Option>(*it));
				}
				else if constexpr (std::is_same_v<SearchTy, Flag>) {
					if (it->type() == Type::FLAG)
						vec.emplace_back(static_cast<Flag>(*it));
				}
			}
			vec.shrink_to_fit();
			return vec;
		}

		/**
		 * @brief Retrieve a vector of iterators to all arguments of a given type found on the commandline.
		 * @tparam SearchTy	- Type of argument to search for. ( Parameter | Option | Flag )
		 * @tparam RT		- Type of container to return
		 * @returns std::vector<ContainerType::const_iterator>
		 */
		template<ValidArgumentType SearchTy, class RT> [[nodiscard]] std::enable_if_t<std::is_same_v<RT, IteratorContainerT>, IteratorContainerT> getAllWithType() const
		{
			IteratorContainerT vec;
			vec.reserve(_args.size());
			for ( auto it{ _args.begin() }; it != _args.end(); ++it ) {
				if constexpr (std::is_same_v<SearchTy, Parameter>) {
					if (it->type() == Type::PARAMETER)
						vec.emplace_back(it);
				}
				else if constexpr (std::is_same_v<SearchTy, Option>) {
					if (it->type() == Type::OPTION)
						vec.emplace_back(it);
				}
				else if constexpr (std::is_same_v<SearchTy, Flag>) {
					if (it->type() == Type::FLAG)
						vec.emplace_back(it);
				}
			}
			vec.shrink_to_fit();
			return vec;
		}
		/**
		 * @brief Retrieve a vector of all arguments of a given type found on the commandline.
		 * @tparam SearchTy	- Type of argument to search for. ( Parameter | Option | Flag )
		 * @tparam RT		- Type of container to return
		 * @returns std::vector<T>
		 */
		template<ValidArgumentType SearchTy, class RT> [[nodiscard]] std::enable_if_t<std::is_same_v<RT, std::vector<SearchTy>>, std::vector<SearchTy>> getAllWithType() const
		{
			std::vector<SearchTy> vec;
			vec.reserve(_args.size());
			for ( auto it{ _args.begin() }; it != _args.end(); ++it ) {
				if constexpr (std::is_same_v<SearchTy, Parameter>) {
					if (it->type() == Type::PARAMETER)
						vec.emplace_back(static_cast<Parameter>(*it));
				}
				else if constexpr (std::is_same_v<SearchTy, Option>) {
					if (it->type() == Type::OPTION)
						vec.emplace_back(static_cast<Option>(*it));
				}
				else if constexpr (std::is_same_v<SearchTy, Flag>) {
					if (it->type() == Type::FLAG)
						vec.emplace_back(static_cast<Flag>(*it));
				}
			}
			vec.shrink_to_fit();
			return vec;
		}

		/**
		 * @brief Retrieve a templated container with all Parameters found on the commandline.
		 * @tparam RT	- Return Type, can be ( IteratorContainerT || std::vector<( Parameter | Option | Flag )> )
		 * @returns RT
		 */
		template<class RT = IteratorContainerT> [[nodiscard]] RT getAllParameters() const { return getAllWithType<Parameter, RT>(); }
		/**
		 * @brief Retrieve a templated container with all Options found on the commandline.
		 * @tparam RT	- Return Type, can be ( IteratorContainerT || std::vector<( Parameter | Option | Flag )> )
		 * @returns RT
		 */
		template<class RT = IteratorContainerT> [[nodiscard]] RT getAllOptions() const { return getAllWithType<Option, RT>(); }
		/**
		 * @brief Retrieve a templated container with all Flags found on the commandline.
		 * @tparam RT	- Return Type, can be ( IteratorContainerT || std::vector<( Parameter | Option | Flag )> )
		 * @returns RT
		 */
		template<class RT = IteratorContainerT> [[nodiscard]] RT getAllFlags() const { return getAllWithType<Flag, RT>(); }

		/**
		 * @brief Check if an argument with any type was included on the commandline.
		 * @param arg	- Argument name to search for.
		 * @returns bool
		 */
		template<ValidInputType T>
		[[nodiscard]] bool check(const T& arg) const
		{
			return find(to_string(arg)) != _args.end();
		}
		/**
		 * @brief Check if an argument with a specified type was included on the commandline.
		 * @tparam T	- Type of argument to search for. ( Parameter | Option | Flag )
		 * @param arg	- Argument name to search for.
		 * @returns bool
		 */
		template<ValidArgumentType SearchTy, ValidInputType T> [[nodiscard]] bool check(const T& arg) const
		{
			return find<SearchTy>(to_string(arg)) != _args.end();
		}
		/**
		 * @brief Check if a specified Option was included on the commandline.
		 * @param arg	- Argument name to search for.
		 * @returns bool
		 */
		[[nodiscard]] bool check_opt(auto&& arg) const { return check<Option>(std::forward<decltype(arg)>(arg)); }
		/**
		 * @brief Check if a specified Parameter was included on the commandline.
		 * @param arg	- Argument name to search for.
		 * @returns bool
		 */
		[[nodiscard]] bool check_param(auto&& arg) const { return check<Parameter>(std::forward<decltype(arg)>(arg)); }
		/**
		 * @brief Check if a specified Flag was included on the commandline.
		 * @param arg	- Argument name to search for.
		 * @returns bool
		 */
		[[nodiscard]] bool check_flag(auto&& arg) const { return check<Flag>(std::forward<decltype(arg)>(arg)); }

		/** @brief Conversion operator that returns a copy of the internal argument container. */
		operator ContainerType() const { return _args; }

		/**
		 * @brief Stream insertion operator. Output is in the same format as the commandline, delimited by spaces.
		 * @param os	- (implicit) Target Output Stream.
		 * @param obj	- (implicit) ParamsAPI instance.
		 * @returns std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const ParamsAPI& obj)
		{
			for ( auto it{ obj._args.begin() }; it != obj._args.end(); ++it ) {
				os << *it;
				if ( it != obj._args.end() - 1u ) // insert a space for every argument except the last.
					os << ' ';
			}
			return os;
		}
	};
}