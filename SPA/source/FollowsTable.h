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

	void AddFollows(int stmt, int nest_index);

	int GetFollowedFrom(int stmt);
	int GetFollower(int stmt);
	bool IsFollowEmpty();
	bool IsValidFollows(int from, int to);
	bool IsFollowsStar(int from, int to);
	list<int> GetFollowedFromStar(int stmt);
	list<int> GetFollowerStar(int stmt);

private:
	map<int, int> table_stmt_wise_; // key: stmt; value: nesting index
	map<int, vector<int> > table_nesting_wise_; // key: nesting index; value: stmt	
};