#include "stdafx.h"
#include <string>
#include <list>
#include "CppUnitTest.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTesting
{
	TEST_CLASS(TestParser)
	{
	public:

		TEST_METHOD(testTokenizer)
		{
			Tokenizer tk("SIMPLE_test_1.txt");
			/*
			1	procedure ABC
				{
			3		x=1;
			4		b=2;
			5		while i 
					{
			6			apple = orange;
					}
				}
			*/
			
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
				Assert::AreEqual(*it, tk.getNextToken());
			}
		}

		TEST_METHOD(testFollowsTable)
		{
			Parser p("SIMPLE_test_2.txt");
			p.process();
			/*
				procedure ABC
				{
			1		x = 1;
			2		b = 2;
			3		while i
					{
			4			apple = orange;
			5			banana = pear;
					}
			6		while x
					{
			7			s = t;
			8			while y
						{
			9				r = 2;
			10				mango = durian;
						}
			11			while z
						{
			12				papaya = watermelon;
						}
					}
				}
			*/
			

			const string NO_STMT = "-1";
			const string FALSE = "-1";

			// Follows(_, x), where x is first statement in each nesting level
			// Should return NO_STMT
			Assert::AreEqual(PKB.getFollowedFrom(1), NO_STMT);
			Assert::AreEqual(PKB.getFollowedFrom(4), NO_STMT);
			Assert::AreEqual(PKB.getFollowedFrom(7), NO_STMT);
			Assert::AreEqual(PKB.getFollowedFrom(9), NO_STMT);
			Assert::AreEqual(PKB.getFollowedFrom(12), NO_STMT);

			// Follows(x, _) where x is last statement in each nesting level
			// Should return NO_STMT
			Assert::AreEqual(PKB.getFollower(6), NO_STMT);
			Assert::AreEqual(PKB.getFollower(5), NO_STMT);
			Assert::AreEqual(PKB.getFollower(11), NO_STMT);
			Assert::AreEqual(PKB.getFollower(10), NO_STMT);
			Assert::AreEqual(PKB.getFollower(12), NO_STMT);

			// Correct Follows relationships
			Assert::AreEqual(PKB.getFollower(1), 2);
			Assert::AreEqual(PKB.getFollower(2), 3);
			Assert::AreEqual(PKB.getFollower(3), 6);
			Assert::AreEqual(PKB.getFollower(4), 5);
			Assert::AreEqual(PKB.getFollower(8), 11);
			Assert::AreEqual(PKB.getFollower(9), 10);

			// Incorrect Follows relationships (same nesting level, not in sequence)
			Assert::AreNotEqual(PKB.getFollower(1), 3);
			Assert::AreNotEqual(PKB.getFollower(7), 11);

			// Incorrect Follows relationships (same nesting level, wrong order)
			Assert::AreNotEqual(PKB.getFollower(2), 1);
			Assert::AreNotEqual(PKB.getFollower(11), 8);

			// Incorrect Follows relationships (different nesting level)
			Assert::AreNotEqual(PKB.getFollower(3), 4);
			Assert::AreNotEqual(PKB.getFollower(5), 7);
		}

		TEST_METHOD(testParentTable)
		{
			Parser p("SIMPLE_test_2.txt");
			p.process();
			/*
				procedure ABC
				{
			1		x = 1;
			2		b = 2;
			3		while i
					{
			4			apple = orange;
			5			banana = pear;
					}
			6		while x
					{
			7			s = t;
			8			while y
						{
			9				r = 2;
			10				mango = durian;
						}
			11			while z
						{
			12				papaya = watermelon;
						}
					}
				}
			*/

			const string NO_PARENT = "-1";
			const string FALSE = "-1";

			// Parent(_, x), where x is not in any container
			// Should return NO_PARENT
			Assert::AreEqual(PKB.getParentOf(1), NO_PARENT);
			Assert::AreEqual(PKB.getParentOf(2), NO_PARENT);
			Assert::AreEqual(PKB.getParentOf(3), NO_PARENT);
			Assert::AreEqual(PKB.getParentOf(6), NO_PARENT);

			// Correct Parent relationships
			Assert::AreEqual(PKB.getParentOf(4), 3);
			Assert::AreEqual(PKB.getParentOf(5), 3);
			Assert::AreEqual(PKB.getParentOf(7), 6);
			Assert::AreEqual(PKB.getParentOf(9), 8);
			Assert::AreEqual(PKB.getParentOf(12), 11);

			// Incorrect Parent relationships (container stmt does not match parent)
			Assert::AreNotEqual(PKB.getParentOf(4), 6);
			Assert::AreNotEqual(PKB.getParentOf(7), 3);
			Assert::AreNotEqual(PKB.getParentOf(12), 8);

			// Incorrect Parent relationships 
			// (not direct parent, i.e. Parent* holds but not Parent)
			Assert::AreNotEqual(PKB.getParentOf(10), 6);
			Assert::AreNotEqual(PKB.getParentOf(12), 6);
		}
	};
}