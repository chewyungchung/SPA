#include "QueryResultProjector.h"

QueryResultProjector::QueryResultProjector()
{
}

QueryResultProjector::QueryResultProjector(vector<vector<ResultTable>> intermediate_results, Clause select_clause)
{
	intermediate_results_ = intermediate_results;
	select_clause_ = select_clause;
}

list<string> QueryResultProjector::GetResults()
{
	list<string> final_results;

	return final_results;
}

void QueryResultProjector::InnerJoin(ResultTable instance_one, ResultTable instance_two)
{
}
