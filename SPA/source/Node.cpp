#include "Node.h"
using namespace std;

Node::Node(int stmtline) {
	stmtnum = stmtline;
}

void Node::addNext(Node* n) {
	/*for (Node i : nextList) {
		int x = i.stmtnum;
		if (x == n.stmtnum) {
			return;
		}
	}*/
	nextList.push_back(n);
}

list<Node*> Node::getNextList() {
	return nextList;
}

int Node::getStmtnum()
{
	return stmtnum;
}
