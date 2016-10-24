#include "QueryResultProjector.h"

QueryResultProjector::QueryResultProjector()
{
}

QueryResultProjector::QueryResultProjector(vector<vector<ResultTable>> connected_group_intermediate_results, vector<vector<ResultTable>> non_connected_group_intermediate_results, Clause select_clause)
{
	connected_group_intermediate_results_ = connected_group_intermediate_results;
	non_connected_group_intermediate_results_ = non_connected_group_intermediate_results;
	select_clause_ = select_clause;
}

// Pre-cond: All clauses will be in non-connected groups if result-cl is "BOOLEAN"
list<string> QueryResultProjector::GetResults()
{
	if (IsBooleanSelected() == true && IsResultsEmpty() == true) {
		final_results_.push_back("true");
		return final_results_;
	}

	if (IsBooleanSelected() == true) {
		ProcessNonConnectedResults();
	}
	else {
		ProcessNonConnectedResults();
		ProcessConnectedResults();
	}

	final_results_.sort();
	final_results_.unique();
	return final_results_;
}

bool QueryResultProjector::IsBooleanSelected()
{
	string selected_arg_type = select_clause_.GetArgType().at(0);
	if (selected_arg_type == "BOOLEAN") {
		return true;
	}
	return false;
}

bool QueryResultProjector::IsResultsEmpty()
{
	if (connected_group_intermediate_results_.empty() == true && non_connected_group_intermediate_results_.empty() == true) {
		return true;
	}
	return false;
}

void QueryResultProjector::ProcessNonConnectedResults()
{
	if (HasFalseResult(non_connected_group_intermediate_results_) == true) {
		if (IsBooleanSelected() == true) {
			final_results_.push_back("false");
			return;
		}
	}
	else {
		// Perform inner join within each non-connected group and check if they are empty
		vector<ResultTable> joined_tables;
		ResultTable joined_table;
		InnerJoinGroups(joined_tables, false);
	}
}

void QueryResultProjector::ProcessConnectedResults()
{
	if (is_stop_evaluating == true) {
		return;
	}

	if (HasFalseResult(connected_group_intermediate_results_)) {
		if (IsBooleanSelected() == true) {
			final_results_.push_back("false");
		}
		return;
	}

	vector<ResultTable> joined_tables;
	ResultTable final_table;
	InnerJoinGroups(joined_tables, true);
	CartesianProductGroups(joined_tables, final_table);
	PopulateFinalResultList(final_table);
}
bool QueryResultProjector::HasFalseResult(vector<vector<ResultTable>>& results)
{
	for (auto &result_set : results) {
		for (auto &result : result_set) {
			if (result.IsQueryTrue() == false) {
				return true;
			}
		}
	}
	return false;
}

void QueryResultProjector::InnerJoinGroups(vector<ResultTable>& joined_tables, bool is_joining_connected_groups)
{
	ResultTable joined_table;
	vector<vector<ResultTable>> *groups;
	bool is_join_success = true;

	// Decide target groups to inner join
	if (is_joining_connected_groups == true) {
		groups = &connected_group_intermediate_results_;
	}
	else {
		groups = &non_connected_group_intermediate_results_;
	}

	// Perform the inner join for the target groups
	for (auto &intermediate_result_set : *groups) {
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

		if (joined_table.GetTableHeight() == 0) {
			is_join_success = false;
			is_stop_evaluating = true;
			break;
		}
		else {
			joined_tables.push_back(joined_table);
		}
	}

	// If joins are successful, and result-cl is "BOOLEAN", set appropriate results
	if (IsBooleanSelected() == true) {
		if (is_join_success == true) {
			final_results_.push_back("true");
		}
		else {
			final_results_.push_back("false");
		}
	}
}

