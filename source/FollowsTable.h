#pragma once

#include <vector>
#include <list>
#include <iostream>

using namespace std;

class FollowsTable {
private:
	vector<int> followersTable; // statement(s) that follow a given statement
	vector<int> followingTable; // for a given statement, the statement(s) that it is following

public:
	FollowsTable();
	~FollowsTable();

	void addFollows(int from, int to);

	int getFollowedFrom(int stmt);
	int getFollower(int stmt);
	bool isFollowEmpty();
	bool isValidFollows(int from, int to);
};