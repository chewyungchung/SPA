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
	void AddProcCFG();
	void AddStmtCFG(int stmt_num, string stmt_type);
	void CloseIfCFG();
	void CloseElseCFG();
	void CloseWhileCFG();
	void BuildCFGMatrix();

	bool IsNext(int n1, int n2);
	bool IsNextStar(int n1, int n2);
	bool IsNextEmpty();
	list<int> GetExecutedBefore(int n);
	list<int> GetExecutedAfter(int n);
	list<int> GetExecutedBeforeStar(int n);
	list<int> GetExecutedAfterStar(int n);
	Node GetNodeByStmt(int stmt_num);
	Node GetTerminalNodeByStmt(int if_stmt_num);

private:
	unordered_map<int, Node*> node_table_;
	unordered_map<int, Node*> if_to_terminal_table_;
	stack<Node*> node_stack_;
	Node *curr_node_ptr_;
	int** cfg_matrix_;
};