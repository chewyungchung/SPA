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
	proc_name_index_table_ = map<string, int>();
	proc_data_table_ = vector<vector<list<string>>>();
	max_proc_index_ = 0;
}

ProcTable::~ProcTable()
{
	proc_name_index_table_.clear();
	proc_data_table_.clear();
}

void ProcTable::AddProc(string proc_name)
{
	if (proc_name_index_table_.find(proc_name) != proc_name_index_table_.end()) {
		return;
	}
	proc_name_index_table_.insert(pair<string, int>(proc_name, max_proc_index_));

	vector<list<string>> new_proc(PROCDATA_COLSIZE);

	list<string> proc_name_list = list<string>();
	proc_name_list.push_back(proc_name);
	new_proc[PROCNAME_COL] = proc_name_list;

	list<string> mod_var_list = list<string>();
	new_proc[MODVAR_COL] = mod_var_list;

	list<string> use_var_list = list<string>();
	new_proc[USEVAR_COL] = use_var_list;

	list<string> called_in_stmt_list = list<string>();
	new_proc[CALLED_IN_STMT_COL] = called_in_stmt_list;
	
	proc_data_table_.push_back(new_proc);
	max_proc_index_++;
}

void ProcTable::AddProcMod(string proc_name, string var)
{
	map<string, int>::iterator it = proc_name_index_table_.find(proc_name);
	list<string> mod_var_list;

	if (it != proc_name_index_table_.end())
	{
		mod_var_list = proc_data_table_[it->second][MODVAR_COL];
		bool is_found_dup = find(mod_var_list.begin(), mod_var_list.end(), var) != mod_var_list.end();
		if (!is_found_dup)
		{
			proc_data_table_[it->second][MODVAR_COL].push_back(var);
		}
	}
}

void ProcTable::AddProcUses(string proc_name, string var)
{
	map<string, int>::iterator it = proc_name_index_table_.find(proc_name);
	list<string> use_var_list;

	if (it != proc_name_index_table_.end())
	{
		use_var_list = proc_data_table_[it->second][USEVAR_COL];
		bool is_found_dup = find(use_var_list.begin(), use_var_list.end(), var) != use_var_list.end();
		if (!is_found_dup)
		{
			proc_data_table_[it->second][USEVAR_COL].push_back(var);
		}
	}
}

void ProcTable::AddProcCalledInStmt(string proc_name, int stmt_line)
{
	map<string, int>::iterator it = proc_name_index_table_.find(proc_name);
	list<string> called_in_var_list;

	if (it != proc_name_index_table_.end())
	{
		called_in_var_list = proc_data_table_[it->second][CALLED_IN_STMT_COL];
		bool is_found_dup = find(called_in_var_list.begin(), called_in_var_list.end(), to_string(stmt_line)) != called_in_var_list.end();
		if (!is_found_dup)
		{
			proc_data_table_[it->second][CALLED_IN_STMT_COL].push_back(to_string(stmt_line));
		}
	}
	else {
		AddProc(proc_name);
		map<string, int>::iterator it = proc_name_index_table_.find(proc_name);
		proc_data_table_[it->second][CALLED_IN_STMT_COL].push_back(to_string(stmt_line));
	}
}

// Return empty string if procIndex does not exist
string ProcTable::GetProcName(int proc_index)
{
	string proc_name = "";
	for (map<string, int>::iterator it = proc_name_index_table_.begin(); it != proc_name_index_table_.end(); ++it)
	{
		if (it->second == proc_index)
		{
			proc_name = it->first;
			return proc_name;
		}
	}
	return proc_name;
}

// Return -1 if procName does not exist
int ProcTable::GetProcIndex(string proc_name)
{
	int proc_index = -1;
	map<string, int>::iterator it = proc_name_index_table_.find(proc_name);

	if (it != proc_name_index_table_.end())
	{
		proc_index = it->second;
		return proc_index;
	}
	else
	{
		return proc_index;
	}
}

