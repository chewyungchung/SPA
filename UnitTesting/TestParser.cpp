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
		TEST_METHOD_CLEANUP(destroyPKB)
		{
			PKB::destroyInstance();
		}

		TEST_METHOD(TestTokenizer1)
		{
			Tokenizer tk("SIMPLE_test_1.txt");
			/*
				procedure ABC
				{
			1.		x=1;
			2.		b =   2;
			3.		y = x + i;
			4.		while i 
					{
			5.			while   z
						{
			6.				good = b2y;
						}
			7.			apple = orange;
					}
				}
			*/
			
			list<string> expectedList;
			vector<string> expectedVec;
			expectedVec = { "procedure", "ABC", "{",
							"x", "=", "1", ";",
							"b", "=", "2", ";",
							"y", "=", "x", "+", "i", ";",
							"while", "i", "{",
							"while", "z", "{",
							"good", "=", "b2y", ";", "}",
							"apple", "=", "orange", ";", "}", "}"};
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
			// Test if FollowsTable is empty before being populated
			Assert::IsTrue(PKB::getPKB()->isFollowEmpty());

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

			/*
			// Hand-evaluate the nesting level
			PKB::getPKB()->addFollows(1, 1);
			PKB::getPKB()->addFollows(2, 1);
			PKB::getPKB()->addFollows(3, 1);
			PKB::getPKB()->addFollows(4, 2);
			PKB::getPKB()->addFollows(5, 2);
			PKB::getPKB()->addFollows(6, 1);
			PKB::getPKB()->addFollows(7, 3);
			PKB::getPKB()->addFollows(8, 3);
			PKB::getPKB()->addFollows(9, 4);
			PKB::getPKB()->addFollows(10, 4);
			PKB::getPKB()->addFollows(11, 3);
			PKB::getPKB()->addFollows(12, 5);
			*/
			const int NO_STMT = -1;
			list<int> expectedList;
			vector<int> expectedVec;

			 //Follows(_, x), where x is first statement in each nesting level
			 //Should return NO_STMT
			Assert::AreEqual(NO_STMT, PKB::getPKB()->getFollowedFrom(1));
			Assert::AreEqual(NO_STMT, PKB::getPKB()->getFollowedFrom(4));
			Assert::AreEqual(NO_STMT, PKB::getPKB()->getFollowedFrom(7));
			Assert::AreEqual(NO_STMT, PKB::getPKB()->getFollowedFrom(9));
			Assert::AreEqual(NO_STMT, PKB::getPKB()->getFollowedFrom(12));
			
			// Follows(x, _) where x is last statement in each nesting level
			// Should return NO_STMT
			Assert::AreEqual(NO_STMT, PKB::getPKB()->getFollower(6));
			Assert::AreEqual(NO_STMT, PKB::getPKB()->getFollower(5));
			Assert::AreEqual(NO_STMT, PKB::getPKB()->getFollower(11));
			Assert::AreEqual(NO_STMT, PKB::getPKB()->getFollower(10));
			Assert::AreEqual(NO_STMT, PKB::getPKB()->getFollower(12));

			// Correct Follows relationships
			Assert::AreEqual(2, PKB::getPKB()->getFollower(1));
			Assert::AreEqual(3, PKB::getPKB()->getFollower(2));
			Assert::AreEqual(6, PKB::getPKB()->getFollower(3));
			Assert::AreEqual(5, PKB::getPKB()->getFollower(4));
			Assert::AreEqual(11, PKB::getPKB()->getFollower(8));
			Assert::AreEqual(10, PKB::getPKB()->getFollower(9));

			// Correct Follows relationship, using isValidFollows API
			Assert::IsTrue(PKB::getPKB()->isValidFollows(1, 2));
			Assert::IsTrue(PKB::getPKB()->isValidFollows(2, 3));
			Assert::IsTrue(PKB::getPKB()->isValidFollows(3, 6));
			Assert::IsTrue(PKB::getPKB()->isValidFollows(8, 11));

			// Incorrect Follows relationships (same nesting level, not in sequence)
			Assert::AreNotEqual(3, PKB::getPKB()->getFollower(1));
			Assert::AreNotEqual(11, PKB::getPKB()->getFollower(7));

			// Incorrect Follows relationships (same nesting level, wrong order)
			Assert::AreNotEqual(1, PKB::getPKB()->getFollower(2));
			Assert::AreNotEqual(8, PKB::getPKB()->getFollower(11));

			// Incorrect Follows relationships (different nesting level)
			Assert::AreNotEqual(4, PKB::getPKB()->getFollower(3));
			Assert::AreNotEqual(7, PKB::getPKB()->getFollower(5));

			// Correct Follows* relationships (Follows(s1, s2))
			Assert::IsTrue(PKB::getPKB()->isFollowsStar(1, 2));
			Assert::IsTrue(PKB::getPKB()->isFollowsStar(3, 6));
			Assert::IsTrue(PKB::getPKB()->isFollowsStar(8, 11));

			// Correct Follows* relationships (statements are not consecutive)
			Assert::IsTrue(PKB::getPKB()->isFollowsStar(1, 6));
			Assert::IsTrue(PKB::getPKB()->isFollowsStar(7, 11));

			// Incorrect Follows* relationships (statements wrong order)
			Assert::IsFalse(PKB::getPKB()->isFollowsStar(6, 1));

			// Incorrect Follows* relationships (different nesting level)
			Assert::IsFalse(PKB::getPKB()->isFollowsStar(3, 5));
			Assert::IsFalse(PKB::getPKB()->isFollowsStar(7, 9));

			// Correct Follows*(_, s) relationships
			// Select s such that Follows*(s, 3): 1, 2
			clearVector(expectedVec);
			expectedVec = { 1, 2 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getFollowedFromStar(3), expectedList));

			// Select s such that Follows*(s, 6): 1, 2, 3
			clearVector(expectedVec);
			expectedVec = { 1, 2, 3 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getFollowedFromStar(6), expectedList));

			// Select s such that Follows*(s, 11): 7, 8
			clearVector(expectedVec);
			expectedVec = { 7, 8 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getFollowedFromStar(11), expectedList));

			// Correct Follows*(s, _) relationships
			// Select s such that Follows*(1, s): 2, 3, 6
			clearVector(expectedVec);
			expectedVec = { 2, 3, 6 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getFollowerStar(1), expectedList));

			clearVector(expectedVec);
			expectedVec = { 8, 11 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getFollowerStar(7), expectedList));

		}

		TEST_METHOD(TestParentTable)
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

			const int NO_PARENT = -1;
			const int FALSE = -1;

			// Parent(_, x), where x is not in any container
			// Should return NO_PARENT
			Assert::AreEqual(PKB::getPKB()->getParentOf(1), NO_PARENT);
			Assert::AreEqual(PKB::getPKB()->getParentOf(2), NO_PARENT);
			Assert::AreEqual(PKB::getPKB()->getParentOf(3), NO_PARENT);
			Assert::AreEqual(PKB::getPKB()->getParentOf(6), NO_PARENT);

			// Correct Parent relationships
			Assert::AreEqual(PKB::getPKB()->getParentOf(4), 3);
			Assert::AreEqual(PKB::getPKB()->getParentOf(5), 3);
			Assert::AreEqual(PKB::getPKB()->getParentOf(7), 6);
			Assert::AreEqual(PKB::getPKB()->getParentOf(9), 8);
			Assert::AreEqual(PKB::getPKB()->getParentOf(12), 11);

			// Incorrect Parent relationships (container stmt does not match parent)
			Assert::AreNotEqual(PKB::getPKB()->getParentOf(4), 6);
			Assert::AreNotEqual(PKB::getPKB()->getParentOf(7), 3);
			Assert::AreNotEqual(PKB::getPKB()->getParentOf(12), 8);

			// Incorrect Parent relationships 
			// (not direct parent, i.e. Parent* holds but not Parent)
			Assert::AreNotEqual(PKB::getPKB()->getParentOf(10), 6);
			Assert::AreNotEqual(PKB::getPKB()->getParentOf(12), 6);
		}

		TEST_METHOD(TestModifiesTable)
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
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getModifiedBy(1), expectedList));

			// Modifies(4, "apple") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "apple" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getModifiedBy(4), expectedList));

			// Modifies(11, "mango") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "mango" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getModifiedBy(11), expectedList));

			/* Correct Modifies relationships (while statements) */
			// Modifies(3, v) for v = apple, banana -- TRUE
			clearVector(expectedVec);
			expectedVec = { "apple", "banana" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getModifiedBy(3), expectedList));

			//Modifies(6, v) for v = s, r, x, mango, papapya -- TRUE
			clearVector(expectedVec);
			expectedVec = { "s", "r", "x", "mango", "papaya" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getModifiedBy(6), expectedList));

			// Modifies(8, v) for v = s, r, x, mango, papapya -- TRUE
			clearVector(expectedVec);
			expectedVec = { "r", "x", "mango" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getModifiedBy(8), expectedList));
		}

		TEST_METHOD(TestUsesTable)
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
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getUsedBy(2), expectedList));

			// Uses(4, "orange") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "orange" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getUsedBy(4), expectedList));

			/* Correct Uses relationships (while statements) */
			// Uses(3, v) for v = i, orange, pear -- TRUE
			clearVector(expectedVec);
			expectedVec = { "i", "orange", "pear" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getUsedBy(3), expectedList));

			// Uses(6, v) for v = i, orange, pear -- TRUE
			clearVector(expectedVec);
			expectedVec = { "i", "orange", "pear" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getUsedBy(3), expectedList));

			// Uses(6, v) for v = x, t, y, durian, z, watermelon -- TRUE
			clearVector(expectedVec);
			expectedVec = { "x", "t", "y", "durian", "z", "watermelon" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getUsedBy(6), expectedList));
		}

		TEST_METHOD(TestConstantTable)
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
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getStmtlineByConstant(1), expectedList));

			/* Correct Uses relationships (constant in containers) */
			// Uses(6, "2"), Uses(8, "2"), Uses(9, "2") -- TRUE
			clearVector(expectedVec);
			expectedVec = { 6, 8, 9 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getStmtlineByConstant(2), expectedList));
		}

		TEST_METHOD(TestStatementTable)
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
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getAssignList(), expectedList));

			/* Select all while stmts */
			// while w; Select w -- 3, 6, 8, 12
			expectedVec = { 3, 6, 8, 12 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(PKB::getPKB()->getWhileList(), expectedList));
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