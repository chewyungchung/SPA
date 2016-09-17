#include "stdafx.h"

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

		// Free PKB and all tables static ptr
	/*	TEST_METHOD_CLEANUP(destroyPKB)
		{
			PKB::destroyInstance();
		}*/

		TEST_METHOD(TestTokenizer1)
		{
			Tokenizer tk("SIMPLE_test_1.txt");
			list<string> expectedList;
			vector<string> expectedVec;
			expectedVec = { "procedure", "ABC", "{",
							"x", "=", "1", ";",
							"b", "=", "2", ";",
							"y", "=", "x", "+", "i", ";",
							"while", "i", "{",
							"while", "z", "{",
							"good", "=", "b2y", ";", "}",
							"apple", "=", "orange", ";", "}", "}" };
			vecToListHelper(expectedVec, expectedList);

			list<string>::iterator it = expectedList.begin();

			for (; it != expectedList.end(); ++it) {
				string token = tk.getNextToken();
				Assert::AreEqual(*it, token);
			}
		}

		TEST_METHOD(TestTokenizer2)
		{
			Tokenizer tk("SIMPLE_test_2.txt");
			list<string> expectedList;
			vector<string> expectedVec;
			expectedVec = {
				"procedure", "ABC", "{", "x", "=", "1", ";", "b", "=", "2", ";",
				"while", "i", "{", "apple", "=", "orange", ";", "banana", "=", "pear",
				";", "}", "while", "x", "{", "s", "=", "t", ";", "while", "y", "{",
				"r", "=", "2", ";", "mango", "=", "durian", ";", "}", "while", "z", "{",
				"papaya", "=", "watermelon", ";", "}", "}", "}" };
			vecToListHelper(expectedVec, expectedList);

			list<string>::iterator it = expectedList.begin();

			for (; it != expectedList.end(); ++it) {
				string token = tk.getNextToken();
				Assert::AreEqual(*it, token);
			}
		}

		TEST_METHOD(TestTokenizer3)
		{
			Tokenizer tk("SIMPLE_test_3.txt");
			list<string> expectedList;
			vector<string> expectedVec;
			expectedVec = {
				"procedure", "XYZ", "{", "x", "=", "1", ";", "x", "=", "z", ";",
				"while", "i", "{", "apple", "=", "orange", ";", "banana", "=", "pear",
				";", "}", "while", "x", "{", "s", "=", "t", ";", "while", "y", "{", "r",
				"=", "2", ";", "x", "=", "y", ";", "mango", "=", "durian", ";", "}",
				"while", "z", "{", "papaya", "=", "watermelon", ";", "}", "}", "}"
			};
			vecToListHelper(expectedVec, expectedList);

			list<string>::iterator it = expectedList.begin();

			for (; it != expectedList.end(); ++it) {
				string token = tk.getNextToken();
				Assert::AreEqual(*it, token);
			}
		}

		TEST_METHOD(TestFollowsTable)
		{
			PKB _pkb;
			// Test if FollowsTable is empty before being populated
			

			Parser p("SIMPLE_test_2.txt");
			_pkb = p.process();
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

			/*
			// Hand-evaluate the nesting level
			_pkb.addFollows(1, 1);
			_pkb.addFollows(2, 1);
			_pkb.addFollows(3, 1);
			_pkb.addFollows(4, 2);
			_pkb.addFollows(5, 2);
			_pkb.addFollows(6, 1);
			_pkb.addFollows(7, 3);
			_pkb.addFollows(8, 3);
			_pkb.addFollows(9, 4);
			_pkb.addFollows(10, 4);
			_pkb.addFollows(11, 3);
			_pkb.addFollows(12, 5);
			*/
			const int NO_STMT = -1;
			list<int> expectedList;
			vector<int> expectedVec;

			 //Follows(_, x), where x is first statement in each nesting level
			 //Should return NO_STMT
			Assert::AreEqual(NO_STMT, _pkb.getFollowedFrom(1));
			Assert::AreEqual(NO_STMT, _pkb.getFollowedFrom(4));
			Assert::AreEqual(NO_STMT, _pkb.getFollowedFrom(7));
			Assert::AreEqual(NO_STMT, _pkb.getFollowedFrom(9));
			Assert::AreEqual(NO_STMT, _pkb.getFollowedFrom(12));

			// Follows(x, _) where x is last statement in each nesting level
			// Should return NO_STMT
			Assert::AreEqual(NO_STMT, _pkb.getFollower(6));
			Assert::AreEqual(NO_STMT, _pkb.getFollower(5));
			Assert::AreEqual(NO_STMT, _pkb.getFollower(11));
			Assert::AreEqual(NO_STMT, _pkb.getFollower(10));
			Assert::AreEqual(NO_STMT, _pkb.getFollower(12));

			// Correct Follows relationships
			Assert::AreEqual(2, _pkb.getFollower(1));
			Assert::AreEqual(3, _pkb.getFollower(2));
			Assert::AreEqual(6, _pkb.getFollower(3));
			Assert::AreEqual(5, _pkb.getFollower(4));
			Assert::AreEqual(11, _pkb.getFollower(8));
			Assert::AreEqual(10, _pkb.getFollower(9));

			// Correct Follows relationship, using isValidFollows API
			Assert::IsTrue(_pkb.isValidFollows(1, 2));
			Assert::IsTrue(_pkb.isValidFollows(2, 3));
			Assert::IsTrue(_pkb.isValidFollows(3, 6));
			Assert::IsTrue(_pkb.isValidFollows(8, 11));

			// Incorrect Follows relationships (same nesting level, not in sequence)
			Assert::AreNotEqual(3, _pkb.getFollower(1));
			Assert::AreNotEqual(11, _pkb.getFollower(7));

			// Incorrect Follows relationships (same nesting level, wrong order)
			Assert::AreNotEqual(1, _pkb.getFollower(2));
			Assert::AreNotEqual(8, _pkb.getFollower(11));

			// Incorrect Follows relationships (different nesting level)
			Assert::AreNotEqual(4, _pkb.getFollower(3));
			Assert::AreNotEqual(7, _pkb.getFollower(5));

			// Correct Follows* relationships (Follows(s1, s2))
			Assert::IsTrue(_pkb.isFollowsStar(1, 2));
			Assert::IsTrue(_pkb.isFollowsStar(3, 6));
			Assert::IsTrue(_pkb.isFollowsStar(8, 11));

			// Correct Follows* relationships (statements are not consecutive)
			Assert::IsTrue(_pkb.isFollowsStar(1, 6));
			Assert::IsTrue(_pkb.isFollowsStar(7, 11));

			// Incorrect Follows* relationships (statements wrong order)
			Assert::IsFalse(_pkb.isFollowsStar(6, 1));

			// Incorrect Follows* relationships (different nesting level)
			Assert::IsFalse(_pkb.isFollowsStar(3, 5));
			Assert::IsFalse(_pkb.isFollowsStar(7, 9));

			// Correct Follows*(_, s) relationships
			// Select s such that Follows*(s, 3): 1, 2
			clearVector(expectedVec);
			expectedVec = { 1, 2 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getFollowedFromStar(3), expectedList));

			// Select s such that Follows*(s, 6): 1, 2, 3
			clearVector(expectedVec);
			expectedVec = { 1, 2, 3 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getFollowedFromStar(6), expectedList));

			// Select s such that Follows*(s, 11): 7, 8
			clearVector(expectedVec);
			expectedVec = { 7, 8 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getFollowedFromStar(11), expectedList));

			// Correct Follows*(s, _) relationships
			// Select s such that Follows*(1, s): 2, 3, 6
			clearVector(expectedVec);
			expectedVec = { 2, 3, 6 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getFollowerStar(1), expectedList));

			clearVector(expectedVec);
			expectedVec = { 8, 11 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getFollowerStar(7), expectedList));

		}

		TEST_METHOD(TestParentTable)
		{
			PKB _pkb;
			Parser p("SIMPLE_test_2.txt");
			_pkb = p.process();
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

			const int NO_PARENT = -1;
			const int FALSE = -1;

			// Parent(_, x), where x is not in any container
			// Should return NO_PARENT
			Assert::AreEqual(_pkb.getParentOf(1), NO_PARENT);
			Assert::AreEqual(_pkb.getParentOf(2), NO_PARENT);
			Assert::AreEqual(_pkb.getParentOf(3), NO_PARENT);
			Assert::AreEqual(_pkb.getParentOf(6), NO_PARENT);

			// Correct Parent relationships
			Assert::AreEqual(_pkb.getParentOf(4), 3);
			Assert::AreEqual(_pkb.getParentOf(5), 3);
			Assert::AreEqual(_pkb.getParentOf(7), 6);
			Assert::AreEqual(_pkb.getParentOf(9), 8);
			Assert::AreEqual(_pkb.getParentOf(12), 11);

			// Incorrect Parent relationships (container stmt does not match parent)
			Assert::AreNotEqual(_pkb.getParentOf(4), 6);
			Assert::AreNotEqual(_pkb.getParentOf(7), 3);
			Assert::AreNotEqual(_pkb.getParentOf(12), 8);

			// Incorrect Parent relationships 
			// (not direct parent, i.e. Parent* holds but not Parent)

			Assert::AreNotEqual(_pkb.getParentOf(10), 6);
			Assert::AreNotEqual(_pkb.getParentOf(12), 6);
		}

		TEST_METHOD(TestModifiesTable)
		{
			PKB _pkb;
			Parser p("SIMPLE_test_3.txt");
			_pkb = p.process();
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
			Assert::IsTrue(listCmpHelper(_pkb.getModifiedBy(1), expectedList));

			// Modifies(4, "apple") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "apple" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getModifiedBy(4), expectedList));

			// Modifies(11, "mango") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "mango" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getModifiedBy(11), expectedList));

			/* Correct Modifies relationships (while statements) */
			// Modifies(3, v) for v = apple, banana -- TRUE
			clearVector(expectedVec);
			expectedVec = { "apple", "banana" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getModifiedBy(3), expectedList));

			//Modifies(6, v) for v = s, r, x, mango, papapya -- TRUE
			clearVector(expectedVec);
			expectedVec = { "s", "r", "x", "mango", "papaya" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getModifiedBy(6), expectedList));

			// Modifies(8, v) for v = s, r, x, mango, papapya -- TRUE
			clearVector(expectedVec);
			expectedVec = { "r", "x", "mango" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getModifiedBy(8), expectedList));
		}

		TEST_METHOD(TestUsesTable)
		{
			PKB _pkb;
			Parser p("SIMPLE_test_3.txt");
			_pkb = p.process();
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
			Assert::IsTrue(listCmpHelper(_pkb.getUsedBy(2), expectedList));

			// Uses(4, "orange") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "orange" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getUsedBy(4), expectedList));

			/* Correct Uses relationships (while statements) */
			// Uses(3, v) for v = i, orange, pear -- TRUE
			clearVector(expectedVec);
			expectedVec = { "i", "orange", "pear" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getUsedBy(3), expectedList));

			// Uses(6, v) for v = i, orange, pear -- TRUE
			clearVector(expectedVec);
			expectedVec = { "i", "orange", "pear" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getUsedBy(3), expectedList));

			// Uses(6, v) for v = x, t, y, durian, z, watermelon -- TRUE
			clearVector(expectedVec);
			expectedVec = { "x", "t", "y", "durian", "z", "watermelon" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getUsedBy(6), expectedList));
		}

		TEST_METHOD(TestConstantTable1)
		{
			PKB _pkb;
			Parser p("SIMPLE_test_3.txt");
			_pkb = p.process();
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

		TEST_METHOD(TestConstantTable3)
		{
			Parser p("SIMPLE_test_3.txt");
			p.process();
			list<int> expectedList;
			vector<int> expectedVec;
			/* Correct Uses relationships (constant not in containers) */
			// Uses(1, "1") -- TRUE
			clearVector(expectedVec);
			expectedVec = { 1 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getStmtlineByConstant(1), expectedList));

			/* Correct Uses relationships (constant in containers) */
			// Uses(6, "2"), Uses(8, "2"), Uses(9, "2") -- TRUE
			clearVector(expectedVec);
			expectedVec = { 6,8,9 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getStmtlineByConstant(2), expectedList));
		}

		TEST_METHOD(TestStatementTable)
		{
			PKB _pkb;
			Parser p("SIMPLE_test_3.txt");
			_pkb = p.process();
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
			Assert::IsTrue(listCmpHelper(_pkb.getAssignList(), expectedList));

			/* Select all while stmts */
			// while w; Select w -- 3, 6, 8, 12
			expectedVec = { 3, 6, 8, 12 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(_pkb.getWhileList(), expectedList));
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