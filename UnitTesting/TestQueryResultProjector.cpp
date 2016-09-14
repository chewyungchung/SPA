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
			string query = "assign a; variable v; Select v pattern a(v, _\"x\"_)"; // 1
			QueryValidator qv(query);
			QueryTable qt = qv.parse();
			Assert::AreEqual(qt.isNullQuery(), -1,L"NULL");
			PKB pkb = parser.process();
			QueryEvaluator qe = QueryEvaluator(qt, pkb);
			QueryTable results = qe.evaluate();
			Assert::AreEqual(results.isNullQuery(), -1, L"123");
			Assert::AreEqual(results.isSelectResultEmpty(), -1 , L"1111");
			Assert::AreEqual(results.isSuchThatResultEmpty(), 1, L"12222");
			Assert::AreEqual(results.isPatternResultEmpty(), -1,L"14444");
			vector<string> selectResults = results.getSelectResult().getArg1ResultList();
			vector<string> patternResults = results.getPatternResult().getPatternResultList();
			Assert::AreEqual(selectResults.size(), (size_t)4, L"SelectResult");
			Assert::AreEqual(patternResults.size(), (size_t)1,L"PatternResult"); // 12345
			QueryResultProjector qrp(results);
			list<string> finalResults = qrp.getResults();
			Assert::IsFalse(finalResults.empty());

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