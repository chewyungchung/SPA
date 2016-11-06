#pragma once
#include <vector>
#include <algorithm>

using namespace std;

class Node
{
public:
	Node();
	Node(int);
	void addNext(Node*);
	vector<Node*> getNextList();
	int getStmtnum();
private:
	int stmtnum;
	vector<Node*> nextList;
};
