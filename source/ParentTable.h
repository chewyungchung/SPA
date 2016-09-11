#pragma once

#include <unordered_map>
#include <list>
#include <iostream>
using namespace std;

class ParentTable {
private:
	unordered_map<int, int> TableChildWise; // key: child: value: parent
	unordered_map<int, list<int> > TableParentWise; // key: parent; value: child(ren)
	unordered_map<int, list<int> > TableChildWiseStar; // key: child; value: parent(s)-star
public:
	ParentTable();
	~ParentTable();

	void addParent(int parent, int child);

	int getParentOf(int stmt);
	list<int> getChildrenOf(int stmt);
	bool isParentEmpty();
	bool isParentOf(int parent, int child);
	list<int> getParentStar(int stmt);
	bool isParentStar(int parent, int child);
};