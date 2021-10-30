/**
 * @file optenv.hpp
 * @author radj307
 * @brief opt Extension that provides support for environment variable parsing & usage. Requires the shared library.
 */
#pragma once
#include <OPT_PARSER_LIB.h>
#include <unordered_map>
#include <strmanip.hpp>
#include <strconv.hpp>
#ifdef SHARED_LIB

namespace opt {

	struct Environment {
		using cont_env = std::unordered_map<std::string, std::string>;
		cont_env _var;

		static cont_env parse(char* envp[])
		{
			cont_env vec;
			for ( auto i{ 0u }; envp[i] != nullptr; ++i ) {
				const auto str{ str::strip_line(envp[i]) };
				const auto dPos{ str.find('=') };
				if ( dPos != std::string::npos )
					vec[str::strip_line(str.substr(0, dPos), "")] = str::strip_line(str.substr(dPos + 1), "");
				else
					throw std::exception("Unknown Environment Variable Syntax");
			}
			return vec;
		}

		explicit Environment(char* envp[]) : _var{ parse(envp) } { }

		[[nodiscard]] bool check(const std::string& var_name, const bool case_sensitive = true) { return std::ranges::any_of(_var, [&var_name, &case_sensitive](const cont_env::value_type
			& pr) { if ( case_sensitive ) return str::tolower(pr.first) == str::tolower(var_name); return pr.first == var_name; }); }

		[[nodiscard]] std::string getv(const std::string& var_name, const bool case_sensitive = true)
		{
			if ( case_sensitive )
				return _var.at(var_name);
			const auto res{ std::ranges::find_if(_var, [&var_name](const cont_env::value_type& pr) { return str::tolower(pr.first) == str::tolower(var_name); }) };
			if ( res != _var.end() )
				return res->second;
			return{ };
		}

		[[nodiscard]] std::vector<std::string> getPath() const
		{
			std::stringstream PATH{ [this]() -> std::string {
				try {
					return _var.at("Path");
				} catch (...) {
					return _var.at("PATH");
				}
			}() };
			std::vector<std::string> vec;
			const auto push{ [&vec](const std::string& entry){ if (!entry.empty()) vec.push_back(entry); } };
			for (std::string entry{}; std::getline(PATH, entry, ';'); push(entry)) {
				const auto dPos{ entry.find(';') };
				if (str::pos_valid(dPos))
					entry.erase(dPos, dPos);
			}
			return vec;
		}

		[[nodiscard]] bool empty() const
		{
			return _var.empty();
		}
	};
}
#else
#error optenv.hpp requires the shared library!
#endif