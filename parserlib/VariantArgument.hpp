#pragma once
#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <variant>

namespace opt {

	using Parameter = std::string;										///< @brief The type used to store parameters from the commandline in a VariantArgument.
	using Option = std::pair<std::string, std::optional<std::string>>;	///< @brief The type used to store options (long-opts) from the commandline in a VariantArgument.
	using Flag = std::pair<char, std::optional<std::string>>;			///< @brief The type used to store flags (short-opts) from the commandline in a VariantArgument.

	using VariantType = std::variant<std::monostate, Parameter, Option, Flag>; ///< @brief The variant type used in VariantArgument. Monostate variant.

	/**
	 * @brief Contains the possible types of a VariantArgument. This is available so the implementation can identify which type a VariantArgument is without cumbersome std::get_if<>() function calls.
	 */
	enum class Type {
		NULL_TYPE,
		PARAMETER,	///< @brief Corresponds to the Parameter type.
		OPTION,		///< @brief Corresponds to the Option type.
		FLAG		///< @brief Corresponds to the Flag type.
	};

	/**
	 * @brief Retrieve an enumerator representing the index of a given variant.
	 * @param var	- A type-variant variable.
	 * @returns Type
	 */
	inline Type determineVariantType(const VariantType& var)
	{
		switch (var.index()) {
		case 0: // type is std::monostate
			return Type::NULL_TYPE;
		case 1: // type is Parameter
			return Type::PARAMETER;
		case 2: // type is Option
			return Type::OPTION;
		case 3: // type is Flag
			return Type::FLAG;
		default:
			throw std::exception("Unknown variant index! (Was a type added?)");
		}
	}

	struct VariantArgument {
	private:
		VariantType _arg; ///< @brief The argument of this instance, stored in a std::variant.
		Type _type; ///< @brief The type of this instance.
	public:
		/**
		 * @brief Default Constructor.
		 * @param value	- The variant value (argument) of this instance.
		 */
		VariantArgument(VariantType value) : _arg{ std::move(value) }, _type{ determineVariantType(_arg) } {}

		/**
		 * @brief Retrieve the argument/name of this VariantArgument. This cannot be null or blank in normal operation. Flag chars are converted to std::string.
		 * @returns std::string
		 *\n	If the returned string is blank, this argument is null.
		 */
		std::string name() const
		{
			if (_arg.valueless_by_exception())
				return{};
			switch (_type) {
			case Type::PARAMETER:
				return get<Parameter>();
			case Type::OPTION:
				return get<Option>().first;
			case Type::FLAG:
				return std::string(1u, get<Flag>().first);
			default:
				return{};
			}
		}

		/**
		 * @brief Check if this argument has a captured parameter.
		 * @returns bool
		 */
		bool hasv() const
		{
			switch (_type) {
			case Type::FLAG:
				return get<Flag>().second.has_value();
			case Type::OPTION:
				return get<Option>().second.has_value();
			case Type::PARAMETER:[[fallthrough]];
			default:
				return false;
			}
		}

		/**
		 * @brief Retrieve a copy of this VariantArgument's argument.
		 * @returns VariantType
		 */
		VariantType arg() const { return _arg; }

		/**
		 * @brief Retrieve this VariantArgument's type.
		 * @returns Type
		 */
		Type type() const { return _type; }

		/**
		 * @brief Retrieve this instance's argument of type Parameter.
		 * @tparam T	- Parameter type.
		 * @returns Parameter
		 * @throws std::bad_variant_access	- If this VariantArgument is not a Parameter.
		 */
		template<class T> std::enable_if_t<std::is_same_v<T, Parameter>, T> get() const
		{
			return std::get<Parameter>(_arg);
		}

		/**
		 * @brief Retrieve this instance's argument of type Option.
		 * @tparam T	- Option type.
		 * @returns Option
		 * @throws std::bad_variant_access	- If this VariantArgument is not an Option.
		 */
		template<class T> std::enable_if_t<std::is_same_v<T, Option>, T> get() const
		{
			return std::get<Option>(_arg);
		}

