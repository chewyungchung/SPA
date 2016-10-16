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
	QueryResultProjector(vector<vector<ResultTable>> intermediate_results, Clause select_clause);

	list<string> GetResults();
	void ProcessIntermediateResults();
	ResultTable InnerJoin(ResultTable table_one, ResultTable table_two);
	ResultTable CartesianProduct(ResultTable intermediate_set_one, ResultTable intermediate_set_two);
	bool HasFalseResult();
	int GetNumOfCommonColumn(vector<string> table_one_columns, vector<string> table_two_columns);
	string GetCommonColumn(vector<string> table_one_columns, vector<string> table_two_columns);
	string GetOtherColumn(vector<string> table_two_columns, string common_column);
	void PopulateFinalResultList(ResultTable& final_table, string selected_syn);

private:
	vector<vector<ResultTable>> intermediate_results_;
	Clause select_clause_;
	list<string> final_results_;
};