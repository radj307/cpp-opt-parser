/**
 * @file VariantArgument.hpp
 * @author radj307
 * @brief	Contains the VariantArgument struct & methods for manipulating variants. This is used by the parseArgs.hpp header.
 */
#pragma once
#include <iostream>
#include <string>
#include <optional>
#include <vector>
#include <variant>
#include <VariantType.hpp>

namespace opt {
	/**
	 * @struct VariantArgument
	 * @brief Wrapper object for VariantType, allows types Parameter, Option, or Flag.
	 */
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
		std::string name() const noexcept
		{
			switch ( _type ) {
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
			return get<Option>().second;
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
			return !this->operator==(o);
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
			return !this->operator==(o);
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
			using enum Type;
			switch ( obj._type ) {
			case OPTION:
				os << '-';
				[[fallthrough]];
			case FLAG:
				os << '-';
				[[fallthrough]];
			case PARAMETER: [[fallthrough]];
			default:
				os << obj.name();
				break;
			}
			return os;
		}

		operator VariantType() const { return _arg; }

		explicit operator Parameter() const
		{
			return std::get<Parameter>(_arg);
		}
		explicit operator Option() const
		{
			return std::get<Option>(_arg);
		}
		explicit operator Flag() const
		{
			return std::get<Flag>(_arg);
		}
	};
}