		/**
		 * @brief Retrieve this instance's argument of type Flag.
		 * @tparam T	- Flag type.
		 * @returns Flag
		 * @throws std::bad_variant_access	- If this VariantArgument is not a Flag.
		 */
		template<class T> std::enable_if_t<std::is_same_v<T, Flag>, T> get() const
		{
			return std::get<Flag>(_arg);
		}
		/**
		 * @brief Retrieve a copy of this VariantArgument's argument.
		 * @returns VariantType
		 */
		VariantType get() const
		{
			return _arg;
		}

		/**
		 * @brief Retrieve the captured value of this flag.
		 * @tparam T	- Flag type.
		 * @returns std::optional<std::string>
		 * @throws std::bad_optional_access	- If this VariantArgument is not a Flag.
		 */
		template<class T> std::enable_if_t<std::is_same_v<T, Flag>, std::optional<std::string>> getv() const
		{
			return get<Flag>().second.value();
		}
		/**
		 * @brief Retrieve the captured value of this option.
		 * @tparam T						- Option type.
		 * @returns std::optional<std::string>
		 * @throws std::bad_optional_access	- If this VariantArgument is not an Option.
		 */
		template<class T> std::enable_if_t<std::is_same_v<T, Option>, std::optional<std::string>> getv() const
		{
			return get<Option>().second.value();
		}

		std::optional<std::string> getv() const
		{
			switch (_type) {
			case Type::FLAG:
				return getv<Flag>();
			case Type::OPTION:
				return getv<Option>();
			default:
				return{};
			}
		}

		/**
		 * @brief Compare this VariantArgument instance's type & arg against another VariantArgument instance's type & arg.
		 * @param o	- Other type.
		 * @returns bool
		 */
		bool operator==(const VariantArgument& o) const
		{
			return _type == o._type && _arg == o._arg;
		}
		/**
		 * @brief Compare this VariantArgument instance's type & arg against another VariantArgument instance's type & arg.
		 * @param o	- Other type.
		 * @returns bool
		 */
		bool operator!=(const VariantArgument& o) const
		{
			return _type == o._type && _arg == o._arg;
		}
		/**
		 * @brief Compare this VariantArgument instance's type against a given type.
		 * @param o	- Other type.
		 * @returns bool
		 */
		bool operator==(const Type& o) const
		{
			return _type == o;
		}
		/**
		 * @brief Compare this VariantArgument instance's type against a given type.
		 * @param o	- Other type.
		 * @returns bool
		 */
		bool operator!=(const Type& o) const
		{
			return _type != o;
		}

		/**
		 * @brief Retrieve a reference to this instance.
		 * @returns VariantArgument&
		 */
		VariantArgument& operator()()
		{
			return *this;
		}
		/**
		 * @brief Retrieve a copy of this instance.
		 * @returns VariantArgument
		 */
		VariantArgument operator()() const
		{
			return *this;
		}

		/**
		 * @brief Set the value & type of this VariantArgument instance.
		 * @param is	- Input stream instance.
		 * @param obj	- VariantArgument instance.
		 * @returns std::istream&
		 */
		friend std::istream& operator>>(std::istream& is, VariantArgument& obj)
		{
			std::string str;
			is >> str;
			obj._arg = str;
			obj._type = determineVariantType(obj._arg);
			return is;
		}
		/**
		 * @brief Insert the value & potential capture value of this VariantArgument instance.
		 * @param os	- Output stream instance.
		 * @param obj	- VariantArgument instance.
		 * @returns std::ostream&
		 */
		friend std::ostream& operator<<(std::ostream& os, const VariantArgument& obj)
		{
			switch (obj._type) {
			case Type::PARAMETER:
				os << obj.get<Parameter>();
				break;
			case Type::OPTION:
				os << obj.get<Option>().first;
				break;
			case Type::FLAG:
				os << obj.get<Flag>().first;
				break;
			}
			return os;
		}
	};
}