#include "stdafx.h"
#include "CppUnitTest.h"
#include "QueryResultProjector.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTesting {
	TEST_CLASS(TestQueryResultProjector) {
		TEST_METHOD(TestInnerJoin) {
			// Some setup
			vector<string> select_arg = { "s" };
			vector<string> select_arg_type = { "stmt" };
			Clause select_clause("select", select_arg, select_arg_type);

			ResultTable test_table1;
			ResultTable test_table2;
			ResultTable test_table3;
			ResultTable joined_table;
			vector<ResultTable> group_intermediate_result;
			vector<vector<ResultTable>> all_intermediate_result;
			vector<string> row_data;

			QueryResultProjector qrp;

			/* Test join table_two with column_count = 2, and one common column*/
			test_table1.InsertNewColumn("s");
			test_table1.InsertNewColumn("a");
			test_table1.InsertNewColumn("n1");
			row_data = { "1", "2", "11" };
			test_table1.InsertRow(row_data);
			row_data = { "2", "4", "12" };
			test_table1.InsertRow(row_data);
			row_data = { "3", "6", "13" };
			test_table1.InsertRow(row_data);
			row_data = { "4", "8", "14" };
			test_table1.InsertRow(row_data);
			row_data = { "5", "10", "15" };
			test_table1.InsertRow(row_data);

			test_table2.InsertNewColumn("a");
			test_table2.InsertNewColumn("v");
			row_data = { "2", "x" };
			test_table2.InsertRow(row_data);
			row_data = { "2", "y" };
			test_table2.InsertRow(row_data);
			row_data = { "4", "z" };
			test_table2.InsertRow(row_data);
			row_data = { "6", "b" };
			test_table2.InsertRow(row_data);
			row_data = { "6", "c" };
			test_table2.InsertRow(row_data);

			test_table3.InsertNewColumn("v");
			row_data = { "x" };
			test_table3.InsertRow(row_data);

			test_table1.SetIsQueryTrue(true);
			test_table2.SetIsQueryTrue(false);
			test_table3.SetIsQueryTrue(true);

			group_intermediate_result.push_back(test_table1);
			group_intermediate_result.push_back(test_table2);
			group_intermediate_result.push_back(test_table3);

			all_intermediate_result.push_back(group_intermediate_result);

			qrp = QueryResultProjector(all_intermediate_result, select_clause);
			joined_table = qrp.InnerJoin(test_table1, test_table2);
			list<string> final_results = qrp.GetResults();

			/* Test join table_two with column_count = 1, and one common column*/
			test_table1 = ResultTable();
			test_table2 = ResultTable();

			test_table1.InsertNewColumn("s");
			test_table1.InsertNewColumn("a");
			test_table1.InsertNewColumn("n1");
			row_data = { "1", "2", "11" };
			test_table1.InsertRow(row_data);
			row_data = { "2", "4", "12" };
			test_table1.InsertRow(row_data);
			row_data = { "3", "6", "13" };
			test_table1.InsertRow(row_data);
			row_data = { "4", "8", "14" };
			test_table1.InsertRow(row_data);
			row_data = { "5", "10", "15" };
			test_table1.InsertRow(row_data);

			test_table2.InsertNewColumn("a");
			row_data = { "2"};
			test_table2.InsertRow(row_data);
			row_data = { "5"};
			test_table2.InsertRow(row_data);
			row_data = { "4"};
			test_table2.InsertRow(row_data);
			row_data = { "6"};
			test_table2.InsertRow(row_data);
			row_data = { "8"};
			test_table2.InsertRow(row_data);

			joined_table = qrp.InnerJoin(test_table1, test_table2);

			/*Test join table_two with column_count = 2, and 2 common column*/
			test_table1 = ResultTable();
			test_table2 = ResultTable();

			test_table1.InsertNewColumn("s");
			test_table1.InsertNewColumn("a");
			test_table1.InsertNewColumn("n1");
			row_data = { "1", "2", "11" };
			test_table1.InsertRow(row_data);
			row_data = { "2", "4", "12" };
			test_table1.InsertRow(row_data);
			row_data = { "3", "6", "13" };
			test_table1.InsertRow(row_data);
			row_data = { "4", "8", "14" };
			test_table1.InsertRow(row_data);
			row_data = { "5", "10", "15" };
			test_table1.InsertRow(row_data);

			test_table2.InsertNewColumn("a");
			test_table2.InsertNewColumn("s");
			row_data = { "2", "1" };
			test_table2.InsertRow(row_data);
			row_data = { "6", "3" };
			test_table2.InsertRow(row_data);
			row_data = { "8", "4" };
			test_table2.InsertRow(row_data);
			row_data = { "10", "5" };
			test_table2.InsertRow(row_data);
			row_data = { "6", "c" };
			test_table2.InsertRow(row_data);

			joined_table = qrp.InnerJoin(test_table1, test_table2);

			/*Test join table_two with more entries than table_one, and 1 common column*/
			test_table1 = ResultTable();
			test_table2 = ResultTable();

			test_table1.InsertNewColumn("s");
			test_table1.InsertNewColumn("a");
			test_table1.InsertNewColumn("n1");
			row_data = { "1", "2", "11" };
			test_table1.InsertRow(row_data);
			row_data = { "2", "4", "12" };
			test_table1.InsertRow(row_data);

			test_table2.InsertNewColumn("a");
			test_table2.InsertNewColumn("s2");
			row_data = { "2", "1" };
			test_table2.InsertRow(row_data);
			row_data = { "4", "3" };
			test_table2.InsertRow(row_data);
			row_data = { "8", "4" };
			test_table2.InsertRow(row_data);
			row_data = { "10", "5" };
			test_table2.InsertRow(row_data);
			row_data = { "6", "c" };
			test_table2.InsertRow(row_data);

			joined_table = qrp.InnerJoin(test_table1, test_table2);
		}
	};
}