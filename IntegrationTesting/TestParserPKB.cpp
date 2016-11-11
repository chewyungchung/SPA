/*
Integration testing for Parser-PKB component interaction
Parser is given source file and populates PKB
The test check if PKB is populated correctly
*/
#include "stdafx.h"
#include <stdexcept>
#include <functional>
#include "CppUnitTest.h"

#include <iterator>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include "CppUnitTest.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "DesignExtractor.h"
#include "PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace IntegrationTesting
{		
	TEST_CLASS(TestParserPKB)
	{
	public:		
		TEST_METHOD(TestParserFollowsTable_IT1)
		{
			PKB pkb_;
			// Test if FollowsTable is empty before being populated

			Parser p("SIMPLE_test_2.txt");
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

		TEST_METHOD(TestParserParentTable_IT1)
		{
			PKB pkb_;
			Parser p("SIMPLE_test_2.txt");
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

		TEST_METHOD(TestParserModifiesTable_IT1)
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
			10				x = y;
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

		TEST_METHOD(TestParserUsesTable_IT1)
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
			10				x = y;
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
			10				x = y;
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
			Assert::IsTrue(listCmpHelper(pkb_.getStmtlineByConstant(1), expectedList));

			/* Correct Uses relationships (constant in containers) */
			// Uses(6, "2"), Uses(8, "2"), Uses(9, "2") -- TRUE
			clearVector(expectedVec);
			expectedVec = { 6,8,9 };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getStmtlineByConstant(2), expectedList));
		}

		TEST_METHOD(TestParserStatementTable_IT1)
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
			10				x = y;
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

		/* Our main objective is to Follows across if/else and multiple procedures and with call stmts */
		TEST_METHOD(TestParserFollowsTable_IT2)
		{
			PKB pkb_;
			Parser p("SIMPLE_test_5.txt");
			pkb_ = p.Process();

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

		/* Our main objective is to test Parent across if/else and multiple procedures and with call stmts */
		TEST_METHOD(TestParserParentTable_IT2)
		{
			PKB pkb_;
			Parser p("SIMPLE_test_5.txt");
			pkb_ = p.Process();

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

		/* We test the calls functionality */
		TEST_METHOD(TestParserCallsTable_IT2)
		{
			PKB pkb_;
			Parser p("SIMPLE_test_6.txt");
			pkb_ = p.Process();
		/* 
			procedure Adam {
				call Beanstalk;
				call Cat;
				call Cat;
				call France;
				call Germany;}

			procedure Beanstalk {
				call Diamond; }

			procedure Cat {
				call Diamond; }

			procedure Diamond {
				call Iowa;
				call Germany; }

			procedure Edmonds {
				x = 1; }
	
			procedure France {
				call Edmonds; }

			procedure Germany {
				call Houston; }

			procedure Houston {
				x = 1; }
	
			procedure Iowa {
				x = 1; }

			procedure Jordan {
				call Kansai;
				call Llama; }

			procedure Kansai {
				k = 2; }

			procedure Llama {
				l = l; }

			procedure Michigan {
				x = 1; }
		*/
			list<string> expectedList;
			vector<string> expectedVec;

			// Test isCallExist()
			Assert::IsTrue(pkb_.isCallExist());

			// Test isCall
			Assert::IsTrue(pkb_.isCall("Adam", "Beanstalk"));
			Assert::IsTrue(pkb_.isCall("Adam", "Germany"));
			Assert::IsTrue(pkb_.isCall("Jordan", "Llama"));

			// Test isCall fails for indirect calls
			Assert::IsFalse(pkb_.isCall("Adam", "Diamond"));

			// Test isCall fails correctly
			Assert::IsFalse(pkb_.isCall("Jordan", "Michigan"));

			// Test isCallStar
			Assert::IsTrue(pkb_.isCallStar("Adam", "Diamond"));

			// Test getCalleeStar
			clearVector(expectedVec);
			expectedVec = { "Beanstalk", "Cat", "France", "Diamond", "Edmonds", "Iowa", "Germany", "Houston" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getCalleeStar("Adam"), expectedList));

			// Test getCallerStar
			clearVector(expectedVec);
			expectedVec = { "Adam", "Beanstalk", "Cat", "Diamond" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.getCallerStar("Iowa"), expectedList));
		}

		/* We test whether we can successfully detect cycles in the call graph */
		TEST_METHOD(TestParserCallsGraphCycleCheck_IT2)
		{
			PKB pkb_;
			Parser p("SIMPLE_test_7.txt");
		/*
			procedure A {
				call B; }

			procedure B {
				call D;
				call C; }

			procedure C {
				call F;
				call G; }

			procedure D {
				call E; }
	
			procedure E {
				call I; }
	
			procedure F {
				call G; }
	
			procedure G {
				call H: }
	
			procedure H {
				call D; }
	
			procedure I {
				call H; }
		*/
			// Observe the cycle H -> D -> E -> I -> H
			Assert::ExpectException<invalid_argument>([&]
			{
				p.Process();
			});			
		}

		/* Primarily to test if the Mod/Uses relationships are propagated up correctly */
		TEST_METHOD(TestParserProcTable_IT2)
		{
			PKB pkb_;
			Parser p("SIMPLE_test_8.txt");
			pkb_ = p.Process();
		/*
			procedure Adam {
		1		call Beanstalk;
		2		call Cat;
		3		call Cat;
		4		if a then {
		5			apple = armor + arms; }
				else {
		6			arrow = arsenal; }
		7		call France;
		8		call Germany;}

			procedure Beanstalk {
		9		call Diamond; }

			procedure Cat {
		10		while crystal {
		11			condiments = cashew;
		12			camper = 1; }
		13		call Diamond; }

			procedure Diamond {
		14		call Iowa;
		15		call Germany; }

			procedure Edmonds {
		16		e = 1; }
	
			procedure France {
		17		fructose = futu;
		18		call Edmonds; }

			procedure Germany {
		19		call Houston; }

			procedure Houston {
		20		while h {
		21			haveproblem = hatter; } }
	
			procedure Iowa {
		22		incense = indigo; }
	
			procedure Jordan {
		23		call Kansai;
		24		call Llama; }

			procedure Kansai {
		25		k = 2; }
	
			procedure Llama {
		26		lipton = l; }

			procedure Michigan {
		27		mint = musk; }
		*/
			list<string> expectedList;
			vector<string> expectedVec;
			list<int> i_expectedList;
			vector<int> i_expectedVec;

			// Test isProcedureExist
			Assert::IsTrue(pkb_.IsProcedureExist("Kansai"));
			Assert::IsFalse(pkb_.IsProcedureExist("Nano"));

			// Test mod for variables directly within a proc
			Assert::IsTrue(pkb_.IsModifiedByProc("Adam", "apple"));
			Assert::IsTrue(pkb_.IsModifiedByProc("Houston", "haveproblem"));

			// Test uses for variables directly within a proc
			Assert::IsTrue(pkb_.IsUsedByProc("Adam", "a"));
			Assert::IsTrue(pkb_.IsUsedByProc("Houston", "hatter"));

			// Test propagation for modifying
			clearVector(expectedVec);
			expectedVec = { "Adam", "Beanstalk", "Cat", "Diamond", "Germany", "Houston" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetProcedureModifying("haveproblem"), expectedList));

			clearVector(expectedVec);
			expectedVec = { "apple", "arrow", "condiments", "camper", "fructose", "haveproblem", "incense" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetModifiedByProc("Adam"), expectedList));

			// Test propagation for using
			clearVector(expectedVec);
			expectedVec = { "Adam", "Beanstalk", "Cat", "Diamond", "Germany", "Houston" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetProcedureUsing("hatter"), expectedList));

			clearVector(expectedVec);
			expectedVec = { "indigo", "h", "hatter" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetUsedByProc("Diamond"), expectedList));

			// Test getCallByProcName
			clearVector(i_expectedVec);
			i_expectedVec = { 9, 13 };
			vecToListHelper(i_expectedVec, i_expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetCallByProcName("Diamond"), i_expectedList));

			// Test getCalledProcNamesList()
			clearVector(expectedVec);
			expectedVec = { "Beanstalk", "Cat", "France", "Edmonds", "Diamond", "Iowa", "Houston", "Germany", "Kansai", "Llama" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetCalledProcNamesList(), expectedList));

			// Test getProcedureList()
			clearVector(expectedVec);
			expectedVec = { "Adam", "Beanstalk", "Cat", "France", "Edmonds", "Diamond", "Iowa", "Houston", "Germany", "Jordan", "Kansai", "Llama", "Michigan" };
			vecToListHelper(expectedVec, expectedList);
			Assert::IsTrue(listCmpHelper(pkb_.GetProcedureList(), expectedList));
		}

	public:

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