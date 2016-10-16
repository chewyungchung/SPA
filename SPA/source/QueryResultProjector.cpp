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
	final_results_.sort();
	final_results_.unique();
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
	vector<string> table_one_columns = table_one.GetColumnNames();
	vector<string> table_two_columns = table_two.GetColumnNames();
	ResultTable joined_table;
	
	for (auto &table_one_column_name : table_one_columns) {
		joined_table.InsertNewColumn(table_one_column_name);
	}

	if (table_two.GetColumnCount() == 1) {
		string common_column = GetCommonColumn(table_one_columns, table_two_columns);
		for (unsigned i = 0; i < table_two.GetTableHeight(); ++i) {
			for (unsigned j = 0; j < table_one.GetTableHeight(); ++j) {
				if (table_two.GetValue(common_column, i) == table_one.GetValue(common_column, j)) {
					joined_table.InsertRow(table_one.GetRow(j));
				}
			}
		}
		return joined_table;
	}
	else {
		vector<string> temp_row_data;
		int common_syn_count = GetNumOfCommonColumn(table_one_columns, table_two_columns);
		if (common_syn_count == 1) {
			string common_column = GetCommonColumn(table_one_columns, table_two_columns);
			string new_column = GetOtherColumn(table_two_columns, common_column);
			joined_table.InsertNewColumn(new_column);
			for (unsigned i = 0; i < table_one.GetTableHeight(); ++i) {
				for (unsigned j = 0; j < table_two.GetTableHeight(); ++j) {
					if (table_one.GetValue(common_column,i) == table_two.GetValue(common_column,j)) {
						temp_row_data = table_one.GetRow(i);
						temp_row_data.push_back(table_two.GetValue(new_column, j));
						joined_table.InsertRow(temp_row_data);
					}
				}
			}
			return joined_table;
		}
		else if (common_syn_count == 2) {
			string common_syn_one = table_two_columns.at(0);
			string common_syn_two = table_two_columns.at(1);
			for (unsigned i = 0; i < table_two.GetTableHeight(); ++i) {
				for (unsigned j = 0; j < table_one.GetTableHeight(); ++j) {
					if ((table_two.GetValue(common_syn_one, i) == table_one.GetValue(common_syn_one, j))
						&& (table_two.GetValue(common_syn_two, i) == table_one.GetValue(common_syn_two, j))) {
						joined_table.InsertRow(table_one.GetRow(j));
					}
				}
			}
			return joined_table;
		}
		else {
			return joined_table;
		}
	}
}

ResultTable QueryResultProjector::CartesianProduct(ResultTable intermediate_set_one, ResultTable intermediate_set_two)
{
	return ResultTable();
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
	for (auto &column : table_two_columns) {
		if (column != common_column) {
			other_column = column;
			break;
		}
	}
	return other_column;
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
