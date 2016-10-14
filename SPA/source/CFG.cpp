#include "CFG.h"
#include <assert.h>

CFG::CFG()
{
	unordered_map<int, Node> nodeTable();
	Node *ptr = NULL;
}

void CFG::addProcCFG()
{
	if (!(stack.empty)) {
		assert(false);
	}
	*ptr = NULL;
}

void CFG::addStmtCFG(int stmtnum, string stmtType) {
	if (stmtType == "assign" || stmtType == "calls") {
		if (!ptr) {
			Node n(stmtnum);
			nodeTable.insert(pair<int, Node>(stmtnum, n));
			ptr = &n;
		}
		else {
			Node n(stmtnum);
			nodeTable.insert(pair<int, Node>(stmtnum, n));
			Node(*ptr).addNext(n);
			ptr = &n;
		}
	}
	else if (stmtType == "if" || stmtType == "while") {
		if (!ptr) {
			Node n(stmtnum);
			nodeTable.insert(pair<int, Node>(stmtnum, n));
			ptr = &n;
		}
		else {
			Node n(stmtnum);
			nodeTable.insert(pair<int, Node>(stmtnum, n));
			Node(*ptr).addNext(n);
			ptr = &n;
		}
		stack.push(ptr);
	}
	assert(false);
}

void CFG::closeIfCFG()
{
	Node empty(-1);
	Node(*ptr).addNext(empty);
	ptr = stack.pop;
	stack.push(&empty);
}

void CFG::closeElseCFG()
{
	Node cur = stack.pop;
	Node(*ptr).addNext(cur);
	ptr = &cur;
}

void CFG::closeWhileCFG()
{
	Node cur = stack.pop;
	Node(*ptr).addNext(cur);
	ptr = &cur;
}

void CFG::buildCFGMatrix()
{
}
