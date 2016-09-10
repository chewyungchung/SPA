#include "stdafx.h"
#include <string>
#include <list>
#include "CppUnitTest.h"
#include "Tokenizer.h"
#include "Parser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTesting
{
	TEST_CLASS(TestParser)
	{
	public:

		TEST_METHOD(testTokenizer)
		{
			Tokenizer tk = Tokenizer("SIMPLE_test_1.txt");

			list<string> expectedList;
			expectedList.push_back("procedure");
			expectedList.push_back("ABC");
			expectedList.push_back("{");
			expectedList.push_back("x");
			expectedList.push_back("=");
			expectedList.push_back("1");
			expectedList.push_back(";");
			expectedList.push_back("b");
			expectedList.push_back("=");
			expectedList.push_back("2");
			expectedList.push_back(";");
			expectedList.push_back("while");
			expectedList.push_back("i");
			expectedList.push_back("{");
			expectedList.push_back("apple");
			expectedList.push_back("=");
			expectedList.push_back("orange");
			expectedList.push_back(";");
			expectedList.push_back("}");
			expectedList.push_back("}");

			list<string>::iterator it = expectedList.begin();

			for (; it != expectedList.end(); ) {
				Assert::IsTrue(it == tk.getNextToken());
			}
		}

		TEST_METHOD(testFollows)
		{
			Parser p = Parser("SIMPLE_test_2.txt");
			p.process();

			const string NO_STMT = "-1";

			// Follows for first statement in each nesting level
			Assert::AreEqual(PKB.getFollowedFrom(1), NO_STMT);
			Assert::AreEqual(PKB.getFollowedFrom(4), NO_STMT);
			Assert::AreEqual(PKB.getFollowedFrom(7), NO_STMT);
			Assert::AreEqual(PKB.getFollowedFrom(9), NO_STMT);
			Assert::AreEqual(PKB.getFollowedFrom(12), NO_STMT);

			// Follows for last statement in each nesting level
			Assert::AreEqual(PKB.getFollower(6), NO_STMT);
			Assert::AreEqual(PKB.getFollower(5), NO_STMT);
			Assert::AreEqual(PKB.getFollower(), NO_STMT);
			Assert::AreEqual(PKB.getFollower(6), NO_STMT);
			Assert::AreEqual(PKB.getFollower(6), NO_STMT);

			// All Follows relationships
			Assert::AreEqual(PKB.getFollower(1), 2);
			Assert::AreEqual(PKB.getFollower(2), 3);
			Assert::AreEqual(PKB.getFollower(3), 6);
			Assert::AreEqual(PKB.getFollower(1), 2);
			Assert::AreEqual(PKB.getFollower(1), 2);
			Assert::AreEqual(PKB.getFollower(1), 2);
			Assert::AreEqual(PKB.getFollower(1), 2);
			Assert::AreEqual(PKB.getFollower(1), 2);
			Assert::AreEqual(PKB.getFollower(1), 2);
			Assert::AreEqual(PKB.getFollower(1), 2);




		}

	};
}