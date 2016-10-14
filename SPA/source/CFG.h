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
private:
	unordered_map<int, Node> nodeTable;
	stack<Node*> stack;
	Node *ptr;
};