void QueryResultProjector::CartesianProductGroups(vector<ResultTable>& joined_tables, ResultTable & final_table)
{
	bool is_first_product = true;
	for (auto &table : joined_tables) {
		if (is_first_product == true) {
			final_table = table;
			is_first_product = false;
		}
		else {
			final_table = CartesianProduct(final_table, table);
		}
	}
}
// Pre-cond: Both tables are non-empty
ResultTable QueryResultProjector::InnerJoin(ResultTable table_one, ResultTable table_two)
{
	vector<string> table_one_columns = table_one.GetColumnNames();
	vector<string> table_two_columns = table_two.GetColumnNames();
	ResultTable joined_table;
	
	joined_table.InsertColumns(table_two_columns);

	if (table_two.GetColumnCount() == 1) {
		string common_column = GetCommonColumn(table_one_columns, table_two_columns);
		for (int i = 0; i < table_two.GetTableHeight(); ++i) {
			for (int j = 0; j < table_one.GetTableHeight(); ++j) {
				if (table_two.GetValue(common_column, i) == table_one.GetValue(common_column, j)) {
					joined_table.InsertRow(table_one.GetRow(j));
				}
			}
		}
	}
	else {
		vector<string> temp_row_data;
		int common_syn_count = GetNumOfCommonColumn(table_one_columns, table_two_columns);
		if (common_syn_count == 1) {
			string common_column = GetCommonColumn(table_one_columns, table_two_columns);
			string new_column = GetOtherColumn(table_two_columns, common_column);
			joined_table.InsertNewColumn(new_column);
			for (int i = 0; i < table_one.GetTableHeight(); ++i) {
				for (int j = 0; j < table_two.GetTableHeight(); ++j) {
					if (table_one.GetValue(common_column,i) == table_two.GetValue(common_column,j)) {
						temp_row_data = table_one.GetRow(i);
						temp_row_data.push_back(table_two.GetValue(new_column, j));
						joined_table.InsertRow(temp_row_data);
					}
				}
			}
		}
		else if (common_syn_count == 2) {
			string common_syn_one = table_two_columns.at(0);
			string common_syn_two = table_two_columns.at(1);
			for (int i = 0; i < table_two.GetTableHeight(); ++i) {
				for (int j = 0; j < table_one.GetTableHeight(); ++j) {
					if ((table_two.GetValue(common_syn_one, i) == table_one.GetValue(common_syn_one, j))
						&& (table_two.GetValue(common_syn_two, i) == table_one.GetValue(common_syn_two, j))) {
						joined_table.InsertRow(table_one.GetRow(j));
					}
				}
			}
		}
	}

	return joined_table;
}

ResultTable QueryResultProjector::CartesianProduct(ResultTable table_one, ResultTable table_two)
{
	vector<string> table_one_columns = table_one.GetColumnNames();
	vector<string> table_two_columns = table_two.GetColumnNames();
	ResultTable joined_table;

	joined_table.InsertColumns(table_one_columns);
	joined_table.InsertColumns(table_two_columns);

	for (int i = 0; i < table_one.GetTableHeight(); ++i) {
		for (int j = 0; j < table_two.GetTableHeight(); ++j) {
			vector<string> table_one_row = table_one.GetRow(i);
			vector<string> table_two_row = table_two.GetRow(j);
			MergeRows(table_one_row, table_two_row);
			joined_table.InsertRow(table_one_row);
		}
	}

	return joined_table;
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

void QueryResultProjector::MergeRows(vector<string>& row_one, vector<string>& row_two)
{
	for (auto &element : row_two) {
		row_one.push_back(element);
	}
}

string QueryResultProjector::GetRowInString(vector<string>& row)
{
	string result = "";
	for (unsigned i = 0; i < row.size(); ++i) {
		result += row.at(i);
		if (i != row.size() - 1) {
			result += " ";
		}
	}
	return result;
}

void QueryResultProjector::PopulateFinalResultList(ResultTable& final_table)
{
	if (select_clause_.GetArg().size() > 1) {
		vector<ResultTable> permuted_final_results;
		for (auto &selected_syn : select_clause_.GetArg()) {
			ResultTable temp(selected_syn);
			vector<string> temp_row;
			set<string> unique_column_entries;
			for (int i = 0; i < final_table.GetTableHeight(); ++i) {
				unique_column_entries.insert(final_table.GetValue(selected_syn, i));
			}

			for (auto &unique_entry : unique_column_entries) {
				temp_row.push_back(unique_entry);
				temp.InsertRow(temp_row);
				temp_row.clear();
			}

			permuted_final_results.push_back(temp);
		}

		// Cart product
		CartesianProductGroups(permuted_final_results, final_table);

		// Ready to get all results
		for (int i = 0; i < final_table.GetTableHeight(); ++i) {
			string final_result_string = GetRowInString(final_table.GetRow(i));
			final_results_.push_back(final_result_string);
		}
	}
	else {
		for (int i = 0; i < final_table.GetTableHeight(); ++i) {
			string selected_syn = select_clause_.GetArg().at(0);
			final_results_.push_back(final_table.GetValue(selected_syn, i));
		}
	}
}
