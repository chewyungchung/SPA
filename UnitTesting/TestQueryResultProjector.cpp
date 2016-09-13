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
			Parser parser("C:\\Users\\Einlanz\\Documents\\GitSPA\\Release\\Sample-Source.txt");
			parser.process();
			string query = "while w; Select w such that Parent(w, 5)"; // 4
			QueryValidator qv(query);
			QueryTable qt = qv.parse();
			QueryEvaluator qe = QueryEvaluator(qt);
			QueryTable results = qe.evaluate();
			Assert::AreEqual(results.isSelectResultEmpty(), false);
			Assert::AreEqual(results.isSuchThatResultEmpty(), false);
			Assert::AreEqual(results.isPatternResultEmpty(), true);
			vector<string> selectResults = results.getSelectResult().getArg1ResultList();
			Assert::AreEqual(selectResults.size(), (size_t)1);

			vector<string> expectedResultList = { "4"};
			vector<string>::iterator expectedResultIterator = expectedResultList.begin();
			for (vector<string>::iterator it = selectResults.begin(); it != selectResults.end() && expectedResultIterator != expectedResultList.end(); it++) {
				Assert::AreEqual(*it, *expectedResultIterator);
				expectedResultIterator++;
			}

			QueryResultProjector qrp(results);
			list<string> finalResults = qrp.getResults();

			Assert::AreEqual(finalResults.empty(), false);

			expectedResultIterator = expectedResultList.begin();
			bool enterLooped = false;
			for (list<string>::iterator it = finalResults.begin(); it != finalResults.end(); ++it) {
				Assert::AreEqual(*it, *expectedResultIterator);
				expectedResultIterator++;
				enterLooped = true;
			}

			Assert::IsTrue(enterLooped);


			query = "while w; Select w such that Parent(w, 6)"; // 4
			qv = QueryValidator(query);
			qt = qv.parse();
			qe = QueryEvaluator(qt);
			results = qe.evaluate();
			Assert::AreEqual(results.isSelectResultEmpty(), false);
			Assert::AreEqual(results.isSuchThatResultEmpty(), true);
			Assert::AreEqual(results.isPatternResultEmpty(), true);
			selectResults = results.getSelectResult().getArg1ResultList();
			vector<string> suchThatResults = results.getSuchThatResult().getArg1ResultList();
			Assert::AreEqual(selectResults.size(), (size_t)1);
			Assert::AreEqual(suchThatResults.size(), (size_t)0);

			qrp = QueryResultProjector(results);
			finalResults = qrp.getResults();

			Assert::AreEqual(finalResults.empty(), true);

			
		}
	};
}