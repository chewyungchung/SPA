#include "FollowsTable.h"

FollowsTable::FollowsTable() {
	followersTable = vector<int>();
	followingTable = vector<int>();
}

FollowsTable::~FollowsTable() {
	followersTable.clear();
	followingTable.clear();
}

void FollowsTable::addFollows(int from, int to) {
	followersTable[from] = to;
	followingTable[to] = from;
}

int FollowsTable::getFollowedFrom(int stmt)
{
	int r;
	try {
		r = followingTable.at(stmt);
		return r;
	}
	catch (out_of_range& err) {
		cerr << "Out of range error: " << err.what() << "\n";
		return -1;
	}
}

int FollowsTable::getFollower(int stmt)
{
	int r;
	try {
		r = followersTable.at(stmt);
		return r;
	}
	catch (out_of_range& err) {
		cerr << "Out of range error: " << err.what() << "\n";
		return -1;
	}
}

bool FollowsTable::isFollowEmpty()
{
	return followersTable.empty();
}

bool FollowsTable::isValidFollows(int from, int to)
{
	int r;
	bool res = true;
	try {
		r = followersTable.at(from);
	}
	catch (out_of_range& oor) {
		cerr << "Out of range error:" << oor.what() << "\n";
		return false;
	}
	if (r != to) {
		res = false;
	}
	return res;
}