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
	ProcessIntermediateResults();
	return final_results_;
}

void QueryResultProjector::ProcessIntermediateResults()
{
	string selected_arg_type = select_clause_.GetArgType().at(0);
	if (selected_arg_type == "BOOLEAN") {
		for (auto &intermediate_result_set : intermediate_results_) {
			for (auto &intermediate_result : intermediate_result_set) {
				if (intermediate_result.IsQueryTrue() == false) {
					final_results_.push_back("false");
					return;
				}
			}
		}
		final_results_.push_back("true");
	}
	else {
		// Perform inner join

		// Perform cartesian product between intermediate result set
	}
}

void QueryResultProjector::InnerJoin(ResultTable instance_one, ResultTable instance_two)
{
}

void QueryResultProjector::CartesianProduct(ResultTable intermediate_set_one, ResultTable intermediate_set_two)
{
}
