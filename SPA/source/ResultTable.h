#pragma once

#include <vector>;
#include <string>;
#include <unordered_map>;

using namespace std;

class ResultTable {
public:
	ResultTable();
	ResultTable(string synonym); // For query that has 1 synonym
	ResultTable(string synonym_one, string synonym_two); // For query that has 2 synonyms

	int GetSynonymColumnIndex(string synonym);
	int GetTableHeight();
	void DeleteRow(int index);
	void InsertRow(vector<string> row_data);
	void InsertNewColumn(string synonym, vector<string> column_content);
	void SetIsQueryTrue(bool is_query_true);
	bool IsQueryTrue();

private:
	void InsertNewSynonymInMap(string syn, int column_index);

	unordered_map<string, int> synonym_to_column_map_;
	vector<string> column_names_;
	vector<vector<string>> row_data_;

	bool is_query_true_; // Default to false

};