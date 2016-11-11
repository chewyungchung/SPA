#include "ModUsesTablebyStmt.h"

using namespace std;

ModUsesTablebyStmt::ModUsesTablebyStmt()
{
	modifies_table_ = map<int, list<string>>();
	uses_table_ = map<int, list<string>>();
}

ModUsesTablebyStmt::~ModUsesTablebyStmt()
{
}

map<int, list<string>> ModUsesTablebyStmt::GetModTable()
{
	return modifies_table_;
}

map<int, list<string>> ModUsesTablebyStmt::GetUsesTable()
{
	return uses_table_;
}

void ModUsesTablebyStmt::AddModifies(int stmtNum, string var)
{
	map<int, list<string>>::iterator it = modifies_table_.find(stmtNum);
	list<string> mod_var_list;

	/* if key is not found, add key-pair into modifiesTable */
	if (it == modifies_table_.end())
	{
		mod_var_list.push_back(var);
		modifies_table_.insert(pair<int, list<string>>(stmtNum, mod_var_list));
	}
	else
	{
		mod_var_list = it->second;
		bool found = find(mod_var_list.begin(), mod_var_list.end(), var) != mod_var_list.end();
		if (!found)
		{
			it->second.push_back(var);
		}
	}
}

void ModUsesTablebyStmt::AddUses(int stmtNum, string var)
{
	map<int, list<string>>::iterator it = uses_table_.find(stmtNum);
	list<string> use_var_list;

	/* if key is not found, add key-value pair into usesTable */
	if (it == uses_table_.end())
	{
		use_var_list.push_back(var);
		uses_table_.insert(pair<int, list<string>>(stmtNum, use_var_list));
	}
	else
	{
		use_var_list = it->second;
		bool is_found = find(use_var_list.begin(), use_var_list.end(), var) != use_var_list.end();
		if (!is_found)
		{
			it->second.push_back(var);
		}
	}
}

bool ModUsesTablebyStmt::IsModified(int stmtNum, string varName)
{
	map<int, list<string>>::iterator it = modifies_table_.find(stmtNum);

	if (it != modifies_table_.end())
	{
		list<string> varModified = it->second;
		return find(varModified.begin(), varModified.end(), varName) != varModified.end();
	}
	else
	{
		return false;
	}
}

bool ModUsesTablebyStmt::IsUsed(int stmtNum, string varName)
{
	map<int, list<string>>::iterator it = uses_table_.find(stmtNum);

	if (it != uses_table_.end())
	{
		list<string> used_var_list = it->second;
		return find(used_var_list.begin(), used_var_list.end(), varName) != used_var_list.end();
	}
	else
	{
		return false;
	}
}

list<string> ModUsesTablebyStmt::GetModifiedBy(int stmtNum)
{
	map<int, list<string>>::iterator it = modifies_table_.find(stmtNum);
	list<string> mod_var_list;

	if (it != modifies_table_.end())
	{
		mod_var_list = it->second;
		return mod_var_list;
	}
	else
	{
		return list<string>();
	}
}

list<string> ModUsesTablebyStmt::GetUsedBy(int stmtNum)
{
	map<int, list<string>>::iterator it = uses_table_.find(stmtNum);
	list<string> use_var_list;

	if (it != uses_table_.end())
	{
		use_var_list = it->second;
		return use_var_list;
	}
	else
	{
		return list<string>();
	}
}

