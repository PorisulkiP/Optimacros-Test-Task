#include "CppUnitTest.h"
#include "..\mtfind.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TESTmtfind
{
	TEST_CLASS(TESTmtfind)
	{
	public:

		TEST_METHOD(TestMethod1)
		{
			std::string data("„p„|„|„€");
			std::string mask("?„|„€");
			std::string tmpAns = comparisonWords(data, mask);
			Assert::AreEqual(tmpAns, data);
		}

		TEST_METHOD(TestMethod2)
		{
			std::string data("„p„|„|„€");
			std::string mask("?„€„|");
			std::string tmpAns = comparisonWords(data, mask);
			Assert::AreNotEqual(tmpAns, data);
		}
	};
}
