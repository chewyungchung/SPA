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
	string selected_arg = select_clause_.GetArg().at(0);
	string selected_arg_type = select_clause_.GetArgType().at(0);
	bool is_false_query = HasFalseResult();

	if (is_false_query == true) {
		if (selected_arg_type == "BOOLEAN") {
			final_results_.push_back("false");
		}
		return;
	}
	else {
		if (selected_arg_type == "BOOLEAN") {
			final_results_.push_back("true");
			return;
		}

		// Perform inner join
		vector<ResultTable> joined_tables;
		ResultTable joined_table;

		for (auto &intermediate_result_set : intermediate_results_) {
			bool is_first_join = true;
			for (auto &intermediate_result : intermediate_result_set) {
				if (is_first_join == true) {
					joined_table = intermediate_result;
					is_first_join = false;
				}
				else {
					joined_table = InnerJoin(joined_table, intermediate_result);
				}
			}
			joined_tables.push_back(joined_table);
		}

		PopulateFinalResultList(joined_tables.at(0), selected_arg);
		// Perform cartesian product between intermediate result set. Required for iteration 3
	}
}

// Pre-cond: Both tables are non-empty
ResultTable QueryResultProjector::InnerJoin(ResultTable table_one, ResultTable table_two)
{
	ResultTable joined_table;
	vector<string> table_one_columns = table_one.GetColumnNames();
	vector<string> table_two_columns = table_two.GetColumnNames();

	if (table_two.GetColumnCount() == 1) {

	}

	int common_syn_count = GetNumOfCommonColumn(table_one_columns, table_one_columns);
	if (common_syn_count == 1) {
		string common_column = GetCommonColumn(table_one_columns, table_two_columns);
	}
}

ResultTable QueryResultProjector::CartesianProduct(ResultTable intermediate_set_one, ResultTable intermediate_set_two)
{
}

bool QueryResultProjector::HasFalseResult()
{
	for (auto &intermediate_result_set : intermediate_results_) {
		for (auto &intermediate_result : intermediate_result_set) {
			if (intermediate_result.IsQueryTrue() == false) {
				return true;
			}
		}
	}
	return false;
}

// Pre-cond: table_two_columns should have max 2 columns
int QueryResultProjector::GetNumOfCommonColumn(vector<string> table_one_columns, vector<string> table_two_columns)
{
	int common_count = 0;
	for (auto &column_name_two : table_two_columns) {
		for (auto &column_name_one : table_one_columns) {
			if (column_name_two == column_name_one) {
				++common_count;
				break;
			}
		}
	}
	return common_count;
}

string QueryResultProjector::GetCommonColumn(vector<string> table_one_columns, vector<string> table_two_columns)
{
	string common_column = "";
	for (auto &column_name_two : table_two_columns) {
		for (auto &column_name_one : table_one_columns) {
			if (column_name_two == column_name_one) {
				common_column = column_name_two;
				break;
			}
		}
	}
	return common_column;
}

string QueryResultProjector::GetOtherColumn(vector<string> table_two_columns, string common_column)
{
	string other_column = "";
	return string();
}

void QueryResultProjector::PopulateFinalResultList(ResultTable& final_table, string selected_syn)
{
	int column_index = final_table.GetSynonymColumnIndex(selected_syn);
	int table_height = final_table.GetTableHeight();
	if (column_index != -1 && table_height != 0) {
		for (unsigned i = 0; i < table_height; ++i) {
			final_results_.push_back(final_table.GetValue(selected_syn, i));
		}
	}
}
