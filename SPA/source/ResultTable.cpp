#include "ResultTable.h"

ResultTable::ResultTable() {

}

ResultTable::ResultTable(string synonym)
{
}

ResultTable::ResultTable(string synonym_one, string synonym_two)
{
}

int ResultTable::GetSynonymColumnIndex(string synonym) {
	
}

int ResultTable::GetTableHeight() {
	
}

void ResultTable::InsertNewColumn(string synonym, vector<string> column_content) {

}

void ResultTable::SetIsQueryTrue(bool is_query_true)
{

}

bool ResultTable::IsQueryTrue()
{
	return false;
}

void ResultTable::DeleteRow(int index) {

}

void ResultTable::InsertRow(vector<string> row_data)
{
}

void ResultTable::InsertNewSynonymInMap(string syn, int column_index) {

}