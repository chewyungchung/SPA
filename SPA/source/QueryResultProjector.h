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
	void InnerJoin(ResultTable instance_one, ResultTable instance_two);

private:
	vector<vector<ResultTable>> intermediate_results_;
	Clause select_clause_;
};