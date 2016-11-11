#pragma once

#include "CallsTable.h"

CallsTable::CallsTable()
{
	calls_table_ = map<string, list<string>>();
}

CallsTable::~CallsTable()
{
	calls_table_.clear();
}

void CallsTable::AddCalls(string caller, string callee)
{
	map<string, list<string>>::iterator it = calls_table_.find(caller);
	list<string> callee_list;

	if (it != calls_table_.end())
	{
		callee_list = it->second;
		bool is_found_dup = find(callee_list.begin(), callee_list.end(), callee) != callee_list.end();
		if (!is_found_dup)
		{
			it->second.push_back(callee);
		}
	}
	else
	{
		callee_list.push_back(callee);
		calls_table_.insert(pair<string, list<string>>(caller, callee_list));
	}
}

list<string> CallsTable::GetCallee(string caller)
{
	map<string, list<string>>::iterator it = calls_table_.find(caller);

	if (it != calls_table_.end())
	{
		return it->second;
	}
	else
	{
		return list<string>();
	}
}

list<string> CallsTable::GetCaller(string callee)
{
	list<string> caller_list = list<string>();

	for (map<string, list<string>>::iterator caller_index = calls_table_.begin(); caller_index != calls_table_.end(); ++caller_index)
	{
		for (list<string>::iterator callee_index = caller_index->second.begin(); callee_index != caller_index->second.end(); ++callee_index)
		{
			if (*callee_index == callee)
			{
				caller_list.push_back(caller_index->first);
			}

		}
	}
	return caller_list;
}

bool CallsTable::IsCallExist()
{
	if (calls_table_.empty() == false) {
		return true;
	}
	else {
		return false;
	}
}

bool CallsTable::IsCall(string caller, string callee)
{
	map<string, list<string>>::iterator it = calls_table_.find(caller);

	if (it != calls_table_.end())
	{
		list<string> callee_list_it = it->second;
		bool is_callee_found = find(callee_list_it.begin(), callee_list_it.end(), callee) != callee_list_it.end();
		return is_callee_found;
	}
	else
	{
		return false;
	}
}

bool CallsTable::IsCallStar(string caller, string callee)
{
	calls_star_helper_checked_list_ = list<string>();
	return IsCallsStarHelper(caller, callee);
}

bool CallsTable::IsCallsStarHelper(string caller, string callee)
{
	map<string, list<string>>::iterator it = calls_table_.find(caller);

	if (it != calls_table_.end())
	{
		list<string> direct_call_list = it->second;

		// Mark the current caller as checked
		calls_star_helper_checked_list_.push_back(caller);

		for (list<string>::iterator callee_list_it = direct_call_list.begin(); callee_list_it != direct_call_list.end(); ++callee_list_it)
		{
			// firstly if the current procedure is the original callee, then return true immediately
			if (*callee_list_it == callee)
			{
				return true;
			}
			else // otherwise we have to calls* into this procedure
			{
				bool is_checked_before = find(calls_star_helper_checked_list_.begin(), calls_star_helper_checked_list_.end(), *callee_list_it) != calls_star_helper_checked_list_.end();
				if (!is_checked_before)
				{
					return IsCallsStarHelper(*callee_list_it, callee);
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

list<string> CallsTable::GetCalleeStar(string caller)
{
	get_callees_star_helper_list_ = list<string>();
	GetCalleesStarHelper(caller);
	return get_callees_star_helper_list_;
}

/* DFS method of searching for callees */
void CallsTable::GetCalleesStarHelper(string caller)
{
	map<string, list<string>>::iterator it = calls_table_.find(caller);

	if (it != calls_table_.end())
	{
		list<string> direct_callee_it = it->second;

		for (list<string>::iterator calleeIt = direct_callee_it.begin(); calleeIt != direct_callee_it.end(); ++calleeIt)
		{
			bool is_checked_before = find(get_callees_star_helper_list_.begin(), get_callees_star_helper_list_.end(), *calleeIt) != get_callees_star_helper_list_.end();
			if (!is_checked_before)
			{
				// Push this callee into the list and search into it
				get_callees_star_helper_list_.push_back(*calleeIt);
				GetCalleesStarHelper(*calleeIt);
			}
		}
	}
	else
	{
		return;
	}
}

list<string> CallsTable::GetCallerStar(string callee)
{
	get_callers_star_helper_list_ = list<string>();
	GetCallersStarHelper(callee);
	get_callees_star_helper_list_.sort();
	get_callers_star_helper_list_.unique();
	return get_callers_star_helper_list_;
}

/* DFS search for all Callers, (because dont have a callee-keyed table, runtime here could be bad, pending further testing */
void CallsTable::GetCallersStarHelper(string callee)
{
	for (map<string, list<string>>::iterator caller_index = calls_table_.begin(); caller_index != calls_table_.end(); ++caller_index)
	{
		for (list<string>::iterator callee_index = caller_index->second.begin(); callee_index != caller_index->second.end(); ++callee_index)
		{
			if (*callee_index == callee)
			{
				get_callers_star_helper_list_.push_back(caller_index->first);
				GetCallersStarHelper(caller_index->first);
				//bool checkedBefore = find(getCallersStarHelperList.begin(), getCallersStarHelperList.end(), *calleeIndex) != getCallersStarHelperList.end();
				//if (!checkedBefore)
				//{
				//	// Push this caller into the list and search into it
				//	getCallersStarHelperList.push_back(callerIndex->first);
				//	getCallersStarHelper(callerIndex->first);
				//}
			}
		}
	}
}