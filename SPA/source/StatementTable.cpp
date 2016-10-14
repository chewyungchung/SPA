#include "StatementTable.h"
#include <assert.h>

using namespace std;

StatementTable::StatementTable()
{
	list<int> assignList;
	list<int> whileList;
	list<int> allStmtList;
	list<int> ifList;
	unordered_map<int, string> ctrlvarList;
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
	else {
		assert(false);
	}
}

void StatementTable::addStatement(int stmtNum, string stmtType, string ctrlvar)
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
		ctrlvarList.insert(pair<int, string>(stmtNum, ctrlvar));
	}
	else if (stmtType == "if")
	{
		ifList.push_back(stmtNum);
		allStmtList.push_back(stmtNum);
		ctrlvarList.insert(pair<int, string>(stmtNum, ctrlvar));
	}
	else {
		assert(false);
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

list<int> StatementTable::getIfListWithControlVariable(string ctrlvar) {
	list<int> iflist;
	for (const auto& i : ctrlvarList) {
		if (i.second == ctrlvar && (find(ifList.begin(), ifList.end(), ctrlvar) != ifList.end())) {
			iflist.push_back(i.first);
		}
	}
	return iflist;
}

list<int> StatementTable::getWhileListWithControlVariable(string ctrlvar) {
	list<int> whilelist;
	for (const auto& i : ctrlvarList) {
		if (i.second == ctrlvar && (find(whileList.begin(), whileList.end(), ctrlvar) != whileList.end())) {
			whilelist.push_back(i.first);
		}
	}
	return whilelist;
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