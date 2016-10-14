/*
We have 2 main structures 
1. procNameIndexTable: map from procName to internal procIndex
2. procDataTable: vector of vector of list<string> indexed corr. to procIndex:
col_1: procName; col_2: modified var, col_3: used var, col_4: called in stmtLine
*/

#pragma once

#include "ProcTable.h"

ProcTable::ProcTable()
{
	procNameIndexTable = unordered_map<string, int>();
	procDataTable = vector<vector<list<string>>>();
	maxProcIndex = 0;
}

ProcTable::~ProcTable()
{
	procNameIndexTable.clear();
	procDataTable.clear();
}

void ProcTable::addProc(string procName)
{
	procNameIndexTable.insert(pair<string, int>(procName, maxProcIndex));

	vector<list<string>> newProc(PROCDATA_COLSIZE);

	list<string> procNameList = list<string>();
	procNameList.push_back(procName);
	newProc[PROCNAME_COL] = procNameList;

	list<string> modVarList = list<string>();
	newProc[MODVAR_COL] = modVarList;

	list<string> usesVarList = list<string>();
	newProc[USEVAR_COL] = usesVarList;

	list<string> calledInStmtList = list<string>();
	newProc[CALLED_IN_STMT_COL] = calledInStmtList;
	
	procDataTable[maxProcIndex] = newProc;
	maxProcIndex++;
}

void ProcTable::addProcMod(string procName, string var)
{
	unordered_map<string, int>::iterator it = procNameIndexTable.find(procName);
	list<string> modVarList;

	if (it != procNameIndexTable.end())
	{
		modVarList = procDataTable[it->second][MODVAR_COL];
		bool foundDup = find(modVarList.begin(), modVarList.end(), var) != modVarList.end();
		if (!foundDup)
		{
			procDataTable[it->second][MODVAR_COL].push_back(var);
		}
	}
}

void ProcTable::addProcUses(string procName, string var)
{
	unordered_map<string, int>::iterator it = procNameIndexTable.find(procName);
	list<string> usesVarList;

	if (it != procNameIndexTable.end())
	{
		usesVarList = procDataTable[it->second][USEVAR_COL];
		bool foundDup = find(usesVarList.begin(), usesVarList.end(), var) != usesVarList.end();
		if (!foundDup)
		{
			procDataTable[it->second][USEVAR_COL].push_back(var);
		}
	}
}

void ProcTable::addProcCalledInStmt(string procName, int stmtLine)
{
	unordered_map<string, int>::iterator it = procNameIndexTable.find(procName);
	list<string> calledInVarList;

	if (it != procNameIndexTable.end())
	{
		calledInVarList = procDataTable[it->second][CALLED_IN_STMT_COL];
		bool foundDup = find(calledInVarList.begin(), calledInVarList.end(), to_string(stmtLine)) != calledInVarList.end();
		if (!foundDup)
		{
			procDataTable[it->second][USEVAR_COL].push_back(to_string(stmtLine));
		}
	}
}

// Return empty string if procIndex does not exist
string ProcTable::getProcName(int procIndex)
{
	string procName = "";
	for (unordered_map<string, int>::iterator it = procNameIndexTable.begin(); it != procNameIndexTable.end(); ++it)
	{
		if (it->second == procIndex)
		{
			procName = it->first;
			return procName;
		}
	}
	return procName;
}

// Return -1 if procName does not exist
int ProcTable::getProcIndex(string procName)
{
	int procIndex = -1;
	unordered_map<string, int>::iterator it = procNameIndexTable.find(procName);

	if (it != procNameIndexTable.end())
	{
		procIndex = it->second;
		return procIndex;
	}
	else
	{
		return procIndex;
	}
}

// Also return false if procName doesn not exist
bool ProcTable::isModifiedByProc(string procName, string varName)
{
	unordered_map<string, int>::iterator it = procNameIndexTable.find(procName);
	list<string> modVarList;
	
	if (it != procNameIndexTable.end())
	{
		modVarList = procDataTable[it->second][MODVAR_COL];
		return find(modVarList.begin(), modVarList.end(), varName) != modVarList.end();
	}
	else
	{
		return false;
	}
}

bool ProcTable::isUsedByProc(string procName, string varName)
{
	unordered_map<string, int>::iterator it = procNameIndexTable.find(procName);
	list<string> usesVarList;

	if (it != procNameIndexTable.end())
	{
		usesVarList = procDataTable[it->second][USEVAR_COL];
		return find(usesVarList.begin(), usesVarList.end(), varName) != usesVarList.end();
	}
	else
	{
		return false;
	}
}

list<string> ProcTable::getModifiedByProc(string procName)
{
	unordered_map<string, int>::iterator it = procNameIndexTable.find(procName);

	if (it != procNameIndexTable.end())
	{
		return procDataTable[it->second][MODVAR_COL];
	}
	else
	{
		return list<string>();
	}
}

list<string> ProcTable::getusedByProc(string procName)
{
	unordered_map<string, int>::iterator it = procNameIndexTable.find(procName);

	if (it != procNameIndexTable.end())
	{
		return procDataTable[it->second][USEVAR_COL];
	}
	else
	{
		return list<string>();
	}
}

list<string> ProcTable::getProcedureModifying(string varName)
{
	list<string> results = list<string>();

	for (int i = 0; i < procDataTable.size(); i++)
	{
		list<string> currModVarList = procDataTable[i][MODVAR_COL];
		bool foundVar = find(currModVarList.begin(), currModVarList.end(), varName) != currModVarList.end();
		if (foundVar)
		{
			results.push_back(procDataTable[i][PROCNAME_COL].back());
		}
	}
	return results;
}

list<string> ProcTable::getProcedureUsing(string varName)
{
	list<string> results = list<string>();

	for (int i = 0; i < procDataTable.size(); i++)
	{
		list<string> currUsesVarList = procDataTable[i][USEVAR_COL];
		bool foundVar = find(currUsesVarList.begin(), currUsesVarList.end(), varName) != currUsesVarList.end();
		if (foundVar)
		{
			results.push_back(procDataTable[i][PROCNAME_COL].back());
		}
	}
	return results;
}

list<string> ProcTable::getCalledProcNamesList()
{
	list<string> results = list<string>();

	for (int i = 0; i < procDataTable.size(); i++)
	{
		list<string> currCalledInStmtList = procDataTable[i][CALLED_IN_STMT_COL];
		if (!currCalledInStmtList.empty())
		{
			results.push_back(procDataTable[i][PROCNAME_COL].back());
		}
	}
	return results;
}

list<string> ProcTable::getProcedureList()
{
	list<string> results = list<string>();
	unordered_map<string, int>::iterator it;
	for (it = procNameIndexTable.begin(); it != procNameIndexTable.end(); ++it)
	{
		results.push_back(it->first);
	}
	return results;
}