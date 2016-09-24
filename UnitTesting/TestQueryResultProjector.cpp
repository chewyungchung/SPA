#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser.h"
#include "PKB.h"
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
			Parser _parser;
			PKB _pkb;
			QueryEvaluator _qe;
			QueryValidator _qv;
			QueryResultProjector _qrp;
			QueryTable _qt;
			QueryTable _qtResults;
			list<string> finalResults;
			string query;
			vector<string> selectResults;
			vector<string> suchThatResults1;
			vector<string> suchThatResults2;
			vector<string> patternSynResults;
			vector<string> patternArg1Results;

			query = "assign a; Select a pattern a(_, _\"1\"_)"; // y
			_parser = Parser("C:\\Users\\Einlanz\\Documents\\GitSPA\\AutoTester\\Release\\PATTERN_SOURCE_01.txt");
			_qv = QueryValidator(query);
			_qt = _qv.parse();

			Assert::AreEqual(_qt.isNullQuery(), -1,L"NULL query");
			
			_pkb = _parser.process();
			_qe = QueryEvaluator(_qt, _pkb);
			_qtResults = _qe.evaluate();
			
			//Assert::AreEqual(_qtResults.isNullQuery(), -1, L"NULL results");
			//Assert::AreEqual(_qtResults.isSelectResultEmpty(), -1 , L"Select not suppose to empty");
			//Assert::AreEqual(_qtResults.isSuchThatResultEmpty(), 1, L"Such suppose to empty");
			//Assert::AreEqual(_qtResults.isPatternResultEmpty(), -1, L"Pattern not suppose to empty");
			//
			//selectResults = _qtResults.getSelectResult().getArg1ResultList();
			//suchThatResults1 = _qtResults.getSuchThatResult().getArg1ResultList();
			//suchThatResults2 = _qtResults.getSuchThatResult().getArg2ResultList();
			//patternSynResults = _qtResults.getPatternResult().getPatternResultList();
			//patternArg1Results = _qtResults.getPatternResult().getArg1ResultList();

			//Assert::AreEqual(selectResults.size(), (size_t)17, L"SelectResult");
			//Assert::AreEqual(suchThatResults1.size(), (size_t)0,L"SuchThat ONE"); 
			//Assert::AreEqual(suchThatResults2.size(), (size_t)0, L"SuchThat TWO");
			//Assert::AreEqual(patternSynResults.size(), (size_t)1, L"Pattern SYN"); // 4
			//Assert::AreEqual(patternArg1Results.size(), (size_t)0, L"Pattern ONE"); // y

			_qrp = QueryResultProjector(_qtResults,_pkb);
			finalResults = _qrp.getResults();
			finalResults.unique();
			Assert::AreEqual(finalResults.size(), (size_t)1, L"FINAL RESULTS SIZE");




			//query = "assign a; Select a such that Follows(3, 6) pattern a(\"x\", _)"; // y
			//_parser = Parser("C:\\Users\\Einlanz\\Documents\\GitSPA\\Release\\Sample-Source.txt");
			//_qv = QueryValidator(query);
			//_qt = _qv.parse();

			//Assert::AreEqual(_qt.isNullQuery(), -1, L"NULL query");

			//_pkb = _parser.process();
			//_qe = QueryEvaluator(_qt, _pkb);
			//_qtResults = _qe.evaluate();

			//Assert::AreEqual(_qtResults.isNullQuery(), -1, L"NULL results");
			//Assert::AreEqual(_qtResults.isSelectResultEmpty(), -1, L"Select not suppose to empty");
			//Assert::AreEqual(_qtResults.isSuchThatResultEmpty(), 1, L"Such suppose to empty");
			//Assert::AreEqual(_qtResults.isPatternResultEmpty(), -1, L"Pattern not suppose to empty");

			//Assert::AreEqual(_qtResults.getSelectResult().getIsExist(), 1, L"SELECT SHOULD EXIST");
			//selectResults = _qtResults.getSelectResult().getArg1ResultList();
			//
			//Assert::AreEqual(_qtResults.getSuchThatResult().getIsExist(), -1, L"SUCHTHAT SHOULD NOT EXIST");
			//suchThatResults1 = _qtResults.getSuchThatResult().getArg1ResultList();
			//suchThatResults2 = _qtResults.getSuchThatResult().getArg2ResultList();
			//
			//Assert::AreEqual(_qtResults.getPatternResult().getIsExist(), 1, L"PATTERN SHOULD EXIST");
			//patternSynResults = _qtResults.getPatternResult().getPatternResultList();
			//patternArg1Results = _qtResults.getPatternResult().getArg1ResultList();

			//Assert::AreEqual(selectResults.size(), (size_t)6, L"SelectResult");
			//Assert::AreEqual(suchThatResults1.size(), (size_t)0,L"SuchThat ONE");
			//Assert::AreEqual(suchThatResults2.size(), (size_t)0, L"SuchThat TWO");
			//Assert::AreEqual(patternSynResults.size(), (size_t)2, L"Pattern SYN"); // 4
			//Assert::AreEqual(patternArg1Results.size(), (size_t)0, L"Pattern ONE"); // y

			//_qrp = QueryResultProjector(_qtResults);
			//finalResults = _qrp.getResults();
			//Assert::AreEqual(finalResults.size(), (size_t)0, L"FINAL RESULTS SIZE");

			//query = "assign a; Select a such that Follows(3, 6) pattern a(\"x\", _)"; // y
			//_parser = Parser("C:\\Users\\Einlanz\\Documents\\GitSPA\\Release\\Sample-Source.txt");
			//_qv = QueryValidator(query);
			//_qt = _qv.parse();

			//Assert::AreEqual(_qt.isNullQuery(), -1, L"NULL query");

			//_pkb = _parser.process();
			//_qe = QueryEvaluator(_qt, _pkb);
			//_qtResults = _qe.evaluate();

			//Assert::AreEqual(_qtResults.isNullQuery(), -1, L"NULL results");
			//Assert::AreEqual(_qtResults.isSelectResultEmpty(), -1, L"Select not suppose to empty");
			//Assert::AreEqual(_qtResults.isSuchThatResultEmpty(), 1, L"Such suppose to empty");
			//Assert::AreEqual(_qtResults.isPatternResultEmpty(), -1, L"Pattern not suppose to empty");

			//Assert::AreEqual(_qtResults.getSelectResult().getIsExist(), 1, L"SELECT SHOULD EXIST");
			//selectResults = _qtResults.getSelectResult().getArg1ResultList();
			//
			//Assert::AreEqual(_qtResults.getSuchThatResult().getIsExist(), -1, L"SUCHTHAT SHOULD NOT EXIST");
			//suchThatResults1 = _qtResults.getSuchThatResult().getArg1ResultList();
			//suchThatResults2 = _qtResults.getSuchThatResult().getArg2ResultList();
			//
			//Assert::AreEqual(_qtResults.getPatternResult().getIsExist(), 1, L"PATTERN SHOULD EXIST");
			//patternSynResults = _qtResults.getPatternResult().getPatternResultList();
			//patternArg1Results = _qtResults.getPatternResult().getArg1ResultList();

			//Assert::AreEqual(selectResults.size(), (size_t)6, L"SelectResult");
			//Assert::AreEqual(suchThatResults1.size(), (size_t)0,L"SuchThat ONE");
			//Assert::AreEqual(suchThatResults2.size(), (size_t)0, L"SuchThat TWO");
			//Assert::AreEqual(patternSynResults.size(), (size_t)2, L"Pattern SYN"); // 4
			//Assert::AreEqual(patternArg1Results.size(), (size_t)0, L"Pattern ONE"); // y

			//_qrp = QueryResultProjector(_qtResults);
			//finalResults = _qrp.getResults();
			//Assert::AreEqual(finalResults.size(), (size_t)0, L"FINAL RESULTS SIZE");










			//query = "prog_line n; Select n such that Parent*(4,n)"; // y
			//_parser = Parser("C:\\Users\\Einlanz\\Documents\\GitSPA\\Release\\procedure_ABC_source.txt");
			//_qv = QueryValidator(query);
			//_qt = _qv.parse();

			//Assert::AreEqual(_qt.isNullQuery(), -1, L"NULL query");

			//_pkb = _parser.process();
			//_qe = QueryEvaluator(_qt, _pkb);
			//_qtResults = _qe.evaluate();

			//Assert::AreEqual(_qtResults.isNullQuery(), -1, L"NULL results");
			//Assert::AreEqual(_qtResults.isSelectResultEmpty(), -1, L"Select not suppose to empty");
			//Assert::AreEqual(_qtResults.isSuchThatResultEmpty(), -1, L"Such That not suppose to empty");
			//Assert::AreEqual(_qtResults.isPatternResultEmpty(), -1, L"Pattern not suppose to empty");

			//Assert::AreEqual(_qtResults.getSelectResult().getIsExist(), 1, L"SELECT SHOULD EXIST");
			//selectResults = _qtResults.getSelectResult().getArg1ResultList();

			//Assert::AreEqual(_qtResults.getSuchThatResult().getIsExist(), 1, L"SUCHTHAT SHOULD EXIST");
			//suchThatResults1 = _qtResults.getSuchThatResult().getArg1ResultList();
			//suchThatResults2 = _qtResults.getSuchThatResult().getArg2ResultList();

			//Assert::AreEqual(_qtResults.getPatternResult().getIsExist(), 1, L"PATTERN SHOULD EXIST");
			//patternSynResults = _qtResults.getPatternResult().getPatternResultList();
			//patternArg1Results = _qtResults.getPatternResult().getArg1ResultList();

			//Assert::AreEqual(selectResults.size(), (size_t)6, L"SelectResult");
			//Assert::AreEqual(suchThatResults1.size(), (size_t)6, L"SuchThat ONE");
			//Assert::AreEqual(suchThatResults2.size(), (size_t)0, L"SuchThat TWO");
			//Assert::AreEqual(patternSynResults.size(), (size_t)2, L"Pattern SYN"); // 4
			//Assert::AreEqual(patternArg1Results.size(), (size_t)0, L"Pattern ONE"); // y

			//_qrp = QueryResultProjector(_qtResults);
			//finalResults = _qrp.getResults();
			//Assert::AreEqual(finalResults.size(), (size_t)2, L"FINAL RESULTS SIZE");
		}
	};
}