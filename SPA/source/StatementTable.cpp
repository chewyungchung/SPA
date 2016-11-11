#include "StatementTable.h"
#include <assert.h>

using namespace std;

StatementTable::StatementTable()
{
	list<int> assign_list_;
	list<int> while_list_;
	list<int> all_stmt_list_;
	list<int> if_list_;
	list<int> call_list_;
	map<int, string> call_proc_list_; // key: callstmt#, value: associated procName
	map<int, string> ctrl_var_list_;
}

StatementTable::~StatementTable()
{
}

void StatementTable::AddStatement(int stmt_num, string stmt_type)
{
	if (stmt_type == "assign")
	{
		assign_list_.push_back(stmt_num);
		all_stmt_list_.insert(pair<int, string>(stmt_num, stmt_type));
	}
	else
	{
		assert(false);
	}
}

void StatementTable::AddStatement(int stmt_num, string stmt_type, string ctrl_var_or_proc_name)
{
	if (stmt_type == "assign")
	{
		assign_list_.push_back(stmt_num);
		all_stmt_list_.insert(pair<int, string>(stmt_num, stmt_type));
	}
	else if (stmt_type == "call")
	{
		call_list_.push_back(stmt_num);
		all_stmt_list_.insert(pair<int, string>(stmt_num, stmt_type));
		call_proc_list_.insert(pair<int, string>(stmt_num, ctrl_var_or_proc_name));
	}
	else if (stmt_type == "while")
	{
		while_list_.push_back(stmt_num);
		all_stmt_list_.insert(pair<int, string>(stmt_num, stmt_type));
		ctrl_var_list_.insert(pair<int, string>(stmt_num, ctrl_var_or_proc_name));
	}
	else if (stmt_type == "if")
	{
		if_list_.push_back(stmt_num);
		all_stmt_list_.insert(pair<int, string>(stmt_num, stmt_type));
		ctrl_var_list_.insert(pair<int, string>(stmt_num, ctrl_var_or_proc_name));
	}
	else
	{
		assert(false);
	}
}

list<int> StatementTable::GetAssignList()
{
	return assign_list_;
}

list<int> StatementTable::GetWhileList()
{
	return while_list_;
}

list<int> StatementTable::GetIfList()
{
	return if_list_;
}

list<int> StatementTable::GetCallList()
{
	return call_list_;
}

string StatementTable::GetProcNameByCallStmt(int call_stmt)
{
	string result = "";
	for (const auto& i : call_proc_list_)
	{
		if (i.first == call_stmt && (find(call_list_.begin(), call_list_.end(), i.first) != call_list_.end()))
		{
			result = i.second;
			break;
		}
	}
	return result;
}

list<int> StatementTable::GetIfListWithControlVariable(string ctrl_var)
{
	list<int> if_list;
	for (const auto& i : ctrl_var_list_)
	{
		if (i.second == ctrl_var && (find(if_list_.begin(), if_list_.end(), i.first) != if_list_.end()))
		{
			if_list.push_back(i.first);
		}
	}
	return if_list;
}

list<int> StatementTable::GetWhileListWithControlVariable(string ctrl_var)
{
	list<int> while_list;
	for (const auto& i : ctrl_var_list_)
	{
		if (i.second == ctrl_var && (find(while_list_.begin(), while_list_.end(), i.first) != while_list_.end()))
		{
			while_list.push_back(i.first);
		}
	}
	return while_list;
}

string StatementTable::GetControlVarWithStmt(int stmt_num)
{
	string ctrl_var = "";
	if (ctrl_var_list_[stmt_num] != "") {
		ctrl_var = ctrl_var_list_[stmt_num];
	}
	return ctrl_var;
}

list<int> StatementTable::GetStmtList()
{
	list<int> results = list<int>();
	map<int, string>::iterator it;
	for (it = all_stmt_list_.begin(); it != all_stmt_list_.end(); ++it)
	{
		results.push_back(it->first);
	}
	return results;
}

int StatementTable::GetStatementCount()
{
	return all_stmt_list_.size();
}

bool StatementTable::IsValidStmt(int stmt_num)
{
	map<int, string>::iterator it = all_stmt_list_.find(stmt_num);
	if (it != all_stmt_list_.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

string StatementTable::GetStmtType(int stmt_num)
{
	string result = "";
	map<int, string>::iterator it = all_stmt_list_.find(stmt_num);

	if (it != all_stmt_list_.end())
	{
		result = it->second;
		return result;
	}
	else
	{
		return result;
	}
}

bool StatementTable::IsAssign(int stmt_num)
{
	return find(assign_list_.begin(), assign_list_.end(), stmt_num) != assign_list_.end();
}
