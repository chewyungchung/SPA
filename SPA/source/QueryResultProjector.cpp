#include "QueryResultProjector.h"

QueryResultProjector::QueryResultProjector() 
{
}

QueryResultProjector::QueryResultProjector(vector<ResultTable> connected_groups_results, Clause select_clause) 
{
	connected_groups_results_ = connected_groups_results;
	select_clause_ = select_clause;
}


// Pre-cond: All clauses will be in non-connected groups if result-cl is "BOOLEAN"
list<string> QueryResultProjector::GetResults() 
{
	ProcessConnectedResultsOptimized();
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

void QueryResultProjector::ProcessConnectedResultsOptimized() 
{
	if (connected_groups_results_.empty() == true) {
		if (IsBooleanSelected() == true) {
			final_results_.push_back("true");
		}
	}
	else {
		ResultTable final_table;
		CartesianProductGroups(connected_groups_results_, final_table);
		PopulateFinalResultList(final_table);
	}
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

ResultTable QueryResultProjector::CartesianProduct(ResultTable& table_one, ResultTable& table_two) 
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
	for (int i = 0; i < final_table.GetTableHeight(); ++i) {
		string result_string = "";
		for (unsigned j = 0; j < select_clause_.GetArg().size(); ++j) {
			if (select_clause_.GetArgType().at(j) == "call" || select_clause_.GetArgType().at(j) == "call_number") {
				int index_of_space = final_table.GetValue(select_clause_.GetArg().at(j), i).find_first_of(" ");
				result_string += final_table.GetValue(select_clause_.GetArg().at(j), i).substr(0, index_of_space);
			}
			else if (select_clause_.GetArgType().at(j) == "call_name") {
				int index_of_space = final_table.GetValue(select_clause_.GetArg().at(j), i).find_first_of(" ");
				result_string += final_table.GetValue(select_clause_.GetArg().at(j), i).substr(index_of_space+1, string::npos);
			} 
			else {
				result_string += final_table.GetValue(select_clause_.GetArg().at(j), i);
			}

			if (j != select_clause_.GetArg().size() - 1) {
				result_string += " ";
			}
		}
		final_results_.push_back(result_string);
	}
}
