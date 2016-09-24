#pragma once

#include <unordered_map>
#include <map>
#include <iostream>

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
	unordered_map<int, vector<int> > tableNestingWise; // key: nesting index; value: stmt	
};