#include "CFG.h"
#include <assert.h>
#include <iostream>

CFG::CFG()
{
	unordered_map<int, Node> nodeTable = unordered_map<int, Node>();
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
			nodeTable.insert(pair<int, Node*>(stmtnum, ptr));
		}
		else {
			Node *n = new Node(stmtnum);
			nodeTable.insert(pair<int, Node*>(stmtnum, n));
			ptr->addNext(n);
			ptr = n;
		}
	}
	else if (stmtType == "if" || stmtType == "while") {
		if (!ptr) {
			Node *n = new Node(stmtnum);
			nodeTable.insert(pair<int, Node*>(stmtnum, n));
			ptr = n;
		}
		else {
			Node *n = new Node(stmtnum);
			nodeTable.insert(pair<int, Node*>(stmtnum, n));
			ptr->addNext(n);
			ptr = n;
		}
		stack.push(ptr);
	}
}

void CFG::closeIfCFG()
{
	Node *empty = new Node(-1);
	ptr->addNext(empty);
	ptr = stack.top();
	stack.pop();
	stack.push(empty);
}

void CFG::closeElseCFG()
{
	Node *cur = stack.top();
	stack.pop();
	ptr->addNext(cur);
	ptr = cur;
}

void CFG::closeWhileCFG()
{
	Node *cur = stack.top();
	stack.pop();
	ptr->addNext(cur);
	ptr = cur;
}

void CFG::buildCFGMatrix()
{
	int size = nodeTable.size() + 1;
	matrix = new int*[size];
	for (int i = 0; i < size; ++i) {
		matrix[i] = new int[size];
	}

	// init 0
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			matrix[i][j] = 0;
		}
	}

	for (auto map : nodeTable) {
		for (Node* n : map.second->getNextList()) {
			while (n->getStmtnum() == -1) {
				if (n->getNextList().empty()) {
					break;
				}
				n = n->getNextList().front();
			}
			if (n->getStmtnum() != -1) {
				matrix[map.first][n->getStmtnum()] = 1;
			}
		}
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (matrix[i][j] != 1 && i != j) {
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

	for (int i = 1; i < size; i++) {
		for (int j : getExecutedBefore(i)) {
			if (isNextStar(i, j)) {
				matrix[i][i] = 2;
				break;
			}
		}
	}
	//cout << "PRINTING FLOYD YO!!!!!!!!!!!!!!!!!1" << endl;
	//// Print matrix to see
	//for (int i = 0; i < size; i++) {
	//	cout << "   ";
	//	for (int j = 0; j < size; j++) {
	//		std::cout << matrix[i][j];
	//		if (j != size) {
	//			cout << " ";
	//		}
	//	}
	//	cout << endl;
	//}
	//cout << endl;
}

bool CFG::isNext(int n1, int n2) {
	if ((matrix[n1][n2] == 1) == true) {
		return true;
	}
	return false;
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

unordered_map<int, Node*> CFG::getNodeTable() {
	return nodeTable;
}