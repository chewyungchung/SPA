#pragma once

#include <list>
#include <string>
#include <map>

using namespace std;

class ModUsesTablebyVariable
{
public:
	ModUsesTablebyVariable();
	~ModUsesTablebyVariable();
	map<string, list<int>> GetModTable();
	map<string, list<int>> GetUsesTable();
	void AddModifies(string var, int stmt_num);
	void AddUses(string var, int stmt_num);
	bool IsValidVar(string var_name);
	list<int> GetModifiedBy(string var_name);
	list<int> GetUsedBy(string var_name);
	list<string> GetAllModVar();
	list<string> GetAllUsedVar();
	list<string> GetVarList();

private:
	map<string, list<int>> var_mod_table_;
	map<string, list<int>> var_uses_table_;
};

