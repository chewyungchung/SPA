#pragma once

#include "CallsTable.h"

CallsTable::CallsTable()
{
	callsTable = unordered_map<string, list<string>>();
}

CallsTable::~CallsTable()
{
	callsTable.clear();
}

void CallsTable::addCalls(string caller, string callee)
{
	unordered_map<string, list<string>>::iterator it = callsTable.find(caller);
	list<string> calleeList;

	if (it != callsTable.end())
	{
		calleeList = it->second;
		bool foundDup = find(calleeList.begin(), calleeList.end(), callee) != calleeList.end();
		if (!foundDup)
		{
			it->second.push_back(callee);
		}
	}
	else
	{
		calleeList.push_back(callee);
		callsTable.insert(pair<string, list<string>>(caller, calleeList));
	}
}

list<string> CallsTable::getCallees(string caller)
{
	unordered_map<string, list<string>>::iterator it = callsTable.find(caller);

	if (it != callsTable.end())
	{
		return it->second;
	}
	else
	{
		return list<string>();
	}
}

list<string> CallsTable::getCallers(string callee)
{
	list<string> callerList = list<string>();

	for (unordered_map<string, list<string>>::iterator callerIndex = callsTable.begin(); callerIndex != callsTable.end(); ++callerIndex)
	{
		for (list<string>::iterator calleeIndex = callerIndex->second.begin(); calleeIndex != callerIndex->second.end(); ++calleeIndex)
		{
			if (*calleeIndex == callee)
			{
				callerList.push_back(callerIndex->first);
			}

		}
	}
	return callerList;
}

bool CallsTable::isCallsEmpty()
{
	return callsTable.empty();
}

bool CallsTable::isCalls(string caller, string callee)
{
	unordered_map<string, list<string>>::iterator it = callsTable.find(caller);

	if (it != callsTable.end())
	{
		list<string> calleeListIt = it->second;
		bool isCalleeFound = find(calleeListIt.begin(), calleeListIt.end(), callee) != calleeListIt.end();
		return isCalleeFound;
	}
	else
	{
		return false;
	}
}

bool CallsTable::isCallsStar(string caller, string callee)
{
	callsStarHelperChecked = list<string>();
	return isCallsStarHelper(caller, callee);
}

bool CallsTable::isCallsStarHelper(string caller, string callee)
{
	unordered_map<string, list<string>>::iterator it = callsTable.find(caller);

	if (it != callsTable.end())
	{
		list<string> directCalleeList = it->second;

		// Mark the current caller as checked
		callsStarHelperChecked.push_back(caller);

		for (list<string>::iterator calleeIt = directCalleeList.begin(); calleeIt != directCalleeList.end(); ++calleeIt)
		{
			// firstly if the current procedure is the original callee, then return true immediately
			if (*calleeIt == callee)
			{
				return true;
			}
			else // otherwise we have to calls* into this procedure
			{
				bool checkedBefore = find(callsStarHelperChecked.begin(), callsStarHelperChecked.end(), *calleeIt) != callsStarHelperChecked.end();
				if (!checkedBefore)
				{
					return isCallsStarHelper(*calleeIt, callee);
				}
			}
		}
		return false;
	}
	else
	{
		return false;
	}
}

list<string> CallsTable::getCalleesStar(string caller)
{
	getCalleesStarHelperList = list<string>();
	getCalleesStarHelper(caller);
	return getCalleesStarHelperList;
}

/* DFS method of searching for callees */
void CallsTable::getCalleesStarHelper(string caller)
{
	unordered_map<string, list<string>>::iterator it = callsTable.find(caller);

	if (it != callsTable.end())
	{
		list<string> directCalleeList = it->second;

		for (list<string>::iterator calleeIt = directCalleeList.begin(); calleeIt != directCalleeList.end(); ++calleeIt)
		{
			bool checkedBefore = find(getCalleesStarHelperList.begin(), getCalleesStarHelperList.end(), *calleeIt) != getCalleesStarHelperList.end();
			if (!checkedBefore)
			{
				// Push this callee into the list and search into it
				getCalleesStarHelperList.push_back(*calleeIt);
				getCalleesStarHelper(*calleeIt);
			}
		}
	}
	else
	{
		return;
	}
}

list<string> CallsTable::getCallersStar(string callee)
{
	getCallersStarHelperList = list<string>();
	getCallersStarHelper(callee);
	return getCallersStarHelperList;
}

/* DFS search for all Callers, (because dont have a callee-keyed table, runtime here could be bad, pending further testing */
void CallsTable::getCallersStarHelper(string callee)
{
	for (unordered_map<string, list<string>>::iterator callerIndex = callsTable.begin(); callerIndex != callsTable.end(); ++callerIndex)
	{
		for (list<string>::iterator calleeIndex = callerIndex->second.begin(); calleeIndex != callerIndex->second.end(); ++calleeIndex)
		{
			if (*calleeIndex == callee)
			{
				bool checkedBefore = find(getCallersStarHelperList.begin(), getCallersStarHelperList.end(), *callerIndex) != getCallersStarHelperList.end();
				if (!checkedBefore)
				{
					// Push this caller into the list and search into it
					getCallersStarHelperList.push_back(callerIndex->first);
					getCallersStarHelper(callerIndex->first);
				}
			}
		}
	}
}