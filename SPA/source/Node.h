#pragma once
#include <vector>

using namespace std;

class Node
{
public:
	Node();
	Node(int);
	void AddNext(Node*);
	vector<Node*> GetNextList();
	int GetStmtnum();
private:
	int stmt_num_;
	vector<Node*> next_list_;
};
