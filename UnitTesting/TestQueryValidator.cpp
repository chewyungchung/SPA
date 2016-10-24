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
		TEST_METHOD(TestIteration2Parsing) {
			// Brief testing. More comprehensive test to be created by Cher Lin

			string declaration = "stmt s1,s2; assign a1,a2; while w1,w2; if if1,if2; call call1,call2; constant c1,c2; prog_line n1,n2; procedure proc1, proc2; variable v1,v2;";
			string query_select = "Select s1 ";
			string query_clause = "such that Follows(s1,s2) and Follows*(s1,s2) and Parent(w1,n2) and Parent*(if1,n2) such that Call(proc1,proc2) and Call*(proc1,proc2) and Next(n1,a2) and Next*(n2, w2) and Modifies(n1,\"x\") and Modifies(proc1,v1) and Modifies(\"main\",\"var2\") and Uses(if1,\"y\") and Uses(proc2, v2) and Uses(\"main\", \"var1\")";
			string query_string = declaration + query_select + query_clause;
			QueryValidator query_parser(query_string);
			QueryTable query_tree = query_parser.Parse();

			//query_clause = "pattern if1(_,_,_) and w1(_,_) pattern if2(v1,_,_) and w2(v2,_) and a1(_,_) and a2(v1,_) and a1(_,_\"x+y\"_) and a2(v1,\"x+y\")";
			query_clause = "pattern a1(_, \"x + y * 5 + (3+2) * 9\")";
			query_string = declaration + query_select + query_clause;
			query_parser = QueryValidator(query_string);
			query_tree = query_parser.Parse();

			query_clause = "with s1.stmt# = s2.stmt# and n1 = 5 and 5 = n2 with n1 = c1.value and proc1.procName = \"main\" and \"var1\" = v1.varName and 5 = 4 and \"test\" = \"test1\" and a1.stmt# = 2 and if1.stmt# = w1.stmt# and call1.procName = \"test\" and 9 = call2.stmt#";
			query_string = declaration + query_select + query_clause;
			query_parser = QueryValidator(query_string);
			query_tree = query_parser.Parse();

			query_clause = "such that Follows(a2,5) and Follows(1,2) and Uses(5,\"y\") and Parent(if1,a1) and Parent(if2,a2) and Next(a2,n2) and Next(n2,s1) and Modifies(s1,v1) and Uses(proc1,v1) and Call(proc1,proc2) and Modifies(s1,v1) and Uses(proc1,v1) with a2.stmt#=1 and 1=0";
			query_string = declaration + query_select + query_clause;
			query_parser = QueryValidator(query_string);
			query_tree = query_parser.Parse();

			query_clause = "such that Follows(s1,s2) and Parent(s2, if1) such that Follows(a1,a2) and Parent(n1,n2) and Uses(a1,\"x\") and Modifies(a2,\"y\") with call1.procName=v1.varName and v1.varName=v2.varName such that Parent(w2,if2)";
			query_string = declaration + query_select + query_clause;
			query_parser = QueryValidator(query_string);
			query_tree = query_parser.Parse();

			query_string = "stmt s; constant c; call call1,call2; Select <s.stmt#,c.value,call1.procName,call2.stmt#> such that Follows(s,call1) and Follows*(call1,2)";
			query_parser = QueryValidator(query_string);
			query_tree = query_parser.Parse();

			query_tree.Optimize();

			cout << "All is well" << endl;
		}

		TEST_METHOD(testSyntaxError) {
			
		}

		TEST_METHOD(testValidStrangeDeclaration) {
			
		}
	};
}