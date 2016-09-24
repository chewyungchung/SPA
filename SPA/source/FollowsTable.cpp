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
	tableStmtWise = map<int, int>();
	tableNestingWise = unordered_map<int, vector<int> >();
}

FollowsTable::~FollowsTable()
{
	tableStmtWise.clear();
	tableNestingWise.clear();
}

void FollowsTable::addFollows(int stmt, int nestingIndex)
{
	map<int, int>::iterator it = tableStmtWise.find(stmt);

	if (it != tableStmtWise.end())
	{
		throw logic_error("statement already exists");
	}
	else 
	{
		tableStmtWise[stmt] = nestingIndex;
		tableNestingWise[nestingIndex].push_back(stmt);
	}
}

int FollowsTable::getFollowedFrom(int stmt)
{
	vector<int> stmts;
	list<int> emptyList;
	int tableStmtWiseLvl = tableStmtWise[stmt];

	if (tableStmtWiseLvl != 0)
	{
		stmts = tableNestingWise[tableStmtWiseLvl];
		if (stmts.empty())
		{
			return -1;
		}
	}
		
	vector<int> r;
		
	for (auto& iterValue : stmts) {
		if (this->isValidFollows(iterValue, stmt)) {
			r.push_back(iterValue);
		}
	}

	if (r.empty() == true) {
		return -1;
	}

	return r.at(0);
}

int FollowsTable::getFollower(int stmt)
{
	vector<int> stmts;
	list<int> emptyList;
	int tableStmtWiseLvl = tableStmtWise[stmt];

	if (tableStmtWiseLvl != 0) {
		stmts = tableNestingWise[tableStmtWiseLvl];
		if (stmts.empty() == true) {
			return -1;
		}
	}

	vector<int> r;

	for (auto& iterValue : stmts) {
		if (this->isValidFollows(stmt, iterValue)) {
			r.push_back(iterValue);
		}
	}

	if (r.empty() == true) {
		return -1;
	}

	return r.at(0);
}

bool FollowsTable::isFollowEmpty()
{
	return tableStmtWise.empty();
}

bool FollowsTable::isValidFollows(int from, int to)
{
	if (from >= to) {
		return false;
	}

	int nestingAtFrom = tableStmtWise[from];
	int nestingAtTo = tableStmtWise[to];

	if (nestingAtFrom == 0 || nestingAtTo == 0)
	{
		return false;
	}

	if (nestingAtFrom == nestingAtTo)
	{
		if (from + 1 == to)
		{
			return true;
		}
		else
		{
			int subsequentStmt = from + 1;
			map<int, int>::iterator i = tableStmtWise.find(subsequentStmt);
			if (i == tableStmtWise.end())
			{
				return false;
			}
			for (map<int, int>::iterator iter = i; iter != tableStmtWise.find(to); ++iter)
			{
				if (iter->second == nestingAtFrom)
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

bool FollowsTable::isFollowsStar(int from, int to)
{
	if (from >= to)
	{
		return false;
	}
		/*int nestingAtFrom = tableStmtWise.at(from);
		int nestingAtTo = tableStmtWise.at(to);*/
	int nestingAtFrom = tableStmtWise[from];
	int nestingAtTo = tableStmtWise[to];

	if (nestingAtFrom != 0 && nestingAtTo != 0)
	{
		if (nestingAtFrom == nestingAtTo)
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

list<int> FollowsTable::getFollowedFromStar(int stmt)
{
	vector<int> stmts;
	list<int> emptyList;
	int tableStmtWiseLvl = tableStmtWise[stmt];
	
	if (tableStmtWiseLvl != 0)
	{
		stmts = tableNestingWise[tableStmtWiseLvl];
		if (stmts.empty() == true)
		{
			return emptyList;
		}
	}

	list<int> r = list<int>();

	for (auto& iterValue : stmts)
	{
		if (this->isFollowsStar(iterValue, stmt))
		{
			r.push_back(iterValue);
		}
	}
	return r;
}

list<int> FollowsTable::getFollowerStar(int stmt)
{
	vector<int> stmts;
	list<int> emptyList;
	int tableStmtWiseLvl = tableStmtWise[stmt];

	if (tableStmtWiseLvl != 0)
	{
		stmts = tableNestingWise[tableStmtWiseLvl];
		if (stmts.empty() == true)
		{
			return emptyList;
		}
	}

	list<int> r;

	for (auto& iterValue : stmts)
	{
		if (this->isFollowsStar(stmt, iterValue))
		{
			r.push_back(iterValue);
		}
	}
	return r;
}


