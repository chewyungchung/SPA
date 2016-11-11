/*
FollowsTable consists of a main table keyed by stmtLine, with value nestingLevel
nestingLevel indicates which stmtLst the stmtLine belongs to
To compute Follows*(s1, s2), the below two conditions must be satisified
1. s1 s2 are in the same nestingLevel
2. s1 and s2 are in sequential order i.e s1 < s2
Thus Follows* can be determined in O(1)
Follows(s1, s2) holds if the above conditions are satisfied AND additionally s1 comes directly after s2 in that same nestingLevel
*/
#include "FollowsTable.h"

FollowsTable::FollowsTable()
{
	table_stmt_wise_ = map<int, int>();
	table_nesting_wise_ = map<int, vector<int> >();
}

FollowsTable::~FollowsTable()
{
	table_stmt_wise_.clear();
	table_nesting_wise_.clear();
}

void FollowsTable::AddFollows(int stmt, int nestingIndex)
{
	map<int, int>::iterator it = table_stmt_wise_.find(stmt);

	if (it != table_stmt_wise_.end())
	{
		throw logic_error("statement already exists");
	}
	else 
	{
		table_stmt_wise_[stmt] = nestingIndex;
		table_nesting_wise_[nestingIndex].push_back(stmt);
	}
}

int FollowsTable::GetFollowedFrom(int stmt)
{
	vector<int> stmts;
	list<int> empty_list;
	int table_stmt_wise_lvl = table_stmt_wise_[stmt];

	if (table_stmt_wise_lvl != 0)
	{
		stmts = table_nesting_wise_[table_stmt_wise_lvl];
		if (stmts.empty())
		{
			return -1;
		}
	}
		
	vector<int> r;
		
	for (auto& iter_value : stmts) {
		if (this->IsValidFollows(iter_value, stmt)) {
			r.push_back(iter_value);
		}
	}

	if (r.empty() == true) {
		return -1;
	}

	return r.at(0);
}

int FollowsTable::GetFollower(int stmt)
{
	vector<int> stmts;
	list<int> empty_list;
	int table_stmt_wise_lvl = table_stmt_wise_[stmt];

	if (table_stmt_wise_lvl != 0) {
		stmts = table_nesting_wise_[table_stmt_wise_lvl];
		if (stmts.empty() == true) {
			return -1;
		}
	}

	vector<int> r;

	for (auto& iter_value : stmts) {
		if (this->IsValidFollows(stmt, iter_value)) {
			r.push_back(iter_value);
		}
	}

	if (r.empty() == true) {
		return -1;
	}

	return r.at(0);
}

bool FollowsTable::IsFollowEmpty()
{
	return table_stmt_wise_.empty();
}

bool FollowsTable::IsValidFollows(int from, int to)
{
	if (from >= to) {
		return false;
	}

	int nesting_at_from = table_stmt_wise_[from];
	int nesting_at_to = table_stmt_wise_[to];

	if (nesting_at_from == 0 || nesting_at_to == 0)
	{
		return false;
	}

	if (nesting_at_from == nesting_at_to)
	{
		if (from + 1 == to)
		{
			return true;
		}
		else
		{
			int subsequent_stmt = from + 1;
			map<int, int>::iterator i = table_stmt_wise_.find(subsequent_stmt);
			if (i == table_stmt_wise_.end())
			{
				return false;
			}
			for (map<int, int>::iterator iter = i; iter != table_stmt_wise_.find(to); ++iter)
			{
				if (iter->second == nesting_at_from)
				{
					return false;
				}
			}
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool FollowsTable::IsFollowsStar(int from, int to)
{
	if (from >= to)
	{
		return false;
	}
		/*int nestingAtFrom = tableStmtWise.at(from);
		int nestingAtTo = tableStmtWise.at(to);*/
	int nesting_at_from = table_stmt_wise_[from];
	int nesting_at_to = table_stmt_wise_[to];

	if (nesting_at_from != 0 && nesting_at_to != 0)
	{
		if (nesting_at_from == nesting_at_to)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

list<int> FollowsTable::GetFollowedFromStar(int stmt)
{
	vector<int> stmts;
	list<int> empty_list;
	int table_stmt_wise_lvl = table_stmt_wise_[stmt];
	
	if (table_stmt_wise_lvl != 0)
	{
		stmts = table_nesting_wise_[table_stmt_wise_lvl];
		if (stmts.empty() == true)
		{
			return empty_list;
		}
	}

	list<int> r = list<int>();

	for (auto& iter_value : stmts)
	{
		if (this->IsFollowsStar(iter_value, stmt))
		{
			r.push_back(iter_value);
		}
	}
	return r;
}

list<int> FollowsTable::GetFollowerStar(int stmt)
{
	vector<int> stmts;
	list<int> empty_list;
	int table_stmt_wise_lvl = table_stmt_wise_[stmt];

	if (table_stmt_wise_lvl != 0)
	{
		stmts = table_nesting_wise_[table_stmt_wise_lvl];
		if (stmts.empty() == true)
		{
			return empty_list;
		}
	}

	list<int> r;

	for (auto& iter_value : stmts)
	{
		if (this->IsFollowsStar(stmt, iter_value))
		{
			r.push_back(iter_value);
		}
	}
	return r;
}


