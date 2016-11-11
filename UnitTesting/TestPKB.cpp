/*
To unit test the PKB, we manually populate it directly by 'add' methods and test the methods
*/
#include "stdafx.h"

#include "stdafx.h"
#include "CppUnitTest.h"
#include <iterator>
#include "Parser.h"
#include "PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestPKB)
	{
	public:

		TEST_METHOD_INITIALIZE(buildPKB)
		{			
		}

		TEST_METHOD_CLEANUP(destroyPKB)
		{
		}

		TEST_METHOD(TestModUsesbyStmtFunction_IT1)
		{
			PKB pkb_;
			/* addModUsesbyStmt */
			pkb_.AddModifies(1, "a");
			pkb_.AddModifies(2, "b");
			pkb_.AddModifies(2, "c");
			pkb_.AddModifies(2, "z");
			pkb_.AddModifies(3, "a");
			pkb_.AddModifies(4, "xyz");

			pkb_.AddUses(1, "a");
			pkb_.AddUses(1, "i");
			pkb_.AddUses(2, "d");
			pkb_.AddUses(2, "i");
			pkb_.AddUses(5, "e");
			pkb_.AddUses(5, "a");

			list<string> actual, expected;

			actual = pkb_.GetModifiedBy(1);
			expected = { "a" };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetModifiedBy(2);
			expected = { "b", "c", "z" };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetModifiedBy(4);
			expected = { "xyz" };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetUsedBy(1);
			expected = { "a", "i" };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetUsedBy(2);
			expected = { "d", "i" };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetUsedBy(5);
			expected = { "e", "a" };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetUsedBy(10);
			expected = {};
			Assert::IsTrue(actual == expected);

			Assert::IsTrue(pkb_.IsModified(1, "a"));
			Assert::IsTrue(pkb_.IsModified(2, "b"));
			Assert::IsTrue(pkb_.IsModified(2, "c"));
			Assert::IsTrue(pkb_.IsModified(2, "z"));
			Assert::IsTrue(pkb_.IsModified(4, "xyz"));
			/* -1 is not a existent statement statement no(key)*/
			Assert::IsFalse(pkb_.IsModified(-1, "a"));
			/* "xyz" exist, but stmt 2 is not its corresponding key*/
			Assert::IsFalse(pkb_.IsModified(2, "xyz"));
			/* "_xyz_" does not exist in the modTable*/
			Assert::IsFalse(pkb_.IsModified(4, "_xyz_"));

			Assert::IsTrue(pkb_.IsUsed(1, "a"));
			Assert::IsTrue(pkb_.IsUsed(1, "i"));
			Assert::IsTrue(pkb_.IsUsed(2, "d"));
			Assert::IsTrue(pkb_.IsUsed(5, "e"));
			Assert::IsFalse(pkb_.IsUsed(-1, "a"));
			Assert::IsFalse(pkb_.IsUsed(5, "i"));
		}

		TEST_METHOD(TestModUsesbyVarFunction_IT1)
		{
			PKB pkb_;
			/* addModUsesbyVar */
			pkb_.AddModifies("a", 1);
			pkb_.AddModifies("a", 3);
			pkb_.AddModifies("b", 2);
			pkb_.AddModifies("c", 2);
			pkb_.AddModifies("z", 2);
			pkb_.AddModifies("xyz", 4);

			pkb_.AddUses("a", 1);
			pkb_.AddUses("a", 5);
			pkb_.AddUses("i", 1);
			pkb_.AddUses("i", 2);
			pkb_.AddUses("d", 2);
			pkb_.AddUses("e", 5);

			list<int> actual, expected;
			list<string> actualVarList, expectedVarList;

			actual = pkb_.GetModifiedBy("a");
			expected = { 1,3 };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetModifiedBy("b");
			expected = { 2 };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetModifiedBy("c");
			expected = { 2 };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetModifiedBy("z");
			expected = { 2 };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetModifiedBy("xyz");
			expected = { 4 };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetUsedBy("a");
			expected = { 1, 5 };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetUsedBy("i");
			expected = { 1, 2 };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetUsedBy("d");
			expected = { 2 };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetUsedBy("e");
			expected = { 5 };
			Assert::IsTrue(actual == expected);

			actual = pkb_.GetUsedBy("invalid");
			expected = {};
			Assert::IsTrue(actual == expected);

			/* all the variables from the mod and uses table should be valid */
			Assert::IsTrue(pkb_.IsValidVar("a"));
			Assert::IsTrue(pkb_.IsValidVar("b"));
			Assert::IsTrue(pkb_.IsValidVar("c"));
			Assert::IsTrue(pkb_.IsValidVar("z"));
			Assert::IsTrue(pkb_.IsValidVar("xyz"));
			Assert::IsTrue(pkb_.IsValidVar("i"));
			Assert::IsTrue(pkb_.IsValidVar("d"));
			Assert::IsTrue(pkb_.IsValidVar("e"));

			actualVarList = pkb_.GetAllModVar();
			expectedVarList = { "a", "b", "c", "z", "xyz" };
			Assert::IsTrue(actual == expected);

			/* clear the existing modVarList and check if the assertion is
			correct for the case where the list is empty */
			actualVarList = pkb_.GetAllUsedVar();
			expectedVarList = { "a", "i", "d", "e" };
			Assert::IsTrue(actual == expected);

			actualVarList = pkb_.GetVarList();
			expectedVarList = { "a", "b", "c", "z", "xyz", "i", "d", "e" };
			Assert::IsTrue(actual == expected);
		}

		TEST_METHOD(TestFollowsTable_IT1)
		{
			PKB pkb_;

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

			pkb_.addFollows(1, 1);
			pkb_.addFollows(2, 1);
			pkb_.addFollows(3, 1);
			pkb_.addFollows(4, 2);
			pkb_.addFollows(5, 2);
			pkb_.addFollows(6, 1);
			pkb_.addFollows(7, 3);
			pkb_.addFollows(8, 3);
			pkb_.addFollows(9, 4);
			pkb_.addFollows(10, 4);
			pkb_.addFollows(11, 3);
			pkb_.addFollows(12, 5);
			
			const int NO_STMT = -1;
			list<int> expectedList;
			vector<int> expectedVec;

			//Follows(_, x), where x is first statement in each nesting level
			//Should return NO_STMT
			Assert::AreEqual(NO_STMT, pkb_.getFollowedFrom(1));
			Assert::AreEqual(NO_STMT, pkb_.getFollowedFrom(4));
			Assert::AreEqual(NO_STMT, pkb_.getFollowedFrom(7));
			Assert::AreEqual(NO_STMT, pkb_.getFollowedFrom(9));
			Assert::AreEqual(NO_STMT, pkb_.getFollowedFrom(12));

			// Follows(x, _) where x is last statement in each nesting level
			// Should return NO_STMT
			Assert::AreEqual(NO_STMT, pkb_.getFollower(6));
			Assert::AreEqual(NO_STMT, pkb_.getFollower(5));
			Assert::AreEqual(NO_STMT, pkb_.getFollower(11));
			Assert::AreEqual(NO_STMT, pkb_.getFollower(10));
			Assert::AreEqual(NO_STMT, pkb_.getFollower(12));

			// Correct Follows relationships
			Assert::AreEqual(2, pkb_.getFollower(1));
			Assert::AreEqual(3, pkb_.getFollower(2));
			Assert::AreEqual(6, pkb_.getFollower(3));
			Assert::AreEqual(5, pkb_.getFollower(4));
			Assert::AreEqual(11, pkb_.getFollower(8));
			Assert::AreEqual(10, pkb_.getFollower(9));

			// Correct Follows relationship, using isValidFollows API
			Assert::IsTrue(pkb_.isValidFollows(1, 2));
			Assert::IsTrue(pkb_.isValidFollows(2, 3));
			Assert::IsTrue(pkb_.isValidFollows(3, 6));
			Assert::IsTrue(pkb_.isValidFollows(8, 11));

			// Incorrect Follows relationships (same nesting level, not in sequence)
			Assert::AreNotEqual(3, pkb_.getFollower(1));
			Assert::AreNotEqual(11, pkb_.getFollower(7));

			// Incorrect Follows relationships (same nesting level, wrong order)
			Assert::AreNotEqual(1, pkb_.getFollower(2));
			Assert::AreNotEqual(8, pkb_.getFollower(11));

			// Incorrect Follows relationships (different nesting level)
			Assert::AreNotEqual(4, pkb_.getFollower(3));
			Assert::AreNotEqual(7, pkb_.getFollower(5));

			// Correct Follows* relationships (Follows(s1, s2))
			Assert::IsTrue(pkb_.isFollowsStar(1, 2));
			Assert::IsTrue(pkb_.isFollowsStar(3, 6));
			Assert::IsTrue(pkb_.isFollowsStar(8, 11));

			// Correct Follows* relationships (statements are not consecutive)
			Assert::IsTrue(pkb_.isFollowsStar(1, 6));
			Assert::IsTrue(pkb_.isFollowsStar(7, 11));

			// Incorrect Follows* relationships (statements wrong order)
			Assert::IsFalse(pkb_.isFollowsStar(6, 1));

			// Incorrect Follows* relationships (different nesting level)
			Assert::IsFalse(pkb_.isFollowsStar(3, 5));
			Assert::IsFalse(pkb_.isFollowsStar(7, 9));

			// Correct Follows*(_, s) relationships
			// Select s such that Follows*(s, 3): 1, 2
			clearVector(expectedVec);
			expectedVec = { 1, 2 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getFollowedFromStar(3), expectedList));

			// Select s such that Follows*(s, 6): 1, 2, 3
			clearVector(expectedVec);
			expectedVec = { 1, 2, 3 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getFollowedFromStar(6), expectedList));

			// Select s such that Follows*(s, 11): 7, 8
			clearVector(expectedVec);
			expectedVec = { 7, 8 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getFollowedFromStar(11), expectedList));

			// Correct Follows*(s, _) relationships
			// Select s such that Follows*(1, s): 2, 3, 6
			clearVector(expectedVec);
			expectedVec = { 2, 3, 6 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getFollowerStar(1), expectedList));

			clearVector(expectedVec);
			expectedVec = { 8, 11 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getFollowerStar(7), expectedList));
		}

		/* Our main objective is to Follows across if/else and multiple procedures and with call stmts */
		TEST_METHOD(TestFollowsTable_IT2)
		{
			PKB pkb_;
		/*
			procedure A {
		1		if r then {
		2			y = x + 2;
		3			a = b;
		4			if x then {
		5				while z {
		6					z = 2;
		7					call B;}
		8				k = kappa;}
					else {
		9				w = omega;
		10				if r then {
		11					call B;
		12					p = pi;
		13					if v then {
		14						x = 1; 
		15						while a {
		16							while b {
		17								while c {
		18									alpha = beta;
		19									beta = gamma; }}}}
							else {
		20						call meMaybe; }
		21					x = 5;}
						else {
		22					miami = nights; }}}
				else {
		23			e = r;
		24			n = m; }
		25		tony = montana;}

			procedure B {
		26		if x then {
		27			if y then {
		28				if u then {
		29					i = 2;
		30					k = p;}
						else {
		31					i = 1; }}
					else {
		32				hop = in; }}
				else {
		33			b = 5;}}
		*/

			pkb_.addFollows(1, 1);
			pkb_.addFollows(2, 2);
			pkb_.addFollows(3, 2);
			pkb_.addFollows(4, 2);
			pkb_.addFollows(5, 3);
			pkb_.addFollows(6, 4);
			pkb_.addFollows(7, 4);
			pkb_.addFollows(8, 3);
			pkb_.addFollows(9, 5);
			pkb_.addFollows(10, 5);
			pkb_.addFollows(11, 6);
			pkb_.addFollows(12, 6);
			pkb_.addFollows(13, 6);
			pkb_.addFollows(14, 7);
			pkb_.addFollows(15, 7);
			pkb_.addFollows(16, 8);
			pkb_.addFollows(17, 9);
			pkb_.addFollows(18, 10);
			pkb_.addFollows(19, 10);
			pkb_.addFollows(20, 11);
			pkb_.addFollows(21, 6);
			pkb_.addFollows(22, 12);
			pkb_.addFollows(23, 13);
			pkb_.addFollows(24, 13);
			pkb_.addFollows(25, 1);
			pkb_.addFollows(26, 14);
			pkb_.addFollows(27, 15);
			pkb_.addFollows(28, 16);
			pkb_.addFollows(29, 17);
			pkb_.addFollows(30, 17);
			pkb_.addFollows(31, 18);
			pkb_.addFollows(32, 19);
			pkb_.addFollows(33, 20);

			const int NO_STMT = -1;
			list<int> expectedList;
			vector<int> expectedVec;

			// Follows(1, 25), where 25 follows the ifstmt 1
			Assert::AreEqual(1, pkb_.getFollowedFrom(25));
			Assert::AreEqual(25, pkb_.getFollower(1));
			Assert::IsTrue(pkb_.isValidFollows(1, 25));
			Assert::IsTrue(pkb_.isFollowsStar(1, 25));

			// Test Follows does not hold between if/else stmt blocks
			Assert::IsFalse(pkb_.isValidFollows(5, 9));

			// We test getFollowsStar for some stmts nested deeply in ifstmts
			// Observe 11, 12, 13, 21 are all in nestinglevel = 6
			clearVector(expectedVec);
			expectedVec = { 12, 13, 21 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getFollowerStar(11), expectedList));

			clearVector(expectedVec);
			expectedVec = { 11, 12, 13 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getFollowedFromStar(21), expectedList));

			// Test Follows does not hold between two procedures
			Assert::IsFalse(pkb_.isValidFollows(25, 26));

			// Test Follows within second procedure 
			Assert::IsTrue(pkb_.isValidFollows(29, 30));
		}

		TEST_METHOD(TestParentTable_IT1)
		{
			PKB pkb_;

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

			pkb_.AddParent(-1, 1);
			pkb_.AddParent(-1, 2);
			pkb_.AddParent(-1, 3);
			pkb_.AddParent(3, 4);
			pkb_.AddParent(3, 5);
			pkb_.AddParent(-1, 6);
			pkb_.AddParent(6, 7);
			pkb_.AddParent(6, 8);
			pkb_.AddParent(8, 9);
			pkb_.AddParent(8, 10);
			pkb_.AddParent(6, 11);
			pkb_.AddParent(11, 12);

			pkb_.AddParentStar(3, 4);
			pkb_.AddParentStar(3, 5);
			pkb_.AddParentStar(6, 7);
			pkb_.AddParentStar(6, 8);
			pkb_.AddParentStar(8, 9);
			pkb_.AddParentStar(6, 9);
			pkb_.AddParentStar(8, 10);
			pkb_.AddParentStar(6, 10);
			pkb_.AddParentStar(6, 11);
			pkb_.AddParentStar(11, 12);
			pkb_.AddParentStar(6, 12);

			const int NO_PARENT = -1;
			const int FALSE = -1;

			// Parent(_, x), where x is not in any container
			// Should return NO_PARENT
			Assert::AreEqual(pkb_.GetParentOf(1), NO_PARENT);
			Assert::AreEqual(pkb_.GetParentOf(2), NO_PARENT);
			Assert::AreEqual(pkb_.GetParentOf(3), NO_PARENT);
			Assert::AreEqual(pkb_.GetParentOf(6), NO_PARENT);

			// Correct Parent relationships
			Assert::AreEqual(pkb_.GetParentOf(4), 3);
			Assert::AreEqual(pkb_.GetParentOf(5), 3);
			Assert::AreEqual(pkb_.GetParentOf(7), 6);
			Assert::AreEqual(pkb_.GetParentOf(9), 8);
			Assert::AreEqual(pkb_.GetParentOf(12), 11);

			// Incorrect Parent relationships (container stmt does not match parent)
			Assert::AreNotEqual(pkb_.GetParentOf(4), 6);
			Assert::AreNotEqual(pkb_.GetParentOf(7), 3);
			Assert::AreNotEqual(pkb_.GetParentOf(12), 8);

			// Incorrect Parent relationships 
			// (not direct parent, i.e. Parent* holds but not Parent)
			Assert::AreNotEqual(pkb_.GetParentOf(10), 6);
			Assert::AreNotEqual(pkb_.GetParentOf(12), 6);
		}

		/* Our main objective is to Parent across if/else and multiple procedures and with call stmts */
		TEST_METHOD(TestParentTable_IT2)
		{
			PKB pkb_;
		/*
			procedure A {
		1		if r then {
		2			y = x + 2;
		3			a = b;
		4			if x then {
		5				while z {
		6					z = 2;
		7					call B;}
		8				k = kappa;}
					else {
		9				w = omega;
		10				if r then {
		11					call B;
		12					p = pi;
		13					if v then {
		14						x = 1; 
		15						while a {
		16							while b {
		17								while c {
		18									alpha = beta;
		19									beta = gamma; }}}}
							else {
		20						call meMaybe; }
		21					x = 5;}
						else {
		22					miami = nights; }}}
				else {
		23			e = r;
		24			n = m; }
		25		tony = montana;}

			procedure B {
		26		if x then {
		27			if y then {
		28				if u then {
		29					i = 2;
		30					k = p;}
						else {
		31					i = 1; }}
					else {
		32				hop = in; }}
				else {
		33			b = 5;}}
		*/

			pkb_.AddParent(-1, 1);
			pkb_.AddParent(1, 2);
			pkb_.AddParent(1, 3);
			pkb_.AddParent(1, 4);
			pkb_.AddParent(4, 5);
			pkb_.AddParent(5, 6);
			pkb_.AddParent(5, 7);
			pkb_.AddParent(4, 8);
			pkb_.AddParent(4, 9);
			pkb_.AddParent(4, 10);
			pkb_.AddParent(10, 11);
			pkb_.AddParent(10, 12);
			pkb_.AddParent(10, 13);
			pkb_.AddParent(13, 14);
			pkb_.AddParent(13, 15);
			pkb_.AddParent(15, 16);
			pkb_.AddParent(16, 17);
			pkb_.AddParent(17, 18);
			pkb_.AddParent(17, 19);
			pkb_.AddParent(13, 20);
			pkb_.AddParent(10, 21);
			pkb_.AddParent(10, 22);
			pkb_.AddParent(1, 23);
			pkb_.AddParent(1, 24);
			pkb_.AddParent(-1, 25);
			pkb_.AddParent(-1, 26);
			pkb_.AddParent(26, 27);
			pkb_.AddParent(26, 28);
			pkb_.AddParent(28, 29);
			pkb_.AddParent(28, 30);
			pkb_.AddParent(28, 31);
			pkb_.AddParent(27, 32);
			pkb_.AddParent(26, 33);

			/* We will only populate for some */
			pkb_.AddParentStar(13, 14);
			pkb_.AddParentStar(13, 15);
			pkb_.AddParentStar(13, 16);
			pkb_.AddParentStar(13, 17);
			pkb_.AddParentStar(13, 18);
			pkb_.AddParentStar(13, 19);
			pkb_.AddParentStar(13, 20);

			const int NO_PARENT = -1;
			list<int> expectedList;
			vector<int> expectedVec;

			// Parent(1, x), x is in fact 2, 3, 4, 23, 24
			Assert::AreEqual(1, pkb_.GetParentOf(2));
			Assert::AreEqual(1, pkb_.GetParentOf(23));
			Assert::AreEqual(1, pkb_.GetParentOf(24));

			clearVector(expectedVec);
			expectedVec = { 2, 3, 4, 23, 24 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetChildrenOf(1), expectedList));

			// Parent*(13, _)
			clearVector(expectedVec);
			expectedVec = { 14, 15, 16, 17, 18, 19, 20 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetChildStarOf(13), expectedList));

			// Test Parent across different procedures
			Assert::AreNotEqual(pkb_.GetParentOf(26), 25);

			// Test Parent else stmts is its ifstmt
			Assert::AreEqual(28, pkb_.GetParentOf(31));
		}

		TEST_METHOD(TestModifiesTable_IT1)
		{
			PKB pkb_;
			Parser p("SIMPLE_test_3.txt");
			pkb_ = p.Process();

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

			list<string> expectedList;
			vector<string> expectedVec;

			/* Correct Modifies relationships (assign statements) */
			// Modifies(1, "x") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "x" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetModifiedBy(1), expectedList));

			// Modifies(4, "apple") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "apple" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetModifiedBy(4), expectedList));

			// Modifies(11, "mango") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "mango" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetModifiedBy(11), expectedList));

			/* Correct Modifies relationships (while statements) */
			// Modifies(3, v) for v = apple, banana -- TRUE
			clearVector(expectedVec);
			expectedVec = { "apple", "banana" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetModifiedBy(3), expectedList));

			//Modifies(6, v) for v = s, r, x, mango, papapya -- TRUE
			clearVector(expectedVec);
			expectedVec = { "s", "r", "x", "mango", "papaya" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetModifiedBy(6), expectedList));

			// Modifies(8, v) for v = s, r, x, mango, papapya -- TRUE
			clearVector(expectedVec);
			expectedVec = { "r", "x", "mango" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetModifiedBy(8), expectedList));
		}

		TEST_METHOD(TestUsesTable_IT1)
		{
			PKB pkb_;
			Parser p("SIMPLE_test_3.txt");
			pkb_ = p.Process();

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

			list<string> expectedList;
			vector<string> expectedVec;

			/* Correct Uses relationships (assign statements) */
			// Uses(2, "z") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "z" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetUsedBy(2), expectedList));

			// Uses(4, "orange") -- TRUE
			clearVector(expectedVec);
			expectedVec = { "orange" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetUsedBy(4), expectedList));

			/* Correct Uses relationships (while statements) */
			// Uses(3, v) for v = i, orange, pear -- TRUE
			clearVector(expectedVec);
			expectedVec = { "i", "orange", "pear" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetUsedBy(3), expectedList));

			// Uses(6, v) for v = i, orange, pear -- TRUE
			clearVector(expectedVec);
			expectedVec = { "i", "orange", "pear" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetUsedBy(3), expectedList));

			// Uses(6, v) for v = x, t, y, durian, z, watermelon -- TRUE
			clearVector(expectedVec);
			expectedVec = { "x", "t", "y", "durian", "z", "watermelon" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetUsedBy(6), expectedList));
		}

		TEST_METHOD(TestConstantTable_IT1)
		{
			PKB pkb_;
			Parser p("SIMPLE_test_3.txt");
			pkb_ = p.Process();

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

			list<int> expectedList;
			vector<int> expectedVec;
			/* Correct Uses relationships (constant not in containers) */
			// Uses(1, "1") -- TRUE
			clearVector(expectedVec);
			expectedVec = { 1 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getStmtlineByConstant(1), expectedList));

			/* Correct Uses relationships (constant in containers) */
			// Uses(6, "2"), Uses(8, "2"), Uses(9, "2") -- TRUE
			clearVector(expectedVec);
			expectedVec = { 6,8,9 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getStmtlineByConstant(2), expectedList));
		}

		TEST_METHOD(TestStatementTable_IT1)
		{
			PKB pkb_;
			Parser p("SIMPLE_test_3.txt");
			pkb_ = p.Process();
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
			Assert::IsTrue(listCmpHelper(pkb_.GetAssignList(), expectedList));

			/* Select all while stmts */
			// while w; Select w -- 3, 6, 8, 12
			expectedVec = { 3, 6, 8, 12 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetWhileList(), expectedList));
		}

		TEST_METHOD(TestStatementFunction_IT1)
		{
			PKB pkb_;

			pkb_.AddStatement(1, "assign");
			pkb_.AddStatement(2, "assign");
			//_pkb.addStatement(1, "while"); //invalid case
			pkb_.AddStatement(3, "assign");
			pkb_.AddStatement(5, "while");
			pkb_.AddStatement(6, "assign");
			pkb_.AddStatement(7, "invalidtype"); //invalid
			pkb_.AddStatement(8, "assign");
			pkb_.AddStatement(9, "while");
			pkb_.AddStatement(10, "assign");

			list<int> actual, expected1, expected2;
			int actualTotal, expectedTotal;

			actual = pkb_.GetAssignList();
			expected1 = { 1, 2, 3, 6, 8, 10 };
			expected2 = { 1, 2, 3, 6, 7, 8, 10 };
			Assert::IsTrue(actual == expected1);
			/* stmt 7 should not be in the list; stmtType is invalid */
			Assert::IsFalse(actual == expected2);
			
			actual = pkb_.GetWhileList();
			expected1 = {5, 9};
			expected2 = {5, 7, 9};
			Assert::IsTrue(actual == expected1);
			/* stmt 7 should not be in the list; stmtType is invalid */
			Assert::IsFalse(actual == expected2);

			actual = pkb_.GetStmtList();
			expected1 = { 1, 2, 3, 5, 6, 8, 9, 10 };
			Assert::IsTrue(actual == expected1);
			
			actualTotal = pkb_.GetStatementCount();
			expectedTotal = 8;
			Assert::IsTrue(actualTotal == expectedTotal);
		///* check isValidStmt */
		Assert::IsTrue(pkb_.IsValidStmt(1));
		Assert::IsTrue(pkb_.IsValidStmt(2));
		Assert::IsTrue(pkb_.IsValidStmt(3));
		Assert::IsFalse(pkb_.IsValidStmt(4));
		Assert::IsTrue(pkb_.IsValidStmt(5));
		Assert::IsFalse(pkb_.IsValidStmt(11));
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