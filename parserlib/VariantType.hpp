#pragma once
#include <string>
#include <utility>
#include <variant>
#include <optional>
namespace opt {
	using Parameter = std::string;										///< @brief The type used to store parameters from the commandline in a VariantArgument.
	using Option = std::pair<std::string, std::optional<std::string>>;	///< @brief The type used to store options (long-opts) from the commandline in a VariantArgument.
	using Flag = std::pair<char, std::optional<std::string>>;			///< @brief The type used to store flags (short-opts) from the commandline in a VariantArgument.

	using VariantType = std::variant<std::monostate, Parameter, Option, Flag>; ///< @brief The variant type used in VariantArgument. Monostate variant.

	// Concept that only allows Parameter, Option, and/or Flag types.
	template<class Ty> concept ValidArgumentType = requires( Ty type )
	{
		std::is_same_v<Ty, Parameter> || std::is_same_v<Ty, Option> || std::is_same_v<Ty, Flag>;
	};

	/**
	 * @brief Contains the possible types of a VariantArgument. This is available so the implementation can identify which type a VariantArgument is without cumbersome std::get_if<>() function calls.
	 */
	enum class Type : unsigned {
		MONOSTATE = 0u,	///< @brief NULL type
		PARAMETER = 1u,	///< @brief Parameter type
		OPTION = 2u,	///< @brief Option type
		FLAG = 3u		///< @brief Flag type
	};

	/**
	 * @brief Retrieve an enumerator representing the index of a given variant.
	 * @param var	- A type-variant variable.
	 * @returns Type
	 */
	inline Type determineVariantType(const VariantType& var)
	{
		switch ( var.index() ) {
		case 0: // type is std::monostate
			return Type::MONOSTATE;
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
	/**
	 * @brief Retrieve the Type associated with a given typename.
	 * @tparam T	- Input type
	 * @returns Type
	 */
	template<class T>
	constexpr static Type determineVariantType()
	{
		using enum Type;
		if constexpr ( std::is_same_v<T, Parameter> )
			return PARAMETER;
		if constexpr ( std::is_same_v<T, Option> )
			return OPTION;
		if constexpr ( std::is_same_v<T, Flag> )
			return FLAG;
		return MONOSTATE;
	}
	/**
	 * @brief Convert a variable type to an enum Type.
	 * @tparam T	- Variable type. Parameter, Option, and Flag are accepted, anything else will return MONOSTATE
	 * @returns Type
	 */
	template<class T> Type getType()
	{
		if constexpr ( std::is_same_v<T, Parameter> )
			return Type::PARAMETER;
		if constexpr ( std::is_same_v<T, Option> )
			return Type::OPTION;
		if constexpr ( std::is_same_v<T, Flag> )
			return Type::FLAG;
		return Type::MONOSTATE;
	}

	/**
	 * @brief Returns a Type as plaintext.
	 * @param type	- Type to convert
	 * @returns std::string
	 */
	inline std::string get_typename(const Type& type)
	{
		using enum Type;
		switch ( type ) {
		case PARAMETER:
			return "PARAMETER";
		case OPTION:
			return "OPTION";
		case FLAG:
			return "FLAG";
		default:
			return "NULL";
		}
	}

}