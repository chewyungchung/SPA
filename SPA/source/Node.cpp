#include "Node.h"
using namespace std;

Node::Node(int stmtline) {
	stmtnum = stmtline;
	list<Node> nextList();
}

void Node::addNext(Node n) {
	if (!(find(nextList.begin(), nextList.end(), n) != nextList.end())) {
		nextList.push_back(n);
	}
}

list<Node> Node::getNextList() {
	return nextList;
}
