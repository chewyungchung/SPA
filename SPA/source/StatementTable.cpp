#include "StatementTable.h"
#include <assert.h>

using namespace std;

StatementTable::StatementTable()
{
	list<int> assignList;
	list<int> whileList;
	list<int> allStmtList;
	list<int> ifList;
	list<int> callList;
	unordered_map<int, string> callProcList; // key: callstmt#, value: associated procName
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
	else
	{
		assert(false);
	}
}

void StatementTable::addStatement(int stmtNum, string stmtType, string ctrlVar_or_procName)
{
	if (stmtType == "assign")
	{
		assignList.push_back(stmtNum);
		allStmtList.push_back(stmtNum);
	}
	else if (stmtType == "call")
	{
		callList.push_back(stmtNum);
		allStmtList.push_back(stmtNum);
		callProcList.insert(pair<int, string>(stmtNum, ctrlVar_or_procName));
	}
	else if (stmtType == "while")
	{
		whileList.push_back(stmtNum);
		allStmtList.push_back(stmtNum);
		ctrlvarList.insert(pair<int, string>(stmtNum, ctrlVar_or_procName));
	}
	else if (stmtType == "if")
	{
		ifList.push_back(stmtNum);
		allStmtList.push_back(stmtNum);
		ctrlvarList.insert(pair<int, string>(stmtNum, ctrlVar_or_procName));
	}
	else
	{
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

list<int> StatementTable::getCallList()
{
	return callList;
}

string StatementTable::getProcNameByCallStmt(int callStmt)
{
	string result = "";
	for (const auto& i : callProcList)
	{
		if (i.first == callStmt && (find(callList.begin(), callList.end(), i.first) != callList.end()))
		{
			result = i.second;
			break;
		}
	}
	return result;
}

list<int> StatementTable::getIfListWithControlVariable(string ctrlvar)
{
	list<int> iflist;
	for (const auto& i : ctrlvarList)
	{
		if (i.second == ctrlvar && (find(ifList.begin(), ifList.end(), i.first) != ifList.end()))
		{
			iflist.push_back(i.first);
		}
	}
	return iflist;
}

list<int> StatementTable::getWhileListWithControlVariable(string ctrlvar)
{
	list<int> whilelist;
	for (const auto& i : ctrlvarList)
	{
		if (i.second == ctrlvar && (find(whileList.begin(), whileList.end(), i.first) != whileList.end()))
		{
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

bool StatementTable::isAssign(int stmtNum)
{
	return find(assignList.begin(), assignList.end(), stmtNum) != assignList.end();
}
