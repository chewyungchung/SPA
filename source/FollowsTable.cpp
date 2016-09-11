#include "FollowsTable.h"

FollowsTable::FollowsTable() {
	tableStmtWise = unordered_map<int, int>();
	tableNestingWise = unordered_map<int, vector<int> >();
}

FollowsTable::~FollowsTable() {
	tableStmtWise.clear();
	tableNestingWise.clear();
}

void FollowsTable::addFollows(int stmt, int nestingIndex) {
	unordered_map<int, int>::iterator it = tableStmtWise.find(stmt);
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
	try {
		vector<int> stmts = tableNestingWise.at(tableStmtWise.at(stmt));
		
		vector<int> r = vector<int>();
		
		for (auto& iterValue : stmts) {
			if (this->isValidFollows(iterValue, stmt)) {
				r.push_back(iterValue);
			}
		}

		if (r.size() != 1) {
			throw logic_error("There are more than 1 statements that the given statement follows");
		}

		return r.at(0);
	}
	catch (out_of_range& oor) {
		cerr << "Out of range error: " << oor.what() << "\n";
		return -1;
	}
}

int FollowsTable::getFollower(int stmt)
{
	try {
		vector<int> stmts = tableNestingWise.at(tableStmtWise.at(stmt));

		vector<int> r = vector<int>();

		for (auto& iterValue : stmts) {
			if (this->isValidFollows(stmt, iterValue)) {
				r.push_back(iterValue);
			}
		}

		if (r.size() != 1) {
			throw logic_error("There are more than 1 statements that follow the given statement");
		}

		return r.at(0);
	}
	catch (out_of_range& oor) {
		cerr << "Out of range error: " << oor.what() << "\n";
		return -1;
	}
}

bool FollowsTable::isFollowEmpty()
{
	return tableStmtWise.empty();
}

bool FollowsTable::isValidFollows(int from, int to)
{
	if (to >= from) {
		return false;
	}

	try {
		int nestingAtFrom = tableStmtWise.at(from);
		int nestingAtTo = tableStmtWise.at(to);

		if (nestingAtFrom == nestingAtTo) {
			for (unordered_map<int, int>::iterator iter = ++tableStmtWise.find(from);
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
	catch (out_of_range& oor) {
		cerr << "Out of range error: " << oor.what() << "\n";
	}
}

bool FollowsTable::isFollowsStar(int from, int to)
{
	if (to >= from) {
		return false;
	}

	try {
		int nestingAtFrom = tableStmtWise.at(from);
		int nestingAtTo = tableStmtWise.at(to);
		if (nestingAtFrom == nestingAtTo) {
			return true;
		}
		else {
			return false;
		}
	}
	catch (out_of_range& oor) {
		cerr << "Out of range error: " << oor.what() << "\n";
	}
}

list<int> FollowsTable::getFollowedFromStar(int stmt)
{

	try {
		vector<int> stmts = tableNestingWise.at(tableStmtWise.at(stmt));

		list<int> r = list<int>();

		for (auto& iterValue : stmts) {
			if (this->isFollowsStar(iterValue, stmt)) {
				r.push_back(iterValue);
			}
		}

		return r;
	}
	catch (out_of_range& oor) {
		cerr << "Out of range error: " << oor.what() << "\n";
		return list<int>();
	}
}

list<int> FollowsTable::getFollowerStar(int stmt)
{

	try {
		vector<int> stmts = tableNestingWise.at(tableStmtWise.at(stmt));

		list<int> r = list<int>();

		for (auto& iterValue : stmts) {
			if (this->isFollowsStar(stmt, iterValue)) {
				r.push_back(iterValue);
			}
		}

		return r;
	}
	catch (out_of_range& oor) {
		cerr << "Out of range error: " << oor.what() << "\n";
		return list<int>();
	}
}


