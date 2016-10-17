#include "CFG.h"
#include <assert.h>

CFG::CFG()
{
	unordered_map<int, Node> nodeTable();
	Node *ptr = nullptr;
}

void CFG::addProcCFG()
{
	if (!(stack.empty())) {
		assert(false);
	}
	ptr = nullptr;
}

void CFG::addStmtCFG(int stmtnum, string stmtType) {
	if (stmtType == "assign" || stmtType == "call") {
		if (!ptr) {
			ptr = new Node(stmtnum);
			nodeTable.insert(pair<int, Node>(stmtnum, *ptr));
		}
		else {
			Node *n = new Node(stmtnum);
			nodeTable.insert(pair<int, Node>(stmtnum, (*n)));
			(*ptr).addNext((*n));
			ptr = n;
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
	//assert(false);
}

void CFG::closeIfCFG()
{
	Node *empty = new Node(-1);
	Node(*ptr).addNext(*empty);
	ptr = stack.top();
	stack.pop();
	stack.push(empty);
}

void CFG::closeElseCFG()
{
	Node *cur = stack.top();
	stack.pop();
	Node(*ptr).addNext(*cur);
	ptr = cur;
}

void CFG::closeWhileCFG()
{
	Node *cur = stack.top();
	stack.pop();
	Node(*ptr).addNext(*cur);
	ptr = cur;
}

void CFG::buildCFGMatrix()
{
	int size = nodeTable.size() + 1;
	int** matrix = new int*[size];
	for (int i = 0; i < size; ++i) {
		matrix[i] = new int[size];
	}

	for (auto map : nodeTable) {
		for (Node n : map.second.getNextList()) {
			matrix[map.first][n.getStmtnum()] = 1;
		}
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (matrix[i][j] == 0 && i != j) {
				matrix[i][j] = size;
			}
		}
	}

	for (int k = 0; k < size; k++) {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				if (matrix[i][k] + matrix[k][j] < matrix[i][j])
					matrix[i][j] = matrix[i][k] + matrix[k][j];
			}
		}
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (matrix[i][j] == size) {
				matrix[i][j] = 0;
			}
		}
	}
}

bool CFG::isNext(int n1, int n2) {
	return matrix[n1][n2] == 1;
}

bool CFG::isNextStar(int n1, int n2) {
	return matrix[n1][n2] != 0;
}

bool CFG::isNextEmpty() {
	int size = nodeTable.size() + 1;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (matrix[i][j] != 0) {
				return false;
			}
		}
	}
	return true;
}

list<int> CFG::getExecutedBefore(int n) {
	int size = nodeTable.size() + 1;
	list<int> output;
	for (int i = 0; i < size; ++i) {
		if (matrix[i][n] == 1) {
			output.push_back(i);
		}
	}
	return output;
}

list<int> CFG::getExecutedAfter(int n) {
	int size = nodeTable.size() + 1;
	list<int> output;
	for (int i = 0; i < size; ++i) {
		if (matrix[n][i] == 1) {
			output.push_back(i);
		}
	}
	return output;
}

list<int> CFG::getExecutedBeforeStar(int n) {
	int size = nodeTable.size() + 1;
	list<int> output;
	for (int i = 0; i < size; ++i) {
		if (matrix[i][n] != 0) {
			output.push_back(i);
		}
	}
	return output;
}

list<int> CFG::getExecutedAfterStar(int n) {
	int size = nodeTable.size() + 1;
	list<int> output;
	for (int i = 0; i < size; ++i) {
		if (matrix[n][i] != 0) {
			output.push_back(i);
		}
	}
	return output;
}
