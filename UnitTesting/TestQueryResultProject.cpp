#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser.h"
#include "QueryValidator.h"
#include "QueryEvaluator.h"
#include "QueryResultProjector.h"
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
	TEST_CLASS(TestQueryResultProjector) {
		TEST_METHOD(testBasicQueryOutput) {
			Parser parser("yc_sample_src.txt");
			parser.process();
			string query = "stmt s; assign a; while w; constant c; prog_line n; Select a such that Follows(2,3)"; // true 1,2,3,5,7,8,9
			QueryValidator qv(query);
			QueryTable qt = qv.parse();
			QueryEvaluator qe = QueryEvaluator(qt);
			QueryTable results = qe.evaluate();
			Assert::AreEqual(results.isSelectResultEmpty(), false);
			Assert::AreEqual(results.isSuchThatResultEmpty(), true);
			Assert::AreEqual(results.isPatternResultEmpty(), true);
			vector<string> selectResults = results.getSelectResult().getArg1ResultList();
			Assert::AreEqual(selectResults.size(), (size_t)7);

			vector<string> expectedResultList = { "1", "2", "3", "5", "7", "8", "9" };
			vector<string>::iterator expectedResultIterator = expectedResultList.begin();
			for (vector<string>::iterator it = selectResults.begin(); it != selectResults.end() && expectedResultIterator != expectedResultList.end(); it++) {
				Assert::AreEqual(*it, *expectedResultIterator);
				expectedResultIterator++;
			}

			QueryResultProjector qrp(results);
			list<string> finalResults = qrp.getResults();
			expectedResultIterator = expectedResultList.begin();
			for (list<string>::iterator it = finalResults.begin(); it != finalResults.end(); it++) {
				Assert::AreEqual(*it, *expectedResultIterator);
				expectedResultIterator++;
			}
		}
	};
}