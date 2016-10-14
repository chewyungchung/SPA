#pragma once
#include <string>
#include <unordered_map>
#include <stack>
#include "Node.h"

using namespace std;

class CFG
{
public:
	CFG();
	void addProcCFG();
	void addStmtCFG(int stmtnum, string stmtType);
	void closeIfCFG();
	void closeElseCFG();
	void closeWhileCFG();
	void buildCFGMatrix();

	bool isNext(int n1, int n2);
	bool isNextStar(int n1, int n2);
	bool isNextEmpty();
	int getExecutedBefore(int n);
	int getExecutedAfter(int n);
	list<int> getExecutedBeforeStar(int n);
	list<int> getExecutedAfterStar(int n);

private:
	unordered_map<int, Node> nodeTable;
	stack<Node*> stack;
	Node *ptr;
};