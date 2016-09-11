#include "stdafx.h"
#include "CppUnitTest.h"
#include "QueryValidator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTesting {
	TEST_CLASS(TestQueryValidator)
	{
	public:

		// Interesting case
		TEST_METHOD(TestBoolOnly) {
			string queryString = "select BOOLEAN;";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"BOOLEAN");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"BOOLEAN");
			delete selectClause;
			delete qt;
		}


		// ClauseCount: 0 -> Select declared synonym
		TEST_METHOD(testZeroClauseSelectDeclared)
		{
			string queryString = "assign a; Select a;";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"a");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"assign");
			delete selectClause;
			delete qt;
			
			string queryString = "while w; Select w";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"w");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"while");
			delete selectClause;
			delete qt;

			string queryString = "stmt s; Select s";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"s");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"stmt");
			delete selectClause;
			delete qt;

			string queryString = "prog_line p; Select p";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"p");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"prog_line");
			delete selectClause;
			delete qt;

			string queryString = "variable v; Select v";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"v");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"variable");
			delete selectClause;
			delete qt;

			string queryString = "constant c; Select c";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"c");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"constant");
			delete selectClause;
			delete qt;
		}

		// ClauseCount: 0 -> Select undeclared synonym
		TEST_METHOD(testZeroClauseSelectUndeclared)
		{
			// select small S
			string queryString = "select ";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNull(qt);
			delete qt;

			string queryString = "Select w";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNull(qt);
			delete qt;

			string queryString = "assign abc; while def; stmt ghi; Select w";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNull(qt);
			delete qt;
		}

		// ClauseCount: 1 (SUCH THAT)
		// Valid Select argument and Valid Follow/Follow* arguments
		TEST_METHOD(testOneClauseFollowsAndStar) {
			string queryString = "assign a; Select a such that Follows(1,2)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"a");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"follows");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"1");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"2");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"constant");
			delete selectClause;
			delete qt;

			string queryString = "assign a; Select a such that Follows*(1,2)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"a");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"follows*");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"1");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"2");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"constant");
			delete selectClause;
			delete qt;

			string queryString = "stmt st4t3m3nt; prog_line n; Select st4t3m3nt such that Follows(st4t3m3nt,n)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"st4t3m3nt");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"follows");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"st4t3m3nt");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"n");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"prog_line");
			delete selectClause;
			delete qt;

			string queryString = "stmt st4t3m3nt; prog_line n; Select st4t3m3nt such that Follows*(st4t3m3nt,n)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"st4t3m3nt");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"follows*");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"st4t3m3nt");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"n");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"prog_line");
			delete selectClause;
			delete qt;

			string queryString = "assign abar; while war; Select war such that Follows(abar,war)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"war");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"follows");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"abar");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"war");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"while");
			delete selectClause;
			delete qt;

			string queryString = "assign abar; while war; Select war such that Follows*(abar,war)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"war");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"follows*");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"abar");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"war");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"while");
			delete selectClause;
			delete qt;

			string queryString = "while w1nd; Select w1nd such that Follows(_,_)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"w1nd");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"follows");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"_");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"_");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"any");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"any");
			delete selectClause;
			delete qt;

			string queryString = "while w1nd; Select w1nd such that Follows*(_,_)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"w1nd");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"while");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"follows*");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"_");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"_");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"any");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"any");
			delete selectClause;
			delete qt;
		}

		// ClauseCount: 1 (SUCH THAT)
		// Valid Select argument and Valid Parent/Parent* arguments
		TEST_METHOD(testOneClauseParentAndStar) {
			string queryString = "assign a; Select a such that Parent(1,2)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"a");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"parent");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"1");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"2");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"constant");
			delete selectClause;
			delete qt;

			string queryString = "assign a; Select a such that Parent*(1,2)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"a");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"parent*");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"1");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"2");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"constant");
			delete selectClause;
			delete qt;

			string queryString = "stmt s; prog_line lin3pr0gram; Select s such that Parent(s,lin3pr0gram)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"s");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"parent");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"s");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"lin3pr0gram");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"s");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"prog_line");
			delete selectClause;
			delete qt;

			string queryString = "stmt s; prog_line lin3pr0gram; Select s such that Parent*(s,lin3pr0gram)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"s");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"stmt");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"parent*");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"s");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"lin3pr0gram");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"s");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"prog_line");
			delete selectClause;
			delete qt;

			string queryString = "assign a55ign; while war0; Select a55ign such that Parent(a55ign,war0)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"a55ign");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"parent");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"a55ign");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"war0");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"while");
			delete selectClause;
			delete qt;

			string queryString = "assign a55ign; while war0; Select a55ign such that Parent*(a55ign,war0)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"a55ign");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"parent*");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"a55ign");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"war0");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"assign");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"while");
			delete selectClause;
			delete qt;

			string queryString = "constant crab007; Select crab007 such that Parent(_,_)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"crab007");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"parent");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"_");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"_");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"any");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"any");
			delete selectClause;
			delete qt;

			string queryString = "constant crab007; Select crab007 such that Parent*(_,_)";
			QueryValidator qv(queryString);
			QueryTable* qt = qv.parse();
			Assert::IsNotNull(qt);
			Clause* selectClause = qt->getSelectClause();
			Assert::IsNotNull(selectClause);
			Clause* suchThatClause = qt->getSuchThatClause();
			Assert::IsNotNull(suchThatClause);
			Assert::AreEqual(selectClause->getArg().at(0), (string)"crab007");
			Assert::AreEqual(selectClause->getArgType().at(0), (string)"constant");
			Assert::AreEqual(suchThatClause->getRelation(), (string)"parent*");
			Assert::AreEqual(suchThatClause->getArg().at(0), (string)"_");
			Assert::AreEqual(suchThatClause->getArg().at(1), (string)"_");
			Assert::AreEqual(suchThatClause->getArgType().at(0), (string)"any");
			Assert::AreEqual(suchThatClause->getArgType().at(1), (string)"any");
			delete selectClause;
			delete qt;
		}
	};
}