#include "ResultTable.h"

ResultTable::ResultTable() {
	next_column_index = 0;
}

ResultTable::ResultTable(string synonym)
{
	synonym_to_column_map_[synonym] = 0;
	column_names_.push_back(synonym);
	next_column_index = 1;
}

ResultTable::ResultTable(string synonym_one, string synonym_two)
{
	synonym_to_column_map_[synonym_one] = 0;
	synonym_to_column_map_[synonym_two] = 1;
	column_names_.push_back(synonym_one);
	column_names_.push_back(synonym_two);
	next_column_index = 2;
}

int ResultTable::GetTableHeight() {
	return table_height_;
}

int ResultTable::GetColumnCount()
{
	return column_names_.size();
}

int ResultTable::GetSynonymColumnIndex(string synonym) {
	if (synonym_to_column_map_.count(synonym) > 0) {
		return synonym_to_column_map_[synonym];
	}
	else {
		return -1;
	}
}
vector<string> ResultTable::GetRow(int row_index)
{
	vector<string> empty;
	if (row_index + 1 > table_height_) {
		return empty;
	}
	else {
		return row_data_.at(row_index);
	}
}

vector<string> ResultTable::GetColumnNames()
{
	return column_names_;
}

string ResultTable::GetValue(string synonym, int row_index)
{
	string empty_string = "";

	if ((table_height_ == 0) || (synonym_to_column_map_.count(synonym) == 0) || (row_index+1 > table_height_)) {
		return empty_string;
	}

	return row_data_.at(row_index).at(synonym_to_column_map_[synonym]);
}

bool ResultTable::IsQueryTrue()
{
	return is_query_true_;
}

void ResultTable::SetIsQueryTrue(bool is_query_true)
{
	is_query_true_ = is_query_true;
}

void ResultTable::InsertNewColumn(string synonym) {
	if (synonym_to_column_map_.count(synonym) == 0) {
		column_names_.push_back(synonym);
		synonym_to_column_map_[synonym] = next_column_index;
		++next_column_index;
		++row_width_;
	}
}

void ResultTable::InsertColumns(vector<string> columns)
{
	for (auto &column_name : columns) {
		if (synonym_to_column_map_.count(column_name) == 0) {
			column_names_.push_back(column_name);
			synonym_to_column_map_[column_name] = next_column_index;
			++next_column_index;
			++row_width_;
		}
	}
}

void ResultTable::InsertRow(vector<string> row_data)
{
	row_data_.push_back(row_data);
	++table_height_;
	if (row_width_ == 0) {
		row_width_ = row_data.size();

	}
}
