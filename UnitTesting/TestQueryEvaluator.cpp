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
	TEST_CLASS(TestQueryEvaluator) {
		TEST_METHOD(testBasicEvaluateSelect) {
			string query = "stmt s; assign a; while w; constant c; prog_line n; Select s"; // 1,2,3,4,5,6,7,8,9
			query = "stmt s; assign a; while w; constant c; prog_line n; Select a"; // 1,2,3,5,7,8,9
			query = "stmt s; assign a; while w; constant c; prog_line n; Select w"; // 4,6
			query = "stmt s; assign a; while w; constant c; prog_line n; Select c"; // 1,2,3,4,5,6,7
			query = "stmt s; assign a; while w; constant c; prog_line n; Select n"; // 1,2,3,4,5,6,7,8,9
		}

		TEST_METHOD(testEvaluateFollow) {
			string query = "stmt s; assign a; while w; constant c; prog_line n; Select s";
			query = "stmt s; assign a; while w; constant c; prog_line n; Select a such that Follows(1,1)";	 // false, corner case
			query = "stmt s; assign a; while w; constant c; prog_line n; Select a such that Follows(2,3)";   // true
			query = "stmt s; assign a; while w; constant c; prog_line n; Select w such that Follows(1,3)";   // false
			query = "stmt s; assign a; while w; constant c; prog_line n; Select w such that Follows(8,9)";   // true, boundary
			query = "stmt s; assign a; while w; constant c; prog_line n; Select n such that Follows(9,10)";  // false, 13 non existent
			query = "stmt s; assign a; while w; constant c; prog_line n; Select n such that Follows(3,2)";	 // false, not in sequence
			query = "stmt s; assign a; while w; constant c; prog_line n; Select a such that Follows(4,5)";	 // false, not in same stmtLst
			query = "stmt s; assign a; while w; constant c; prog_line n; Select a such that Follows(4,6)";	 // true, 2 conscecutive while loop
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(1,_)";   // true  
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(_,1)";	 // fail, nothing before 1
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(_,_)";   // true 
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(9,_)";   // false, nothing following last statement
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(_,9)";   // true
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(4,_)";	 // true, what follows is the next while
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(_,6)";	 // true, what followed from is the 1st while
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(s,2)";	 // true, 1
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(s,1)";   // false, nothing before 1st statement
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(1,s)";   // true, 2
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(4,s)";	 // true, 6
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(9,s)";	 // false, nothing follow last
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(s,s)";	 // false, corner case
			query = "stmt s; assign a; while w; constant c; prog_line n; Select s such that Follows(2,3)";	 // 
		}

		TEST_METHOD(testEvaluateFollowStar) {

		}

		TEST_METHOD(testEvaluateParent) {

		}

		TEST_METHOD(testEvaluateParentStar) {

		}

		TEST_METHOD(testEvaluateModifies) {

		}

		TEST_METHOD(testEvaluateUses) {

		}

		TEST_METHOD(testEvaluatePattern) {

		}
	};
}