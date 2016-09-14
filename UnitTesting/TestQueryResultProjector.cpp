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
			string query = "assign a; Select a such that Follows(6, a)"; // 
			QueryValidator qv(query);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1);
			PKB pkb = parser.process();
			QueryEvaluator qe = QueryEvaluator(qt, pkb);
			QueryTable results = qe.evaluate();
			Assert::AreEqual(results.isNullQuery(), -1);
			Assert::AreEqual(results.isSelectResultEmpty(), -1, L"itsme");
			Assert::AreEqual(results.isSuchThatResultEmpty(), 1, L"itsme");
			Assert::AreEqual(results.isPatternResultEmpty(), 1);
			vector<string> selectResults = results.getSelectResult().getArg1ResultList();
			//vector<string> suchThatResults1 = results.getSuchThatResult().getArg1ResultList();
			vector<string> suchThatResults2 = results.getSuchThatResult().getArg2ResultList();
			//vector<string> suchThatResults2 = results.getSuchThatResult().getArg2ResultList();
			Assert::AreEqual(selectResults.size(), (size_t)6);
			//Assert::AreEqual(suchThatResults1.size(), (size_t)5, L"itsme"); // 12345
			Assert::AreEqual(suchThatResults2.size(), (size_t)0, L"itsme"); // 23456

			/*vector<string> expectedResultList = { "6"};
			vector<string>::iterator expectedResultIterator = expectedResultList.begin();
			for (vector<string>::iterator it = selectResults.begin(); it != selectResults.end() && expectedResultIterator != expectedResultList.end(); it++) {
				Assert::AreEqual(*it, *expectedResultIterator, L"lol");
				expectedResultIterator++;
			}*/

			QueryResultProjector qrp(results);
			list<string> finalResults = qrp.getResults();

			Assert::IsTrue(finalResults.empty(), L"ME");

			/*expectedResultIterator = expectedResultList.begin();
			bool enterLooped = false;
			for (list<string>::iterator it = finalResults.begin(); it != finalResults.end(); ++it) {
				Assert::AreEqual(*it, *expectedResultIterator);
				expectedResultIterator++;
				enterLooped = true;
			}*/
			//Assert::IsTrue(enterLooped);

			//query = "while w; Select w such that Parent(w, 6)"; // 4
			//qv = QueryValidator(query);
			//qt = qv.parse();
			//qe = QueryEvaluator(qt,pkb);
			//results = qe.evaluate();
			//Assert::AreEqual(results.isSelectResultEmpty(), false);
			//Assert::AreEqual(results.isSuchThatResultEmpty(), true);
			//Assert::AreEqual(results.isPatternResultEmpty(), true);
			//selectResults = results.getSelectResult().getArg1ResultList();
			//vector<string> suchThatResults = results.getSuchThatResult().getArg1ResultList();
			//Assert::AreEqual(selectResults.size(), (size_t)1);
			//Assert::AreEqual(suchThatResults.size(), (size_t)0);

			//qrp = QueryResultProjector(results);
			//finalResults = qrp.getResults();

			//Assert::AreEqual(finalResults.empty(), true);

			//
		}
	};
}