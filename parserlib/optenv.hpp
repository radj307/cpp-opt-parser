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
	struct VariantVariable {
		using VariantT = std::variant<std::monostate, std::string, std::vector<std::string>>;
		std::string _name;
		VariantT _value;

		VariantVariable(std::string name, VariantT value) : _name{ std::move(name) }, _value{ std::move(value) } {}

		operator VariantT() const { return _value; }

		template<typename T>
		constexpr bool is_type() const { return std::holds_alternative<T>(_value); }
		constexpr bool is_null() const { return is_type<std::monostate>(); }
		/// @brief Check if this VariantVariable is a string. If it is, you can safely use the value_string().value() functions.
		constexpr bool is_string() const { return is_type<std::string>(); }
		/// @brief Check if this VariantVariable is an array. If it is, you can safely use the value_array().value() functions.
		constexpr bool is_array() const { return is_type<std::vector<std::string>>(); }

		std::string name() const { return _name; }
		VariantT value() const { return _value; }

		std::optional<std::string> value_string() const { if (is_string()) return std::get<std::string>(_value); return std::nullopt; }
		std::optional<std::vector<std::string>> value_array() const { if (is_array()) return std::get<std::vector<std::string>>(_value); return std::nullopt; }

		friend std::ostream& operator<<(std::ostream& os, const VariantVariable& var)
		{
			os << var.name() << '=';
			if (var.is_string())
				os << std::get<std::string>(var.value());
			else if (var.is_array()) {
				const auto arr{ std::get<std::vector<std::string>>(var.value()) };
				for (auto it{ arr.begin() }; it != arr.end(); ++it)
					if (os << *it; it != arr.end() - 1ull)
						os << ';';
			}
			return os;
		}
	};

	using EnvContainer = std::vector<VariantVariable>;

	inline EnvContainer parse_envp(char** envp)
	{
		EnvContainer vec;

		for (unsigned i{ 0u }; envp[i] != nullptr; ++i) {
			std::string ln{ envp[i] };
			const auto eqPos{ ln.find('=') };
			vec.emplace_back(std::move(VariantVariable{ std::move(ln.substr(0ull, eqPos)), std::move([&ln, &eqPos]() -> VariantVariable::VariantT {
				if (const auto substr_val{ ln.substr(eqPos) }; str::pos_valid(substr_val.find(';'))) {
					std::vector<std::string> vec;
					vec.reserve(std::count(substr_val.begin(), substr_val.end(), ';') + 1ull);
					std::stringstream buffer{ substr_val + ';' };
					for (std::string val{}; std::getline(buffer, val, ';'); vec.emplace_back(val)) {}
					vec.shrink_to_fit();
					return{ vec };
				}
				else
					return{ substr_val };
				}()) }));
		}

		return vec;
	}

	struct Env {
		EnvContainer _vars;
		Env(char** envp) : _vars{ std::move(parse_envp(envp)) } {}

		[[nodiscard]] EnvContainer::const_iterator find(std::string var_name, const bool case_sensitive = false) const
		{
			if (!case_sensitive) // convert var name to lowercase if case sensitivity is off
				var_name = str::tolower(var_name);
			for (auto it{ _vars.begin() }; it != _vars.end(); ++it) // iterate through all vars
				if (const auto name{ it->name() }; (case_sensitive && name == var_name) || (!case_sensitive && str::tolower(name) == var_name))
					return it;
			return _vars.end();
		}

		[[nodiscard]] bool exists(const std::string& var_name, const bool case_sensitive = false) const
		{
			return find(var_name, case_sensitive) != _vars.end();
		}

		[[nodiscard]] std::optional<VariantVariable> get(const std::string& var_name, const bool case_sensitive = false) const
		{
			if (const auto target{ find(var_name, case_sensitive) }; target != _vars.end())
				return *target;
			return std::nullopt;
		}

		[[nodiscard]] std::vector<std::string> PATH() const
		{
			if (const auto path_var{ find("PATH", false) }; path_var != _vars.end() && path_var->is_array())
				return path_var->value_array().value();
			throw std::exception("Failed to find PATH environment variable!");
		}

		[[nodiscard]] std::string HOME() const
		{
			if (const auto home_var{ find("HOME", false) }; home_var != _vars.end() && home_var->is_string())
				return home_var->value_string().value();
			throw std::exception("Failed to find HOME environment variable!");
		}
	};

	struct Environment {
		using cont_env = std::unordered_map<std::string, std::string>;
		cont_env _var;

		static cont_env parse(char* envp[])
		{
			cont_env vec;
			for (auto i{ 0u }; envp[i] != nullptr; ++i) {
				const auto str{ str::strip_line(envp[i]) };
				const auto dPos{ str.find('=') };
				if (dPos != std::string::npos)
					vec[str::strip_line(str.substr(0, dPos), "")] = str::strip_line(str.substr(dPos + 1), "");
				else
					throw std::exception("Unknown Environment Variable Syntax");
			}
			return vec;
		}

		explicit Environment(char* envp[]) : _var{ parse(envp) } {}

		[[nodiscard]] bool check(const std::string& var_name, const bool case_sensitive = true)
		{
			return std::ranges::any_of(_var, [&var_name, &case_sensitive](const cont_env::value_type
				& pr) { if (case_sensitive) return str::tolower(pr.first) == str::tolower(var_name); return pr.first == var_name; });
		}

		[[nodiscard]] std::string getv(const std::string& var_name, const bool case_sensitive = false)
		{
			if (case_sensitive)
				return _var.at(var_name);
			const auto res{ std::ranges::find_if(_var, [&var_name](const cont_env::value_type& pr) { return str::tolower(pr.first) == str::tolower(var_name); }) };
			if (res != _var.end())
				return res->second;
			return{};
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
			const auto push{ [&vec](const std::string& entry) { if (!entry.empty()) vec.push_back(entry); } };
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