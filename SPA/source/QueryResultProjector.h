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
	QueryResultProjector(vector<ResultTable> connected_group_intermediate_results, Clause select_clause);
	list<string> GetResults();

private:
	bool IsBooleanSelected();
	void ProcessConnectedResultsOptimized();
	bool HasFalseResult(vector<vector<ResultTable>>& results);
	void CartesianProductGroups(vector<ResultTable>& joined_tables, ResultTable& final_table);
	ResultTable CartesianProduct(ResultTable table_one, ResultTable table_two);
	void MergeRows(vector<string>& row_one, vector<string>& row_two);
	string GetRowInString(vector<string>& row);
	void PopulateFinalResultList(ResultTable& final_table);

	vector<ResultTable> connected_groups_results_;
	Clause select_clause_;
	list<string> final_results_;
};