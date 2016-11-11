#pragma once

#include <map>
#include <list>

using namespace std;

class ParentTable
{
public:
	ParentTable();
	~ParentTable();

	void AddParent(int parent, int child);
	void AddParentStar(int parent_star, int child);

	int GetParentOf(int stmt);
	list<int> GetChildrenOf(int stmt);
	bool IsParentEmpty();
	bool IsParentOf(int parent, int child);
	list<int> GetParentStar(int stmt);
	bool IsParentStar(int parent, int child);
	list<int> GetChildStarOf(int stmt);

private:
	int NO_PARENT_FLAG = -1;
	map<int, int> table_child_wise_; // key: child: value: parent
	map<int, list<int> > table_parent_wise_; // key: parent; value: child(ren)	
	map<int, list<int> > table_child_wise_star_; // key: child; value: parent(s)-star
	map<int, list<int> > table_parent_wise_star_; // key: parent; value: child(ren)-star
};