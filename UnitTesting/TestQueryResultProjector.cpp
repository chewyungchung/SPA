#include "stdafx.h"
#include "CppUnitTest.h"

#include "QueryResultProjector.h"
#include "Clause.h"
#include "ResultTable.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTesting {
	TEST_CLASS(TestQueryResultProjector) {
		TEST_METHOD(testConstructors) {
			
			QueryResultProjector _qrp;
			
			list<string> finalResults;
			
			ResultTable rt;
			rt = ResultTable("a1");
			ResultTable r2d2;
			r2d2 = ResultTable("r2d2");
			vector<ResultTable> vrt;
			vrt.push_back(rt);
			vrt.push_back(r2d2);
			ResultTable rt2 = ResultTable("a1");
			ResultTable c3po = ResultTable("c3po");
			vector<ResultTable> vrt2;
			vrt2.push_back(rt2);
			vrt2.push_back(c3po);
			vector<vector<ResultTable>> vvrt;
			vvrt.push_back(vrt);
			vvrt.push_back(vrt2);

			Clause c;
			vector<string> args;
			args.push_back("a1");
			vector<string> arg_types;
			arg_types.push_back("nonsense");
			c = Clause("NONSENSE", args, arg_types);
			_qrp = QueryResultProjector(vvrt, c);

			finalResults = _qrp.GetResults();
			size_t NUM_OF_COLUMNS_AFTER_INNER_JOIN = 3;

			Assert::AreEqual(NUM_OF_COLUMNS_AFTER_INNER_JOIN, finalResults.size(), L"FINAL RESULTS SIZE");




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