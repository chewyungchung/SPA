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

	int GetSynonymColumnIndex(string synonym);
	int GetTableHeight();
	string GetValue(string synonym, int row_index);
	void InsertRow(vector<string> row_data);
	void InsertNewColumn(string synonym);
	void SetIsQueryTrue(bool is_query_true);
	bool IsQueryTrue();

private:
	unordered_map<string, int> synonym_to_column_map_;
	vector<string> column_names_;
	vector<vector<string>> row_data_;

	bool is_query_true_ = false; // Default to false
	int table_height_ = 0;
	int row_width_ = 0;
	int next_column_index = 0;

};