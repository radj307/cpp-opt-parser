/**
 * @file resolve-path.hpp
 * @author radj307
 * @brief Contains helper methods for parsing argv[0] to resolve the location of the currently running executable. Also functions as a basic whereis command.
 */
#pragma once
#include <OPT_PARSER_LIB.h>
#include <file.h>		///< @brief Requires file.h from sharedlib!
#include <optenv.hpp>
#ifdef SHARED_LIB

namespace opt {
	/**
	 * @function split_arg(const std::string&, const std::string& = "./\")
	 * @brief Splits a filepath between the filename and path to that file.
	 * @param arg			- argv[0] from main() or another patch to resolve.
	 * @param delimiters	- Path delimiters.
	 * @returns std::pair<std::string, std::string>
	 *\n		first	- The path to the directory where this program is located.
	 *\n		second	- The name used to call this program.
	 */
	inline std::pair<std::string, std::string> split_path(const std::string& arg, const std::string& delimiters = "/\\")
	{
		if (const auto dPos{ arg.find_last_of(delimiters) }; str::pos_valid(dPos))
			return { arg.substr(0u, dPos + 1u), arg.substr(dPos + 1u) };
		return { {}, arg };
	}

	/**
	 * @function resolve_split_path(const std::vector<std::string>&, const std::string&, const char = '/')
	 * @brief Parse the user's PATH environment variable to find the location of argv[0]
	 * @param PATH			- Contents of the PATH environment variable. Can be retrieve from opt::Environment::getPath()
	 * @param arg			- argv[0] from main() or another path to resolve.
	 * @param extensions	- A list of possible file extensions to append to arg before giving up on a directory.
	 * @param pathDelim		- Delimiter used to separate paths. ( '/' on UNIX, '\' on Windows )
	 * @returns std::pair<std::string, std::string>
	 *\n		first		- The path to the directory where this program is located, including a trailing slash.
	 *\n		second		- The name used to call this program.
	 */
	inline std::pair<std::string, std::string> resolve_split_path(const std::vector<std::string>& PATH, const std::string& arg, const std::vector<std::string>& extensions = { ".exe", ".bat", ".so" }, const char pathDelim = '/')
	{
		const auto [path, name] { split_path(arg) };
		if (!path.empty() && !str::pos_valid(path.find('.')))
			return{ path, name }; // return absolute path
		// iterate through PATH env var
		for (auto& it : PATH) {
			std::string target(it + pathDelim);
			if (file::exists(target + arg))
				return { target, arg };
			for (auto& ext : extensions)
				if (file::exists(std::string{ target + arg }.append(ext)))
					return { target, { arg + ext } };
		}
		return { {}, arg }; // return not found
	}

	/**
	 * @function resolve_split_path(const std::vector<std::string>&, const std::string&)
	 * @brief Parse the user's PATH environment variable to find the location of argv[0]
	 * @param envp	- envp[] from main()
	 * @param arg	- argv[0] from main()
	 * @returns std::pair<std::string, std::string>
	 *\n		first	- The path to the directory where this program is located, including a trailing slash.
	 *\n		second	- The name used to call this program.
	 */
	inline std::pair<std::string, std::string> resolve_split_path(char** envp, const std::string& arg)
	{
		return resolve_split_path(Env{ envp }.PATH(), arg);
	}

	/**
	 * @function resolve_path(T&&, const std::string&)
	 * @brief Parse the user's PATH environment variable to find the location of argv[0]
	 * @tparam T			- Templated environment type. Accepts char*[] & string vectors.
	 * @param env			- Perfect-Forwarded environment variable type, passed to resolve_split_path.
	 * @param arg			- argv[0] from main() or another path to resolve.
	 * @returns std::string	- This is the same as calling resolve_split_path and appending second value to the first.
	 */
	template<class T> static std::string resolve_path(T&& env, const std::string& arg)
	{
		const auto [path, name] { resolve_split_path(std::forward<T>(env), arg) };
		return path + name;
	}
}
#else
#error resolve-path.hpp requires the shared library!
#endif