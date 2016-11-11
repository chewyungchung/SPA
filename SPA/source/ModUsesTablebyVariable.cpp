#include "ModUsesTablebyVariable.h"

using namespace std;

ModUsesTablebyVariable::ModUsesTablebyVariable()
{
	var_mod_table_ = map<string, list<int>>();
	var_uses_table_ = map<string, list<int>>();
}

ModUsesTablebyVariable::~ModUsesTablebyVariable()
{
}

map<string, list<int>> ModUsesTablebyVariable::GetModTable()
{
	return var_mod_table_;
}

map<string, list<int>> ModUsesTablebyVariable::GetUsesTable()
{
	return var_uses_table_;
}


void ModUsesTablebyVariable::AddModifies(string varName, int stmtNum)
{
	map<string, list<int>>::iterator it = var_mod_table_.find(varName);
	list<int> mod_stmt_list;

	/* if key is not found, add key-value pair into modifiesTable */
	if (it == var_mod_table_.end())
	{
		mod_stmt_list.push_back(stmtNum);
		var_mod_table_.insert(pair<string, list<int>>(varName, mod_stmt_list));
	}
	else
	{
		mod_stmt_list = it->second;
		bool is_found = find(mod_stmt_list.begin(), mod_stmt_list.end(), stmtNum) != mod_stmt_list.end();
		if (is_found == false)
		{
			it->second.push_back(stmtNum);
		}
	}
}


void ModUsesTablebyVariable::AddUses(string varName, int stmtNum)
{
	map<string, list<int>>::iterator it = var_uses_table_.find(varName);
	list<int> use_stmt_list;

	/* if key is not found, add key-value pair into modifiesTable */
	if (it == var_uses_table_.end())
	{
		use_stmt_list.push_back(stmtNum);
		var_uses_table_.insert(pair<string, list<int>>(varName, use_stmt_list));
	}
	else
	{
		use_stmt_list = it->second;
		bool is_found = find(use_stmt_list.begin(), use_stmt_list.end(), stmtNum) != use_stmt_list.end();
		if (is_found == false)
		{
			it->second.push_back(stmtNum);
		}
	}
}

bool ModUsesTablebyVariable::IsValidVar(string varName)
{
	map<string, list<int>>::iterator it1 = var_uses_table_.find(varName);
	map<string, list<int>>::iterator it2 = var_mod_table_.find(varName);

	if (it1 != var_uses_table_.end() || it2 != var_mod_table_.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

list<int> ModUsesTablebyVariable::GetModifiedBy(string varName)
{
	map<string, list<int>>::iterator it = var_mod_table_.find(varName);

	if (it != var_mod_table_.end())
	{
		return var_mod_table_.at(varName);
	}
	else
	{
		return list<int>();
	}
}

list<int> ModUsesTablebyVariable::GetUsedBy(string varName)
{
	map<string, list<int>>::iterator it = var_uses_table_.find(varName);
	list<int> empty_list;
	if (it != var_uses_table_.end())
	{
		return var_uses_table_.at(varName);
	}
	else
	{
		return empty_list;
	}
}

list<string> ModUsesTablebyVariable::GetAllModVar()
{
	map<string, list<int>>::iterator it;
	list<string> all_mod_var_list;

	if (var_mod_table_.empty())
	{
		return list<string>();
	}

	for (it = var_mod_table_.begin(); it != var_mod_table_.end(); ++it)
	{
		all_mod_var_list.push_back(it->first);
	}
	all_mod_var_list.sort();
	all_mod_var_list.unique();
	return all_mod_var_list;
}

list<string> ModUsesTablebyVariable::GetAllUsedVar()
{
	map<string, list<int>>::iterator it;
	list<string> all_used_var_list;

	if (var_uses_table_.empty())
	{
		return list<string>();
	}

	for (it = var_uses_table_.begin(); it != var_uses_table_.end(); ++it)
	{
		all_used_var_list.push_back(it->first);
	}
	all_used_var_list.sort();
	all_used_var_list.unique();
	return all_used_var_list;
}

list<string> ModUsesTablebyVariable::GetVarList()
{
	map<string, list<int>>::iterator it1;
	map<string, list<int>>::iterator it2;
	list<string> all_var_list;

	for (it1 = var_mod_table_.begin(); it1 != var_mod_table_.end(); ++it1)
	{
		all_var_list.push_back(it1->first);
	}

	for (it2 = var_uses_table_.begin(); it2 != var_uses_table_.end(); ++it2)
	{
		all_var_list.push_back(it2->first);
	}

	all_var_list.sort();
	all_var_list.unique();

	return all_var_list;
}
