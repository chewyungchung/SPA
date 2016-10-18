#pragma once
#include <list>
#include <algorithm>

using namespace std;

class Node
{
public:
	Node(int);
	void addNext(Node*);
	list<Node*> getNextList();
	int getStmtnum();
private:
	int stmtnum;
	list<Node*> nextList;
};
