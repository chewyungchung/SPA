#include "stdafx.h"
#include <iterator>
#include <algorithm>
#include <string>
#include <vector>
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

		TEST_METHOD(testModifiesTable)
		{
			Parser p("SIMPLE_test_3.txt");
			p.process();
			/*
				procedure XYZ
				{
			1		x=1;
			2		x=z;
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
			10				x = y
			11				mango = durian;
						}
			12			while z
						{
			13				papaya = watermelon;
						}
					}
				}
			*/

			list<string> expectedList;
			vector<string> expectedVec;

			/* Correct Modifies relationships (assign statements) */
			// Modifies(1, "x") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "x" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getModifiedBy(1), expectedList));

			// Modifies(4, "apple") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "apple" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getModifiedBy(4), expectedList));

			// Modifies(11, "mango") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "mango" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getModifiedBy(11), expectedList));

			/* Correct Modifies relationships (while statements) */
			// Modifies(3, v) for v = apple, banana -- TRUE
			clearVector(expectedVec);
			expectedVec = { "apple", "banana" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getModifiedBy(3), expectedList));

			// Modifies(6, v) for v = s, r, x, mango, papapya -- TRUE
			clearVector(expectedVec);
			expectedVec = { "s", "r", "x", "mango", "papaya" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getModifiedBy(6), expectedList));

			// Modifies(8, v) for v = s, r, x, mango, papapya -- TRUE
			clearVector(expectedVec);
			expectedVec = { "r", "x", "mango" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getModifiedBy(8), expectedList));
		}

		TEST_METHOD(testUsesTable)
		{
			Parser p("SIMPLE_test_3.txt");
			p.process();
			/*
				procedure XYZ
				{
			1		x=1;
			2		x=z;
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
			10				x = y
			11				mango = durian;
						}
			12			while z
						{
			13				papaya = watermelon;
						}
					}
				}
			*/

			list<string> expectedList;
			vector<string> expectedVec;

			/* Correct Uses relationships (assign statements) */
			// Uses(2, "z") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "z" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getUsedBy(2), expectedList));

			// Uses(4, "orange") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "orange" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getUsedBy(4), expectedList));

			/* Correct Uses relationships (while statements) */
			// Uses(3, v) for v = i, orange, pear -- TRUE
			clearVector(expectedVec);
			expectedVec = { "i", "orange", "pear" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getUsedBy(3), expectedList));

			// Uses(6, v) for v = i, orange, pear -- TRUE
			clearVector(expectedVec);
			expectedVec = { "i", "orange", "pear" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getUsedBy(3), expectedList));

			// Uses(6, v) for v = x, t, y, durian, z, watermelon -- TRUE
			clearVector(expectedVec);
			expectedVec = { "x", "t", "y", "durian", "z", "watermelon" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getUsedBy(6), expectedList));
		}

		TEST_METHOD(testConstantTable)
		{
			Parser p("SIMPLE_test_3.txt");
			p.process();
			/*
				procedure XYZ
				{
			1		x=1;
			2		x=z;
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
			10				x = y
			11				mango = durian;
						}
			12			while z
						{
			13				papaya = watermelon;
						}
					}
				}
			*/

			list<int> expectedList;
			vector<int> expectedVec;
			/* Correct Uses relationships (constant not in containers) */
			// Uses(1, "1") -- TRUE
			clearVector(expectedVec);
			expectedVec = { 1 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getStmtlineByConstant(1), expectedList));

			/* Correct Uses relationships (constant in containers) */
			// Uses(6, "2"), Uses(8, "2"), Uses(9, "2") -- TRUE
			clearVector(expectedVec);
			expectedVec = { 6, 8, 9 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getStmtlineByConstant(2), expectedList));
		}

		TEST_METHOD(testStatementTable)
		{
			Parser p("SIMPLE_test_3.txt");
			p.process();
			/*
				procedure XYZ
				{
			1		x=1;
			2		x=z;
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
			10				x = y
			11				mango = durian;
						}
			12			while z
						{
			13				papaya = watermelon;
						}
					}
				}
			*/

			list<int> expectedList;
			vector<int> expectedVec;
			/* Select all assign stmts */
			// assign a; Select a -- 1, 2, 4, 5, 7, 9, 10, 11, 13
			clearVector(expectedVec);
			expectedVec = { 1, 2, 4, 5, 7, 9, 10, 11, 13 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getAssignList(), expectedList));

			/* Select all while stmts */
			// while w; Select w -- 3, 6, 8, 12
			expectedVec = { 3, 6, 8, 12 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB.getWhileList(), expectedList));
		}

	private:

		bool listCmpHelper(list<string> list1, list<string> list2)
		{
			if (list1.size() != list2.size())
			{
				return false;
			}
			else
			{
				list<string>::iterator it = list1.begin();
				for (; it != list1.end(); ++it)
				{
					if (find(list2.begin(), list2.end(), *it) == list2.end())
					{
						return false;
					}
				}
				return true;
			}
		}

		bool listCmpHelper(list<int> list1, list<int> list2)
		{
			if (list1.size() != list2.size())
			{
				return false;
			}
			else
			{
				list<int>::iterator it = list1.begin();
				for (; it != list1.end(); ++it)
				{
					if (find(list2.begin(), list2.end(), *it) == list2.end())
					{
						return false;
					}
				}
				return true;
			}
		}

		// Swap with empty vector to free memory
		void clearVector(vector<string> v)
		{
			vector<string>().swap(v);
		}

		void clearVector(vector<int> v)
		{
			vector<int>().swap(v);
		}
				
		void vecToListHelper(vector<string> v, list<string> & s)
		{
			s.clear();
			copy(v.begin(), v.end(), back_inserter(s));
		}

		void vecToListHelper(vector<int> v, list<int> & s)
		{
			s.clear();
			copy(v.begin(), v.end(), back_inserter(s));
		}

	};
}