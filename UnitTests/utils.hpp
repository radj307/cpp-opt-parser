#pragma once
#include <Params.hpp>
#include <ParamsAPI.hpp>
#include "pch.h"
namespace utils {
	inline std::streambuf* swap_stream(std::ostream& os, std::streambuf* newBuffer)
	{
		auto copy{ os.rdbuf() };
		os.set_rdbuf(newBuffer);
		return copy;
	}

	// Do not change ordering!
	inline static const std::vector<std::string> default_commandline{
		"-hvac",
		"--test-inner-dash",
		"--help",
		"Hello",
		"World!",
		"6000",
		"-1024",
		"0x00FE",
	};

	template<class RT>
	static std::enable_if_t<( std::is_same_v<RT, opt::Params> || std::is_same_v<RT, opt::ParamsAPI> ), RT> make_args(std::vector<std::string> commandline)
	{
		return RT{ std::move(opt::parseArgs(std::move(commandline))) };
	}

	namespace params {
		inline static const opt::Params _args{ make_args<opt::Params>(default_commandline) };
	}
	namespace paramsapi {
		inline static const opt::ParamsAPI _args{ make_args<opt::ParamsAPI>(default_commandline) };
	}

	using ParamsVariantT = std::variant<std::monostate, opt::Params, opt::ParamsAPI>;

	enum class TypeIndex : size_t {
		NULL_TYPE = 0llu,
		PARAMS = 1llu,
		PARAMSAPI = 2llu,
	};

	inline TypeIndex resolve_type(const ParamsVariantT& instance)
	{
		using enum TypeIndex;
		if ( std::holds_alternative<opt::Params>(instance) )
			return PARAMS;
		else if ( std::holds_alternative<opt::ParamsAPI>(instance) )
			return PARAMSAPI;
		return NULL_TYPE;
	}

	template<class ParamType>
	constexpr bool is_params()
	{
		return std::is_same_v<ParamType, opt::Params>;
	}
	template<class ParamType>
	constexpr bool is_paramsapi()
	{
		return std::is_same_v<ParamType, opt::ParamsAPI>;
	}
}