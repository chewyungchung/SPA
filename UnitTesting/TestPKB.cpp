#include "stdafx.h"
#include "CppUnitTest.h"
#include "PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestPKB)
	{
	public:

		TEST_METHOD(Initialiser)
		{
			/* addModUsesbyStmt */
			PKB::getPKB()->addModifies(1, "a");
			PKB::getPKB()->addModifies(2, "b");
			PKB::getPKB()->addModifies(2, "c");
			PKB::getPKB()->addModifies(2, "z");
			PKB::getPKB()->addModifies(3, "a");
			PKB::getPKB()->addModifies(4, "xyz");

			PKB::getPKB()->addUses(1, "a");
			PKB::getPKB()->addUses(1, "i");
			PKB::getPKB()->addUses(2, "d");
			PKB::getPKB()->addUses(2, "i");
			PKB::getPKB()->addUses(5, "e");
			PKB::getPKB()->addUses(5, "a");

			/* addModUsesbyVar */
			PKB::getPKB()->addModifies("a", 1);
			PKB::getPKB()->addModifies("a", 3);
			PKB::getPKB()->addModifies("b", 2);
			PKB::getPKB()->addModifies("c", 2);
			PKB::getPKB()->addModifies("z", 2);
			PKB::getPKB()->addModifies("xyz", 4);

			PKB::getPKB()->addUses("a", 1);
			PKB::getPKB()->addUses("a", 5);
			PKB::getPKB()->addUses("i", 1);
			PKB::getPKB()->addUses("i", 2);
			PKB::getPKB()->addUses("d", 2);
			PKB::getPKB()->addUses("e", 5);

			PKB::getPKB()->addStatement(1, "assign");
			PKB::getPKB()->addStatement(2, "assign");
			PKB::getPKB()->addStatement(1, "while"); //invalid case
			PKB::getPKB()->addStatement(3, "assign");
			PKB::getPKB()->addStatement(5, "while");
			PKB::getPKB()->addStatement(6, "assign");
			PKB::getPKB()->addStatement(7, "invalidtype"); //invalid
			PKB::getPKB()->addStatement(8, "assign");
			PKB::getPKB()->addStatement(9, "while");
			PKB::getPKB()->addStatement(10, "assign");
		}

		TEST_METHOD(testModUsesbyStmtFunction)
		{
			list<string> actual, expected;

			actual = PKB::getPKB()->getModifiedBy(1);
			expected = { "a" };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getModifiedBy(2);
			expected = { "b", "c", "z" };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getModifiedBy(3);
			expected = { "xyz" };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getUsedBy(1);
			expected = { "a", "i" };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getUsedBy(2);
			expected = { "d", "i" };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getUsedBy(5);
			expected = { "e", "a" };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getUsedBy(10);
			expected = {};
			Assert::IsTrue(actual == expected);

			Assert::IsTrue(PKB::getPKB()->isModified(1, "a"));
			Assert::IsTrue(PKB::getPKB()->isModified(2, "b"));
			Assert::IsTrue(PKB::getPKB()->isModified(2, "c"));
			Assert::IsTrue(PKB::getPKB()->isModified(2, "z"));
			Assert::IsTrue(PKB::getPKB()->isModified(4, "xyz"));
			/* -1 is not a existent statement statement no(key)*/
			Assert::IsFalse(PKB::getPKB()->isModified(-1, "a"));
			/* "xyz" exist, but stmt 2 is not its corresponding key*/
			Assert::IsFalse(PKB::getPKB()->isModified(2, "xyz"));
			/* "_xyz_" does not exist in the modTable*/
			Assert::IsFalse(PKB::getPKB()->isModified(4, "_xyz_"));

			Assert::IsTrue(PKB::getPKB()->isUsed(1, "a"));
			Assert::IsTrue(PKB::getPKB()->isUsed(1, "i"));
			Assert::IsTrue(PKB::getPKB()->isUsed(2, "d"));
			Assert::IsTrue(PKB::getPKB()->isUsed(5, "e"));
			Assert::IsFalse(PKB::getPKB()->isUsed(-1, "a"));
			Assert::IsFalse(PKB::getPKB()->isUsed(5, "i"));

			/* check the isValid code */
			/* is there a need to check both the mod & uses table? */
			Assert::IsTrue(PKB::getPKB()->isValidStmt(1));
			Assert::IsTrue(PKB::getPKB()->isValidStmt(2));
			Assert::IsTrue(PKB::getPKB()->isValidStmt(3));
			Assert::IsTrue(PKB::getPKB()->isValidStmt(4));
			Assert::IsTrue(PKB::getPKB()->isValidStmt(5));
		}

		TEST_METHOD(testModUsesbyVarFunction)
		{
			list<int> actual, expected;
			list<string> actualVarList, expectedVarList;

			actual = PKB::getPKB()->getModifiedBy("a");
			expected = { 1,3 };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getModifiedBy("b");
			expected = { 2 };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getModifiedBy("c");
			expected = { 2 };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getModifiedBy("z");
			expected = { 2 };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getModifiedBy("xyz");
			expected = { 4 };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getUsedBy("a");
			expected = { 1, 5 };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getUsedBy("i");
			expected = { 1, 2 };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getUsedBy("d");
			expected = { 2 };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getUsedBy("e");
			expected = { 5 };
			Assert::IsTrue(actual == expected);

			actual = PKB::getPKB()->getUsedBy("invalid");
			expected = {};
			Assert::IsTrue(actual == expected);

			/* all the variables from the mod and uses table should be valid */
			Assert::IsTrue(PKB::getPKB()->isValidVar("a"));
			Assert::IsTrue(PKB::getPKB()->isValidVar("b"));
			Assert::IsTrue(PKB::getPKB()->isValidVar("c"));
			Assert::IsTrue(PKB::getPKB()->isValidVar("z"));
			Assert::IsTrue(PKB::getPKB()->isValidVar("xyz"));
			Assert::IsTrue(PKB::getPKB()->isValidVar("i"));
			Assert::IsTrue(PKB::getPKB()->isValidVar("d"));
			Assert::IsTrue(PKB::getPKB()->isValidVar("e"));

			actualVarList = PKB::getPKB()->getAllModVar();
			expectedVarList = { "a", "b", "c", "z", "xyz" };
			Assert::IsTrue(actual == expected);

			/* clear the existing modVarList and check if the assertion is
			correct for the case where the list is empty */

			actualVarList = PKB::getPKB()->getAllUsedVar();
			expectedVarList = { "a", "i", "d", "e" };
			Assert::IsTrue(actual == expected);

			actualVarList = PKB::getPKB()->getVarList();
			expectedVarList = { "a", "b", "c", "z", "xyz", "i", "d", "e" };
			Assert::IsTrue(actual == expected);
		}


		TEST_METHOD(testStatementFunction)
		{
			list<int> actual, expected1, expected2;
			int actualTotal, expectedTotal;

			actual = PKB::getPKB()->getAssignList();
			expected1 = { 1, 2, 3, 6, 8, 10 };
			expected2 = { 1, 2, 3, 6, 7, 8, 10 };
			Assert::IsTrue(actual == expected1);
			/* stmt 7 should not be in the list; stmtType is invalid */
			Assert::IsFalse(actual == expected2);
			
			actual = PKB::getPKB()->getWhileList();
			expected1 = {5, 9};
			expected2 = {5, 7, 9};
			Assert::IsTrue(actual == expected1);
			/* stmt 7 should not be in the list; stmtType is invalid */
			Assert::IsFalse(actual == expected2);

			actual = PKB::getPKB()->getStmtList();
			expected1 = { 1, 2, 3, 5, 6, 8, 9, 10 };
			Assert::IsTrue(actual == expected1);
			
			actualTotal = PKB::getPKB()->getStatementCount();
			expectedTotal = 8;
			Assert::IsTrue(actualTotal == expectedTotal);
		}
	};
}