// Also return false if procName doesn not exist
bool ProcTable::IsModifiedByProc(string proc_name, string var_name)
{
	map<string, int>::iterator it = proc_name_index_table_.find(proc_name);
	list<string> mod_var_list;
	
	if (it != proc_name_index_table_.end())
	{
		mod_var_list = proc_data_table_[it->second][MODVAR_COL];
		return find(mod_var_list.begin(), mod_var_list.end(), var_name) != mod_var_list.end();
	}
	else
	{
		return false;
	}
}

bool ProcTable::IsUsedByProc(string proc_name, string var_name)
{
	map<string, int>::iterator it = proc_name_index_table_.find(proc_name);
	list<string> use_var_list;

	if (it != proc_name_index_table_.end())
	{
		use_var_list = proc_data_table_[it->second][USEVAR_COL];
		return find(use_var_list.begin(), use_var_list.end(), var_name) != use_var_list.end();
	}
	else
	{
		return false;
	}
}

list<string> ProcTable::GetModifiedByProc(string proc_name)
{
	map<string, int>::iterator it = proc_name_index_table_.find(proc_name);

	if (it != proc_name_index_table_.end())
	{
		return proc_data_table_[it->second][MODVAR_COL];
	}
	else
	{
		return list<string>();
	}
}

list<string> ProcTable::GetUsedByProc(string proc_name)
{
	map<string, int>::iterator it = proc_name_index_table_.find(proc_name);

	if (it != proc_name_index_table_.end())
	{
		return proc_data_table_[it->second][USEVAR_COL];
	}
	else
	{
		return list<string>();
	}
}

list<string> ProcTable::GetProcedureModifying(string var_name)
{
	list<string> results = list<string>();

	for (unsigned i = 0; i < proc_data_table_.size(); i++)
	{
		list<string> curr_mod_var_list = proc_data_table_[i][MODVAR_COL];
		bool is_found_var = find(curr_mod_var_list.begin(), curr_mod_var_list.end(), var_name) != curr_mod_var_list.end();
		if (is_found_var)
		{
			results.push_back(proc_data_table_[i][PROCNAME_COL].back());
		}
	}
	return results;
}

list<string> ProcTable::GetProcedureUsing(string var_name)
{
	list<string> results = list<string>();

	for (unsigned i = 0; i < proc_data_table_.size(); i++)
	{
		list<string> curr_use_var_list = proc_data_table_[i][USEVAR_COL];
		bool is_found_var = find(curr_use_var_list.begin(), curr_use_var_list.end(), var_name) != curr_use_var_list.end();
		if (is_found_var)
		{
			results.push_back(proc_data_table_[i][PROCNAME_COL].back());
		}
	}
	return results;
}

bool ProcTable::IsProcedureExist(string proc_name)
{
	map<string, int>::iterator it = proc_name_index_table_.find(proc_name);
	return it != proc_name_index_table_.end();
}

list<int> ProcTable::GetCallByProcName(string proc_name)
{
	map<string, int>::iterator it = proc_name_index_table_.find(proc_name);
	list<int> results = list<int>();

	if (it != proc_name_index_table_.end())
	{
		list<string> s_results = proc_data_table_[it->second][CALLED_IN_STMT_COL];
		list<string>::iterator it;
		for (it = s_results.begin(); it != s_results.end(); ++it)
		{
			results.push_back(stoi(*it));
		}
		return results;
	}
	else
	{
		return results;
	}
}

list<string> ProcTable::GetCalledProcNamesList()
{
	list<string> results = list<string>();

	for (unsigned i = 0; i < proc_data_table_.size(); i++)
	{
		list<string> curr_called_in_stmt_list = proc_data_table_[i][CALLED_IN_STMT_COL];
		if (!curr_called_in_stmt_list.empty())
		{
			results.push_back(proc_data_table_[i][PROCNAME_COL].back());
		}
	}
	return results;
}

list<string> ProcTable::GetProcedureList()
{
	list<string> results = list<string>();
	map<string, int>::iterator it;
	for (it = proc_name_index_table_.begin(); it != proc_name_index_table_.end(); ++it)
	{
		results.push_back(it->first);
	}
	return results;
}