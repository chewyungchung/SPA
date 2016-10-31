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
		allStmtList.insert(pair<int, string>(stmtNum, stmtType));
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
		allStmtList.insert(pair<int, string>(stmtNum, stmtType));
	}
	else if (stmtType == "call")
	{
		callList.push_back(stmtNum);
		allStmtList.insert(pair<int, string>(stmtNum, stmtType));
		callProcList.insert(pair<int, string>(stmtNum, ctrlVar_or_procName));
	}
	else if (stmtType == "while")
	{
		whileList.push_back(stmtNum);
		allStmtList.insert(pair<int, string>(stmtNum, stmtType));
		ctrlvarList.insert(pair<int, string>(stmtNum, ctrlVar_or_procName));
	}
	else if (stmtType == "if")
	{
		ifList.push_back(stmtNum);
		allStmtList.insert(pair<int, string>(stmtNum, stmtType));
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

string StatementTable::getControlVarWithStmt(int stmtNum)
{
	string ctrlVar = "";
	if (ctrlvarList[stmtNum] != "") {
		ctrlVar = ctrlvarList[stmtNum];
	}
	return ctrlVar;
}

list<int> StatementTable::getStmtList()
{
	list<int> results = list<int>();
	unordered_map<int, string>::iterator it;
	for (it = allStmtList.begin(); it != allStmtList.end(); ++it)
	{
		results.push_back(it->first);
	}
	return results;
}

int StatementTable::getStatementCount()
{
	return allStmtList.size();
}

bool StatementTable::isValidStmt(int stmtNum)
{
	unordered_map<int, string>::iterator it = allStmtList.find(stmtNum);
	if (it != allStmtList.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

string StatementTable::getStmtType(int stmtNum)
{
	string result = "";
	unordered_map<int, string>::iterator it = allStmtList.find(stmtNum);

	if (it != allStmtList.end())
	{
		result = it->second;
		return result;
	}
	else
	{
		return result;
	}
}
