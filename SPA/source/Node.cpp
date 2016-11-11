#include "Node.h"
using namespace std;

Node::Node() {
	stmt_num_ = -1;
}

Node::Node(int stmtline) {
	stmt_num_ = stmtline;
}

void Node::AddNext(Node* n) {
	next_list_.push_back(n);
}

vector<Node*> Node::GetNextList() {
	return next_list_;
}

int Node::GetStmtnum()
{
	return stmt_num_;
}
