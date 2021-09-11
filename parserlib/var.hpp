/**
 *
 */
#pragma once
namespace var {
#pragma region BOOL
	/**
	 * @function variadic_or(const T...)
	 * @brief Boolean OR comparison that accepts any number of variadic boolean arguments.
	 * @tparam ...T		- Variadic Template Parameter Pack.
	 * @param booleans	- Any number of boolean values to compare.
	 * @returns bool
	 */
	template<typename...T> static bool variadic_or(const T... booleans)
	{
		return ((booleans) || ...);
	}
	/**
	 * @function variadic_and(const T...)
	 * @brief Boolean AND comparison that accepts any number of variadic boolean arguments.
	 * @tparam ...T		- Variadic Template Parameter Pack.
	 * @param booleans	- Any number of boolean values to compare.
	 * @returns bool
	 */
	template<typename...T> static bool variadic_and(const T... booleans)
	{
		return ((booleans) && ...);
	}
#pragma endregion BOOL
}