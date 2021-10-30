#pragma once
#include "pch.h"
#include "utils.hpp"

namespace tests {
	using namespace utils;

	template<class ParamType>
	int test_check(const ParamType& args)
	{
		static_assert( std::is_same_v<ParamType, opt::Params> || std::is_same_v<ParamType, opt::ParamsAPI> );
		try {
			// Generics
			Assert::IsTrue(args.check('h'));
			Assert::IsTrue(args.check('v'));
			Assert::IsTrue(args.check('a'));
			Assert::IsTrue(args.check('c'));
			Assert::IsTrue(args.check("test-inner-dash"));
			Assert::IsTrue(args.check("help"));
			Assert::IsTrue(args.check("Hello"));
			Assert::IsTrue(args.check("World!"));
			Assert::IsTrue(args.check("6000"));
			Assert::IsTrue(args.check("-1024"));
			Assert::IsTrue(args.check("0x00FE"));
			// Flags
			Assert::IsTrue(args.check_flag('h'));
			Assert::IsTrue(args.check_flag('v'));
			Assert::IsTrue(args.check_flag('a'));
			Assert::IsTrue(args.check_flag('c'));
			// Options
			Assert::IsTrue(args.check_opt("test-inner-dash"));
			Assert::IsTrue(args.check_opt("help"));
			// Parameters
			Assert::IsTrue(args.check_param("Hello"));
			Assert::IsTrue(args.check_param("World!"));
			Assert::IsTrue(args.check_param("6000"));
			Assert::IsTrue(args.check_param("-1024"));
			Assert::IsTrue(args.check_param("0x00FE"));
			// Variadic
			if constexpr ( is_params<ParamType>() ) { // use opt::Params-specific methods
				Assert::IsTrue(args.check_flag('h', 'v', 'a', 'c'));
				Assert::IsTrue(args.check_opt("test-inner-dash", "help"));
			}
			else if constexpr ( is_paramsapi<ParamType>() ) { // use opt::ParamsAPI-specific methods
				Assert::IsTrue(args.check_any<opt::Flag>('h', 'v', 'a', 'c'));
				Assert::IsTrue(args.check_any<opt::Option>("test-inner-dash", "help"));
			}
			Assert::IsTrue(args.check_all('h', 'v', 'a', 'c', "test-inner-dash", "help", "Hello", "World!", "6000", "-1024", "0x00FE"));
			Assert::IsTrue(args.check_all("Hello", "World!", "test-inner-dash"));
			return 0;
		} catch ( ... ) { return -1; }
	}

	template<class ParamType>
	int test_find(const ParamType& args)
	{
		static_assert( std::is_same_v<ParamType, opt::Params> || std::is_same_v<ParamType, opt::ParamsAPI> );
		try {
			Assert::AreNotEqual(args.begin() - args.end(), args.begin() - args.find('h'));
			Assert::AreNotEqual(args.begin() - args.end(), args.begin() - args.find('v'));
			Assert::AreNotEqual(args.begin() - args.end(), args.begin() - args.find('a'));
			Assert::AreNotEqual(args.begin() - args.end(), args.begin() - args.find('c'));
			Assert::AreNotEqual(args.begin() - args.end(), args.begin() - args.find("test-inner-dash"));
			Assert::AreNotEqual(args.begin() - args.end(), args.begin() - args.find("help"));
			Assert::AreNotEqual(args.begin() - args.end(), args.begin() - args.find("Hello"));
			Assert::AreNotEqual(args.begin() - args.end(), args.begin() - args.find("World!"));
			Assert::AreNotEqual(args.begin() - args.end(), args.begin() - args.find("6000"));
			Assert::AreNotEqual(args.begin() - args.end(), args.begin() - args.find("-1024"));
			Assert::AreNotEqual(args.begin() - args.end(), args.begin() - args.find("0x00FE"));
			return 0;
		} catch ( ... ) { return -1; }
	}

	template<class ParamTypeLeft, class ParamTypeRight>
	int test_compare_output(const ParamTypeLeft& left, const ParamTypeRight& right)
	{
		static_assert( std::is_same_v<ParamTypeLeft, opt::Param> || std::is_same_v<ParamTypeLeft, opt::ParamAPI> || std::is_same_v<ParamTypeRight, opt::Param> || std::is_same_v<ParamTypeRight, opt::ParamAPI> );
		try {
			std::stringstream left_buffer, right_buffer;
			left_buffer << left;
			right_buffer << right;
			Assert::AreSame(left_buffer.str(), right_buffer.str());
			return 0;
		} catch ( ... ) { return -1; }
	}
}