#include "FollowsTable.h"

FollowsTable::FollowsTable() {
	tableStmtWise = map<int, int>();
	tableNestingWise = unordered_map<int, vector<int> >();
}

FollowsTable::~FollowsTable() {
	tableStmtWise.clear();
	tableNestingWise.clear();
}

void FollowsTable::addFollows(int stmt, int nestingIndex) {
	map<int, int>::iterator it = tableStmtWise.find(stmt);
	if (it != tableStmtWise.end()) {
		throw logic_error("statement already exists");
	}
	else {
		tableStmtWise[stmt] = nestingIndex;
		tableNestingWise[nestingIndex].push_back(stmt);
	}
}

int FollowsTable::getFollowedFrom(int stmt)
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
		//vector<int> stmts = tableNestingWise.at(tableStmtWise.at(stmt));
		
		vector<int> r;
		
		for (auto& iterValue : stmts) {
			if (this->isValidFollows(iterValue, stmt)) {
				r.push_back(iterValue);
			}
		}

		/*if (r.size() > 1) {
			throw logic_error("There are more than 1 statements that the given statement follows");
		}*/

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
		//vector<int> stmts = tableNestingWise.at(tableStmtWise.at(stmt));

	vector<int> r;

		for (auto& iterValue : stmts) {
			if (this->isValidFollows(stmt, iterValue)) {
				r.push_back(iterValue);
			}
		}

		/*if (r.size() > 1) {
			throw logic_error("There are more than 1 statements that follow the given statement");
		}*/

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
	if (isgreaterequal(from, to) == true) {
		return false;
	}

		int nestingAtFrom = tableStmtWise[from];
		int nestingAtTo = tableStmtWise[to];

		if (nestingAtFrom == 0 || nestingAtTo == 0) {
			return false;
		}

		if (nestingAtFrom == nestingAtTo) {
			if (from + 1 == to) return true; 
			else {
				int subsequentStmt = from + 1;
				map<int, int>::iterator i = tableStmtWise.find(subsequentStmt);
				if (i == tableStmtWise.end()) {
					return false;
				}
				for (map<int, int>::iterator iter = i;
					iter != tableStmtWise.find(to);
					++iter) {
					if (iter->second == nestingAtFrom) {
						return false;
					}
				}
				return true;
			}
			int subsequentStmt = from + 1;
			map<int, int>::iterator i = tableStmtWise.find(subsequentStmt);
			if (i == tableStmtWise.end()) {
				return false;
			}
			for (map<int, int>::iterator iter = i;
				iter != tableStmtWise.find(to);
				++iter) {
				if (iter->second == nestingAtFrom) {
					return false;
				}
			}
			return true;
		}
		return false;
}

bool FollowsTable::isFollowsStar(int from, int to)
{
	if (from >= to) {
		return false;
	}
		/*int nestingAtFrom = tableStmtWise.at(from);
		int nestingAtTo = tableStmtWise.at(to);*/
	int nestingAtFrom = tableStmtWise[from];
	int nestingAtTo = tableStmtWise[to];

	if (nestingAtFrom != 0 && nestingAtTo != 0) {
		if (nestingAtFrom == nestingAtTo) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}

}

list<int> FollowsTable::getFollowedFromStar(int stmt)
{
	vector<int> stmts;
	list<int> emptyList;
	int tableStmtWiseLvl = tableStmtWise[stmt];
	
	if (tableStmtWiseLvl != 0) {
		stmts = tableNestingWise[tableStmtWiseLvl];
		if (stmts.empty() == true) {
			return emptyList;
		}
	}

		//vector<int> stmts = tableNestingWise.at(tableStmtWise.at(stmt));

		list<int> r = list<int>();

		for (auto& iterValue : stmts) {
			if (this->isFollowsStar(iterValue, stmt)) {
				r.push_back(iterValue);
			}
		}

		return r;
}

list<int> FollowsTable::getFollowerStar(int stmt)
{
		//vector<int> stmts = tableNestingWise.at(tableStmtWise.at(stmt));

		vector<int> stmts;
		list<int> emptyList;
		int tableStmtWiseLvl = tableStmtWise[stmt];

		if (tableStmtWiseLvl != 0) {
			stmts = tableNestingWise[tableStmtWiseLvl];
			if (stmts.empty() == true) {
				return emptyList;
			}
		}

		list<int> r;

		for (auto& iterValue : stmts) {
			if (this->isFollowsStar(stmt, iterValue)) {
				r.push_back(iterValue);
			}
		}

		return r;
}


