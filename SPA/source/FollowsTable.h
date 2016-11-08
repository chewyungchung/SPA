#pragma once

#include <vector>
#include <list>
#include <map>

using namespace std;

class FollowsTable 
{
public:
	FollowsTable();
	~FollowsTable();

	void addFollows(int stmt, int nestingIndex);

	int getFollowedFrom(int stmt);
	int getFollower(int stmt);
	bool isFollowEmpty();
	bool isValidFollows(int from, int to);
	bool isFollowsStar(int from, int to);
	list<int> getFollowedFromStar(int stmt);
	list<int> getFollowerStar(int stmt);

private:
	map<int, int> tableStmtWise; // key: stmt; value: nesting index
	map<int, vector<int> > tableNestingWise; // key: nesting index; value: stmt	
};