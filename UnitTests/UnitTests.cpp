#include "pch.h"
#include "CppUnitTest.h"
#include "tests.hpp"

namespace UnitTests
{
	TEST_CLASS(UnitTests)
	{
	public:

		TEST_METHOD(Test_Params_Function_Check)
		{
			Assert::AreEqual(0, tests::test_check(utils::params::_args));
		}
		TEST_METHOD(Test_Params_Function_Find)
		{
			Assert::AreEqual(0, tests::test_find(utils::params::_args));
		}
		TEST_METHOD(Test_ParamsAPI_Function_Check)
		{
			Assert::AreEqual(0, tests::test_check(utils::paramsapi::_args));
		}
		TEST_METHOD(Test_ParamsAPI_Function_Find)
		{
			Assert::AreEqual(0, tests::test_find(utils::paramsapi::_args));
		}
		TEST_METHOD(Test_Compare_Outputs)
		{
			Assert::AreEqual(0, tests::test_compare_output(utils::params::_args, utils::paramsapi::_args));
		}
	};
}
