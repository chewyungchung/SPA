#include "StatementTable.h"

using namespace std;

StatementTable::StatementTable()
{
	list<int> assignList;
	list<int> whileList;
	list<int> allStmtList;
	list<int> ifList;
}

StatementTable::~StatementTable()
{
}

void StatementTable::addStatement(int stmtNum, string stmtType)
{
	if (stmtType == "assign")
	{
		assignList.push_back(stmtNum);
		allStmtList.push_back(stmtNum);
	}
	else if (stmtType == "while")
	{
		whileList.push_back(stmtNum);
		allStmtList.push_back(stmtNum);
	}
	else if (stmtType == "if")
	{
		ifList.push_back(stmtNum);
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

list<int> StatementTable::getIfList()
{
	return ifList;
}

list<int> StatementTable::getStmtList()
{
	return allStmtList;
}

int StatementTable::getStatementCount()
{
	return allStmtList.size();
}

bool StatementTable::isValidStmt(int stmtNum)
{
	bool isFound = find(allStmtList.begin(), allStmtList.end(), stmtNum) != allStmtList.end();
	if (isFound)
	{
		return true;
	}
	else
	{
		return false;
	}
}