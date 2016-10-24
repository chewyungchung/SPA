#include "stdafx.h"
#include "CppUnitTest.h"
#include "QueryResultProjector.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTesting {
	TEST_CLASS(TestQueryResultProjector) {
		TEST_METHOD(TestGetResults) {
			// Some setup
			vector<string> select_arg = { "s1", "s2", "s3" };
			vector<string> select_arg_type = { "stmt", "stmt","stmt" };
			Clause select_clause("select", select_arg, select_arg_type);

			ResultTable test_table1, test_table2, test_table3, joined_table;
			vector<ResultTable> group_intermediate_result;
			vector<vector<ResultTable>> all_intermediate_result;
			vector<vector<ResultTable>> empty_non_connected_groups;
			vector<string> row_data;
			QueryResultProjector qrp;

			/* Test join table_two with column_count = 2, and one common column*/
			test_table1.InsertNewColumn("s1");
			test_table1.InsertNewColumn("a");
			row_data = { "1", "11"};
			test_table1.InsertRow(row_data);
			row_data = { "2", "12"};
			test_table1.InsertRow(row_data);
			row_data = { "3", "13"};
			test_table1.InsertRow(row_data);

			test_table2.InsertNewColumn("s2");
			row_data = { "1" };
			test_table2.InsertRow(row_data);
			row_data = { "2" };
			test_table2.InsertRow(row_data);
			row_data = { "3" };
			test_table2.InsertRow(row_data);

			test_table3.InsertNewColumn("s3");
			row_data = { "1" };
			test_table3.InsertRow(row_data);
			row_data = { "2" };
			test_table3.InsertRow(row_data);
			row_data = { "3" };
			test_table3.InsertRow(row_data);

			test_table1.SetIsQueryTrue(true);
			test_table2.SetIsQueryTrue(true);
			test_table3.SetIsQueryTrue(true);

			group_intermediate_result.push_back(test_table1);
			all_intermediate_result.push_back(group_intermediate_result);
			group_intermediate_result.clear();

			group_intermediate_result.push_back(test_table2);
			all_intermediate_result.push_back(group_intermediate_result);
			group_intermediate_result.clear();

			group_intermediate_result.push_back(test_table3);
			all_intermediate_result.push_back(group_intermediate_result);
			group_intermediate_result.clear();

			qrp = QueryResultProjector(all_intermediate_result, empty_non_connected_groups, select_clause);
			
			list<string> final_results = qrp.GetResults();
			cout << "All is good" << endl;
		}

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
			//_qrp = QueryResultProjector(vvrt, c);

			finalResults = _qrp.GetResults();
			size_t NUM_OF_COLUMNS_AFTER_INNER_JOIN = 3;

			Assert::AreEqual(NUM_OF_COLUMNS_AFTER_INNER_JOIN, finalResults.size(), L"FINAL RESULTS SIZE");
		}
	};
}