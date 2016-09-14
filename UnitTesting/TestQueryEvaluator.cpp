#include "stdafx.h"

//#include "stdafx.h"
//#include "CppUnitTest.h"
//#include "Parser.h"
//#include "QueryValidator.h"
//#include "QueryEvaluator.h"
//#include "QueryResultProjector.h"
//#include "QueryTokenizer.h"
//#include "QueryToken.h"
//#include "QueryTable.h"
//#include "QueryException.h"
//#include "RelationTable.h"
//#include "Relation.h"
//#include "Clause.h"
//
//using namespace Microsoft::VisualStudio::CppUnitTestFramework;
//using namespace std;
//
//namespace UnitTesting {
//	TEST_CLASS(TestQueryEvaluator) {
//		TEST_METHOD(testBasicEvaluateSelect) {
//			string query = "stmt s; assign a; while w; constant c; prog_line n; Select s"; // 1,2,3,4,5,6,7,8,9
//			Parser parser("C:\\Users\\Einlanz\\Documents\\GitSPA\\Release\\Sample-Source.txt");
//			parser.process();
//			QueryValidator qv(query);
//			QueryTable qt = qv.parse();
//			QueryEvaluator qe(qt);
//			QueryTable results = qe.evaluate();
//			Assert::AreEqual(results.isSelectResultEmpty(), false);
//			Assert::AreEqual(results.isSuchThatResultEmpty(), true);
//			Assert::AreEqual(results.isPatternResultEmpty(), true);
//			vector<string> selectResults = results.getSelectResult().getArg1ResultList();
//			Assert::AreEqual(selectResults.size(), (size_t)9);
//			
//			vector<string> expectedResultList({ "1", "2", "3", "4", "5", "6", "7", "8", "9" });
//			vector<string>::iterator expectedResultIterator = expectedResultList.begin();
//			for (vector<string>::iterator it = selectResults.begin(); it != selectResults.end() && expectedResultIterator != expectedResultList.end(); it++) {
//				Assert::AreEqual(*it, *expectedResultIterator);
//				expectedResultIterator++;
//			}
//
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select a"; // 1,2,3,5,7,8,9
//			qv = QueryValidator(query);
//			qt = qv.parse();
//			qe = QueryEvaluator(qt);
//			results = qe.evaluate();
//			Assert::AreEqual(results.isSelectResultEmpty(), false);
//			Assert::AreEqual(results.isSuchThatResultEmpty(), true);
//			Assert::AreEqual(results.isPatternResultEmpty(), true);
//			selectResults = results.getSelectResult().getArg1ResultList();
//			Assert::AreEqual(selectResults.size(), (size_t)7);
//
//			expectedResultList = { "1", "2", "3", "5", "7", "8", "9"};
//			expectedResultIterator = expectedResultList.begin();
//			for (vector<string>::iterator it = selectResults.begin(); it != selectResults.end() && expectedResultIterator != expectedResultList.end(); it++) {
//				Assert::AreEqual(*it, *expectedResultIterator);
//				expectedResultIterator++;
//			}
//
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select w"; // 4,6
//			qv = QueryValidator(query);
//			qt = qv.parse();
//			qe = QueryEvaluator(qt);
//			results = qe.evaluate();
//			Assert::AreEqual(results.isSelectResultEmpty(), false);
//			Assert::AreEqual(results.isSuchThatResultEmpty(), true);
//			Assert::AreEqual(results.isPatternResultEmpty(), true);
//			selectResults = results.getSelectResult().getArg1ResultList();
//			Assert::AreEqual(selectResults.size(), (size_t)2);
//
//			expectedResultList = { "4", "6" };
//			expectedResultIterator = expectedResultList.begin();
//			for (vector<string>::iterator it = selectResults.begin(); it != selectResults.end() && expectedResultIterator != expectedResultList.end(); it++) {
//				Assert::AreEqual(*it, *expectedResultIterator);
//				expectedResultIterator++;
//			}
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select c"; // 1,2,3,4,5,6,7
//			qv = QueryValidator(query);
//			qt = qv.parse();
//			qe = QueryEvaluator(qt);
//			results = qe.evaluate();
//			Assert::AreEqual(results.isSelectResultEmpty(), false);
//			Assert::AreEqual(results.isSuchThatResultEmpty(), true);
//			Assert::AreEqual(results.isPatternResultEmpty(), true);
//			selectResults = results.getSelectResult().getArg1ResultList();
//			Assert::AreEqual(selectResults.size(), (size_t)7);
//
//			expectedResultList = { "1", "2", "3", "4", "5", "6", "7" };
//			expectedResultIterator = expectedResultList.begin();
//			for (vector<string>::iterator it = selectResults.begin(); it != selectResults.end() && expectedResultIterator != expectedResultList.end(); it++) {
//				Assert::AreEqual(*it, *expectedResultIterator);
//				expectedResultIterator++;
//			}
//
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select n"; // 1,2,3,4,5,6,7,8,9
//			qv = QueryValidator(query);
//			qt = qv.parse();
//			qe = QueryEvaluator(qt);
//			results = qe.evaluate();
//			Assert::AreEqual(results.isSelectResultEmpty(), false);
//			Assert::AreEqual(results.isSuchThatResultEmpty(), true);
//			Assert::AreEqual(results.isPatternResultEmpty(), true);
//			selectResults = results.getSelectResult().getArg1ResultList();
//			Assert::AreEqual(selectResults.size(), (size_t)9);
//
//			expectedResultList = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
//			expectedResultIterator = expectedResultList.begin();
//			for (vector<string>::iterator it = selectResults.begin(); it != selectResults.end() && expectedResultIterator != expectedResultList.end(); it++) {
//				Assert::AreEqual(*it, *expectedResultIterator);
//				expectedResultIterator++;
//			}
//		}
//
//		TEST_METHOD(testEvaluateFollow) {
//			string query = "stmt s; assign a; while w; constant c; prog_line n; Select a such that Follows(1,1)";	 // false, corner case
//			Parser parser("C:\\Users\\Einlanz\\Documents\\GitSPA\\Release\\Sample-Source.txt");
//			parser.process();
//			QueryValidator qv(query);
//			QueryTable qt = qv.parse();
//			QueryEvaluator qe(qt);
//			QueryTable results = qe.evaluate();
//			Assert::AreEqual(results.isSelectResultEmpty(), true);
//			Assert::AreEqual(results.isSuchThatResultEmpty(), true);
//			Assert::AreEqual(results.isPatternResultEmpty(), true);
//
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select a such that Follows(2,3)"; // true 1,2,3,5,7,8,9
//			qv = QueryValidator(query);
//			qt = qv.parse();
//			qe = QueryEvaluator(qt);
//			results = qe.evaluate();
//			Assert::AreEqual(results.isSelectResultEmpty(), false);
//			Assert::AreEqual(results.isSuchThatResultEmpty(), true);
//			Assert::AreEqual(results.isPatternResultEmpty(), true);
//			vector<string> selectResults = results.getSelectResult().getArg1ResultList();
//			Assert::AreEqual(selectResults.size(), (size_t)7);
//
//			vector<string> expectedResultList = { "1", "2", "3", "5", "7", "8", "9" };
//			vector<string>::iterator expectedResultIterator = expectedResultList.begin();
//			for (vector<string>::iterator it = selectResults.begin(); it != selectResults.end() && expectedResultIterator != expectedResultList.end(); it++) {
//				Assert::AreEqual(*it, *expectedResultIterator);
//				expectedResultIterator++;
//			}
//			/*
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select a such that Follows(2,3)";   // true
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select w such that Follows(1,3)";   // false
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select w such that Follows(8,9)";   // true, boundary
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select n such that Follows(9,10)";  // false, 13 non existent
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select n such that Follows(3,2)";	 // false, not in sequence
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select a such that Follows(4,5)";	 // false, not in same stmtLst
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select a such that Follows(4,6)";	 // true, 2 conscecutive while loop
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(1,_)";   // true  
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(_,1)";	 // fail, nothing before 1
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(_,_)";   // true 
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(9,_)";   // false, nothing following last statement
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(_,9)";   // true
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(4,_)";	 // true, what follows is the next while
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(_,6)";	 // true, what followed from is the 1st while
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(s,2)";	 // true, 1
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(s,1)";   // false, nothing before 1st statement
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(1,s)";   // true, 2
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(4,s)";	 // true, 6
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(9,s)";	 // false, nothing follow last
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(s,s)";	 // false, corner case
//			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(2,3)";	 // 
//			*/
//		}
//
//		TEST_METHOD(testEvaluateFollowStar) {
//
//		}
//
//		TEST_METHOD(testEvaluateParent) {
//
//		}
//
//		TEST_METHOD(testEvaluateParentStar) {
//
//		}
//
//		TEST_METHOD(testEvaluateModifies) {
//
//		}
//
//		TEST_METHOD(testEvaluateUses) {
//
//		}
//
//		TEST_METHOD(testEvaluatePattern) {
//
//		}
//	};
//}