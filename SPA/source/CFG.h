#pragma once
#include <string>
#include <unordered_map>
#include <stack>
#include <list>
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
	list<int> getExecutedBefore(int n);
	list<int> getExecutedAfter(int n);
	list<int> getExecutedBeforeStar(int n);
	list<int> getExecutedAfterStar(int n);
	Node getNodeByStmt(int stmtNum);
	Node getTerminalNodeByStmt(int if_stmt_num);

private:
	unordered_map<int, Node*> nodeTable;
	unordered_map<int, Node*> if_to_terminal_table;
	stack<Node*> stack;
	Node *ptr;
	int** matrix;
};