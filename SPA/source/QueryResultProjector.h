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
	void InnerJoin(ResultTable instance_one, ResultTable instance_two);
	void CartesianProduct(ResultTable intermediate_set_one, ResultTable intermediate_set_two);

private:
	vector<vector<ResultTable>> intermediate_results_;
	Clause select_clause_;
	list<string> final_results_;
};