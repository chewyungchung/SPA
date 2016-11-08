#pragma once

#include <map>
#include <list>

using namespace std;

class ParentTable
{
public:
	ParentTable();
	~ParentTable();

	void addParent(int parent, int child);
	void addParentStar(int parentStar, int child);

	int getParentOf(int stmt);
	list<int> getChildrenOf(int stmt);
	bool isParentEmpty();
	bool isParentOf(int parent, int child);
	list<int> getParentStar(int stmt);
	bool isParentStar(int parent, int child);
	list<int> getChildStarOf(int stmt);

private:
	int NO_PARENT_FLAG = -1;
	map<int, int> TableChildWise; // key: child: value: parent
	map<int, list<int> > TableParentWise; // key: parent; value: child(ren)	
	map<int, list<int> > TableChildWiseStar; // key: child; value: parent(s)-star
	map<int, list<int> > TableParentWiseStar; // key: parent; value: child(ren)-star
};