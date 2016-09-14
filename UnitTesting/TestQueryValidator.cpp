#include "stdafx.h"
#include "CppUnitTest.h"
#include "QueryValidator.h"
#include "QueryTokenizer.h"
#include "QueryToken.h"
#include "QueryTable.h"
#include "QueryException.h"
#include "RelationTable.h"
#include "Relation.h"
#include "Clause.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTesting {
	TEST_CLASS(TestQueryValidator)
	{
	public:
		// Interesting case
		TEST_METHOD(TestBoolOnly) {
			string queryString = "Select BOOLEAN";
			QueryValidator qv(queryString);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			Clause selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"BOOLEAN");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"BOOLEAN");
		}

		// ClauseCount: 0 -> Select declared synonym
		TEST_METHOD(testZeroClauseSelectDeclared)
		{
			string queryString = "assign a,b,c; Select a";
			QueryValidator qv(queryString);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1, L"Query should not be NULL");
			Clause selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1, L"Select clause should not be NULL");
			Assert::AreEqual(selectClause.getArg().at(0), (string)"a");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"assign");
			
			queryString = "while w; Select w";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"w");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"while");
			
			queryString = "stmt s; Select s";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"s");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"stmt");
			
			queryString = "prog_line p; Select p";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"p");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"prog_line");

			queryString = "variable v; Select v";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"v");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"variable");
		
			queryString = "constant c; Select c";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"c");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"constant");
		}

		// ClauseCount: 0 -> Select undeclared synonym
		TEST_METHOD(testZeroClauseSelectUndeclared)
		{
			// select small S
			string queryString = "Select ";
			QueryValidator qv(queryString);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);

			queryString = "Select w";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);
			
			queryString = "assign abc; while def; stmt ghi; Select w";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);
		}

		// ClauseCount: 1 (SUCH THAT)
		// Valid Select argument and Valid Follow/Follow* arguments
		TEST_METHOD(testOneClauseFollowsAndStar) {
			string queryString = "assign a; Select a such that Follows(1,2)";
			QueryValidator qv(queryString);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			Clause selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Clause suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"a");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"follows");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"1");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"2");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"constant");

			queryString = "assign a; Select a such that Follows*(1,2)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"a");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"follows*");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"1");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"2");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"constant");

			queryString = "stmt st4t3m3nt; prog_line n; Select st4t3m3nt such that Follows(st4t3m3nt,n)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"st4t3m3nt");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"follows");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"st4t3m3nt");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"n");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"prog_line");

			queryString = "stmt st4t3m3nt; prog_line n; Select st4t3m3nt such that Follows*(st4t3m3nt,n)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"st4t3m3nt");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"follows*");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"st4t3m3nt");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"n");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"prog_line");

			queryString = "assign abar; while war; Select war such that Follows(abar,war)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"war");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"follows");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"abar");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"war");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"while");

			queryString = "assign abar; while war; Select war such that Follows*(abar,war)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"war");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"follows*");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"abar");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"war");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"while");

			queryString = "while w1nd; Select w1nd such that Follows(_,_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"w1nd");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"follows");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"_");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"_");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"any");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"any");

			queryString = "while w1nd; Select w1nd such that Follows*(_,_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"w1nd");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"follows*");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"_");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"_");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"any");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"any");
		}

		// ClauseCount: 1 (SUCH THAT)
		// Valid Select argument and Valid Parent/Parent* arguments
		TEST_METHOD(testOneClauseParentAndStar) {
			string queryString = "assign a; Select a such that Parent(1,2)";
			QueryValidator qv(queryString);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			Clause selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Clause suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"a");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"parent");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"1");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"2");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"constant");

			queryString = "assign a; Select a such that Parent*(1,2)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"a");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"parent*");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"1");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"2");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"constant");

			queryString = "stmt s; prog_line lin3pr0gram; Select s such that Parent(s,lin3pr0gram)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"s");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"parent");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"s");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"lin3pr0gram");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"prog_line");

			queryString = "stmt s,salm0n3lla; prog_line lin3pr0gram; Select s such that Parent*(s,lin3pr0gram)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"s");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"parent*");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"s");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"lin3pr0gram");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"prog_line");

			queryString = "assign a55ign; while war0; Select war0 such that Parent(war0,a55ign)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"war0");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"parent");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"war0");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"a55ign");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"assign");

			queryString = "assign a55ign; while war0; Select war0 such that Parent*(war0,a55ign)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"war0");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"parent*");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"war0");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"a55ign");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"assign");

			queryString = "constant crab007; Select crab007 such that Parent(_,_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"crab007");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"parent");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"_");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"_");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"any");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"any");

			queryString = "constant crab007; Select crab007 such that Parent*(_,_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"crab007");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"parent*");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"_");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"_");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"any");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"any");
		}

		// ClauseCount: 1 (SUCH THAT)
		// Valid Select argument and Valid Modifies arguments
		TEST_METHOD(testOneClauseModifies) {
			string queryString = "assign a; variable v; while w; constant c; stmt s; Select v such that Modifies(1,\"x\")";
			QueryValidator qv(queryString);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			Clause selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Clause suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"v");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"variable");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"modifies");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"1");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"x");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"string");

			queryString = "assign a1b2c3; variable d4; while e5; constant c6; stmt s; Select e5 such that Modifies(_,_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"e5");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"modifies");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"_");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"_");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"any");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"any");

			queryString = "assign a1b2c3; variable d4; while e5; constant c6; stmt salm0n3lla; Select c6 such that Modifies(a1b2c3,d4)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"c6");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"modifies");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"a1b2c3");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"d4");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"variable");

			queryString = "assign a1b2c3; variable d4; while e5; constant c6; stmt salm0n3lla; Select a1b2c3 such that Modifies(a1b2c3,\"10\")";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"a1b2c3");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"modifies");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"a1b2c3");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"10");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"string");
		}

		// ClauseCount: 1 (SUCH THAT)
		// Valid Select argument and Valid Uses arguments
		TEST_METHOD(testOneClauseUses) {
			string queryString = "assign a; variable v; while w; constant c; stmt s; Select v such that Uses(1,\"x\")";
			QueryValidator qv(queryString);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			Clause selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Clause suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"v");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"variable");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"uses");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"1");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"x");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"string");

			queryString = "assign a1b2c3; variable d4; while e5; constant c6; stmt s; Select e5 such that Uses(_,_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"e5");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"uses");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"_");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"_");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"any");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"any");

			queryString = "assign a1b2c3; variable d4; while e5; constant c6; stmt salm0n3lla; Select c6 such that Uses(a1b2c3,d4)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"c6");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"uses");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"a1b2c3");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"d4");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"variable");

			queryString = "assign a1b2c3; variable d4; while e5; constant c6; stmt salm0n3lla; Select a1b2c3 such that Uses(a1b2c3,\"10\")";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"a1b2c3");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"uses");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"a1b2c3");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"10");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"string");
		}

		// ClauseCount: 1 (PATTERN)
		// Valid Select argument and Valid Pattern arguments
		TEST_METHOD(testOneClausePattern) {
			string queryString = "assign a; variable v; while w; constant c; stmt s; Select a pattern a(_,_)";
			QueryValidator qv(queryString);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			Clause selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Clause patternClause = qt.getPatternClause();
			Assert::AreEqual(patternClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"a");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(patternClause.getRelation(), (string)"patternAssign");
			Assert::AreEqual(patternClause.getArg().at(0), (string)"_");
			Assert::AreEqual(patternClause.getArg().at(1), (string)"_");
			Assert::AreEqual(patternClause.getArg().at(2), (string)"a");
			Assert::AreEqual(patternClause.getArgType().at(0), (string)"any");
			Assert::AreEqual(patternClause.getArgType().at(1), (string)"any");
			Assert::AreEqual(patternClause.getArgType().at(2), (string)"assign");

			queryString = "assign a1b2c3; variable d4; while e5; constant c6; stmt salm0n3lla; Select salm0n3lla pattern a1b2c3(d4,_\"x\"_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			patternClause = qt.getPatternClause();
			Assert::AreEqual(patternClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"salm0n3lla");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"stmt");
			Assert::AreEqual(patternClause.getRelation(), (string)"patternAssign");
			Assert::AreEqual(patternClause.getArg().at(0), (string)"d4");
			Assert::AreEqual(patternClause.getArg().at(1), (string)"x");
			Assert::AreEqual(patternClause.getArg().at(2), (string)"a1b2c3");
			Assert::AreEqual(patternClause.getArgType().at(0), (string)"variable");
			Assert::AreEqual(patternClause.getArgType().at(1), (string)"string");
			Assert::AreEqual(patternClause.getArgType().at(2), (string)"assign");

			queryString = "assign a1b2c3; variable d4; while e5; constant c6; stmt salm0n3lla; Select d4 pattern a1b2c3(\"y\",_\"10\"_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			patternClause = qt.getPatternClause();
			Assert::AreEqual(patternClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"d4");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"variable");
			Assert::AreEqual(patternClause.getRelation(), (string)"patternAssign");
			Assert::AreEqual(patternClause.getArg().at(0), (string)"y");
			Assert::AreEqual(patternClause.getArg().at(1), (string)"10");
			Assert::AreEqual(patternClause.getArg().at(2), (string)"a1b2c3");
			Assert::AreEqual(patternClause.getArgType().at(0), (string)"string");
			Assert::AreEqual(patternClause.getArgType().at(1), (string)"constant");
			Assert::AreEqual(patternClause.getArgType().at(2), (string)"assign");
		}

		TEST_METHOD(testTwoClauseValidQuery) {
			string queryString = "assign a1b2c3; variable d4; while e5; constant c6; stmt salm0n3lla; Select d4 such that Follows*(a1b2c3, e5) pattern a1b2c3(\"y\",_\"10\"_)";
			QueryValidator qv(queryString);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			Clause selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Clause suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Clause patternClause = qt.getPatternClause();
			Assert::AreEqual(patternClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"d4");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"variable");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"follows*");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"a1b2c3");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"e5");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"while");
			Assert::AreEqual(patternClause.getRelation(), (string)"patternAssign");
			Assert::AreEqual(patternClause.getArg().at(0), (string)"y");
			Assert::AreEqual(patternClause.getArg().at(1), (string)"10");
			Assert::AreEqual(patternClause.getArg().at(2), (string)"a1b2c3");
			Assert::AreEqual(patternClause.getArgType().at(0), (string)"string");
			Assert::AreEqual(patternClause.getArgType().at(1), (string)"constant");
			Assert::AreEqual(patternClause.getArgType().at(2), (string)"assign");

			queryString = "assign a1b2c3; variable d4; while e5; constant c6; stmt salm0n3lla; Select d4 such that Parent*(e5, salm0n3lla) pattern a1b2c3(d4,_\"on3l0ngstr1ng\"_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			patternClause = qt.getPatternClause();
			Assert::AreEqual(patternClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"d4");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"variable");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"parent*");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"e5");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"salm0n3lla");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"stmt");
			Assert::AreEqual(patternClause.getRelation(), (string)"patternAssign");
			Assert::AreEqual(patternClause.getArg().at(0), (string)"d4");
			Assert::AreEqual(patternClause.getArg().at(1), (string)"on3l0ngstr1ng");
			Assert::AreEqual(patternClause.getArg().at(2), (string)"a1b2c3");
			Assert::AreEqual(patternClause.getArgType().at(0), (string)"variable");
			Assert::AreEqual(patternClause.getArgType().at(1), (string)"string");
			Assert::AreEqual(patternClause.getArgType().at(2), (string)"assign");

			queryString = "assign a1b2c3, aquaman, as0n; variable d4,vict0ry; while e5; constant c6; stmt salm0n3lla; Select c6 such that Modifies(salm0n3lla, _) pattern as0n(vict0ry,_\"on3l0ngstr1ng\"_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			patternClause = qt.getPatternClause();
			Assert::AreEqual(patternClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"c6");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getRelation(), (string)"modifies");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"salm0n3lla");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"_");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"any");
			Assert::AreEqual(patternClause.getRelation(), (string)"patternAssign");
			Assert::AreEqual(patternClause.getArg().at(0), (string)"vict0ry");
			Assert::AreEqual(patternClause.getArg().at(1), (string)"on3l0ngstr1ng");
			Assert::AreEqual(patternClause.getArg().at(2), (string)"as0n");
			Assert::AreEqual(patternClause.getArgType().at(0), (string)"variable");
			Assert::AreEqual(patternClause.getArgType().at(1), (string)"string");
			Assert::AreEqual(patternClause.getArgType().at(2), (string)"assign");
		}

		TEST_METHOD(testSyntaxError) {
			string queryString = "wr0ng";
			QueryValidator qv(queryString);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);

			queryString = "pattern such that Select";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);

			queryString = "while w; assine a; assasin a; Serect BOOREAN";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);

			queryString = "assign a1b2c3, aquaman, as0n; variable d4,vict0ry; while e5; constant c6; stmt salm0n3lla; Select c6 such that Modifies(salm0n3lla, _) puttern as0n(vict0ry,_\"on3l0ngstr1ng\"_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);

			queryString = "assign abc; while w123; Select w123 such dat pattern abc(_,_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);

			queryString = "assign a, b, a; Select a";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);

			queryString = "assign a, b, c; while x, y, x; Select x";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);

			// more than 1 common synonym
			queryString = "assign a; stmt s; variable v; Select s such that Uses(a, v) pattern a(v,_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);

			queryString = "assign a, a1; stmt s; variable v; Select s such that Modifies(a1,v) pattern a1(v,_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), 1);
		}

		TEST_METHOD(testValidStrangeDeclaration) {
			string queryString = "assign a; assign b; assign c; assign d; Select d";
			QueryValidator qv(queryString);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			Clause selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"d");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"assign");

			queryString = "      assign     farAssign   ;    Select    farAssign   such			that    Follows(1,2)   pattern   farAssign(_,_\"str1ng\"_)";
			qv = QueryValidator(queryString);
			qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			selectClause = qt.getSelectClause();
			Assert::AreEqual(selectClause.isClauseNull(), -1);
			Assert::AreEqual(selectClause.getArg().at(0), (string)"farAssign");
			Assert::AreEqual(selectClause.getArgType().at(0), (string)"assign");
			Clause suchThatClause = qt.getSuchThatClause();
			Assert::AreEqual(suchThatClause.isClauseNull(), -1);
			Assert::AreEqual(suchThatClause.getRelation(), (string)"follows");
			Assert::AreEqual(suchThatClause.getArg().at(0), (string)"1");
			Assert::AreEqual(suchThatClause.getArg().at(1), (string)"2");
			Assert::AreEqual(suchThatClause.getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause.getArgType().at(1), (string)"constant");
			Clause patternClause = qt.getPatternClause();
			Assert::AreEqual(patternClause.isClauseNull(), -1);
			Assert::AreEqual(patternClause.getArg().at(0), (string)"_");
			Assert::AreEqual(patternClause.getArg().at(1), (string)"str1ng");
			Assert::AreEqual(patternClause.getArg().at(2), (string)"farAssign");
			Assert::AreEqual(patternClause.getArgType().at(0), (string)"any");
			Assert::AreEqual(patternClause.getArgType().at(1), (string)"string");
			Assert::AreEqual(patternClause.getArgType().at(2), (string)"assign");
		}
	};
}