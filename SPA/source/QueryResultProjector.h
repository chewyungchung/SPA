#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

#include "QueryTable.h"
#include "ResultTable.h"

using namespace std;

class QueryResultProjector {
public:
	QueryResultProjector();
	QueryResultProjector(vector<vector<ResultTable>> connected_group_intermediate_results, vector<vector<ResultTable>> non_connected_group_intermediate_results, Clause select_clause);
	list<string> GetResults();

private:
	bool IsBooleanSelected();
	bool IsResultsEmpty();
	void ProcessNonConnectedResults();
	void ProcessConnectedResults();
	bool HasFalseResult(vector<vector<ResultTable>>& results);
	void InnerJoinGroups(vector<ResultTable>& joined_tables, bool is_joining_connected_groups);
	void CartesianProductGroups(vector<ResultTable>& joined_tables, ResultTable& final_table);
	ResultTable InnerJoin(ResultTable table_one, ResultTable table_two);
	ResultTable CartesianProduct(ResultTable table_one, ResultTable table_two);
	string GetCommonColumn(vector<string> table_one_columns, vector<string> table_two_columns);
	int GetNumOfCommonColumn(vector<string> table_one_columns, vector<string> table_two_columns);
	string GetOtherColumn(vector<string> table_two_columns, string common_column);
	void MergeRows(vector<string>& row_one, vector<string>& row_two);
	string GetRowInString(vector<string>& row);
	void PopulateFinalResultList(ResultTable& final_table);

	vector<vector<ResultTable>> connected_group_intermediate_results_;
	vector<vector<ResultTable>> non_connected_group_intermediate_results_;
	Clause select_clause_;
	list<string> final_results_;
	bool is_stop_evaluating = false;
};