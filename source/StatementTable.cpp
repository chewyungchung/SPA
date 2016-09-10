#include "StatementTable.h"

using namespace std;

StatementTable::StatementTable()
{
	list<int> assignList;
	list<int> whileList;
	list<int> allStmtList;
}

StatementTable::~StatementTable()
{
}

void StatementTable::addStatement(int stmtNum, string stmtType)
{
	if (stmtType == "ASSIGN") {
		assignList.push_back(stmtNum);
		allStmtList.push_back(stmtNum);
	}
	else if (stmtType == "WHILE") {
		whileList.push_back(stmtNum);
		allStmtList.push_back(stmtNum);
	}
}

list<int> StatementTable::getAssignList()
{
	return assignList;
}

list<int> StatementTable::getWhileList()
{
	return whileList;
}

list<int> StatementTable::getStmtList()
{
	return allStmtList;
}

int StatementTable::getStatementCount()
{
	return allStmtList.size();
}



