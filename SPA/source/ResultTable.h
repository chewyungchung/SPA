#pragma once

#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class ResultTable {
public:
	ResultTable();
	ResultTable(string synonym); // For query that has 1 synonym
	ResultTable(string synonym_one, string synonym_two); // For query that has 2 synonyms

	/* Getters */
	int GetTableHeight();
	int GetColumnCount();
	int GetSynonymColumnIndex(string synonym);
	vector<string> GetRow(int row_index);
	vector<string> GetColumn(string column_name);
	vector<string> GetColumnNames();
	string GetValue(string synonym, int row_index);
	bool IsQueryTrue();

	/* Setters */
	void SetIsQueryTrue(bool is_query_true);
	void InsertRow(vector<string> row_data);
	void InsertNewColumn(string synonym);
	void InsertColumns(vector<string> columns);

private:
	unordered_map<string, int> synonym_to_column_map_;
	vector<string> column_names_;
	vector<vector<string>> row_data_;
	bool is_query_true_ = false;
	int table_height_ = 0;
	int row_width_ = 0;
	int next_column_index = 0;

};