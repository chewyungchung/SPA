#include "CFG.h"
#include <assert.h>
#include <iostream>

CFG::CFG()
{
	unordered_map<int, Node> node_table_ = unordered_map<int, Node>();
	Node *curr_node_ptr_ = nullptr;
}

void CFG::AddProcCFG()
{
	if (!(node_stack_.empty())) {
		assert(false);
	}
	curr_node_ptr_ = nullptr;
}

void CFG::AddStmtCFG(int stmt_num, string stmt_type) {
	if (stmt_type == "assign" || stmt_type == "call") {
		if (!curr_node_ptr_) {
			curr_node_ptr_ = new Node(stmt_num);
			node_table_.insert(pair<int, Node*>(stmt_num, curr_node_ptr_));
		}
		else {
			Node *n = new Node(stmt_num);
			node_table_.insert(pair<int, Node*>(stmt_num, n));
			curr_node_ptr_->AddNext(n);
			curr_node_ptr_ = n;
		}
	}
	else if (stmt_type == "if" || stmt_type == "while") {
		if (!curr_node_ptr_) {
			Node *n = new Node(stmt_num);
			node_table_.insert(pair<int, Node*>(stmt_num, n));
			curr_node_ptr_ = n;
		}
		else {
			Node *n = new Node(stmt_num);
			node_table_.insert(pair<int, Node*>(stmt_num, n));
			curr_node_ptr_->AddNext(n);
			curr_node_ptr_ = n;
		}
		node_stack_.push(curr_node_ptr_);
	}
}

void CFG::CloseIfCFG()
{
	Node *empty = new Node(-1);
	curr_node_ptr_->AddNext(empty);
	// Add to if_to_terminal_table
	if_to_terminal_table_[node_stack_.top()->GetStmtnum()] = empty;
	curr_node_ptr_ = node_stack_.top();
	node_stack_.pop();
	node_stack_.push(empty);
}

void CFG::CloseElseCFG()
{
	Node *cur = node_stack_.top();
	node_stack_.pop();
	curr_node_ptr_->AddNext(cur);
	curr_node_ptr_ = cur;
}

void CFG::CloseWhileCFG()
{
	Node *cur = node_stack_.top();
	node_stack_.pop();
	curr_node_ptr_->AddNext(cur);
	curr_node_ptr_ = cur;
}

void CFG::BuildCFGMatrix()
{
	int size = node_table_.size() + 1;
	cfg_matrix_ = new int*[size];
	for (int i = 0; i < size; ++i) {
		cfg_matrix_[i] = new int[size];
	}

	// init 0
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			cfg_matrix_[i][j] = 0;
		}
	}

	for (auto map : node_table_) {
		for (Node* n : map.second->GetNextList()) {
			while (n->GetStmtnum() == -1) {
				if (n->GetNextList().empty()) {
					break;
				}
				n = n->GetNextList().front();
			}
			if (n->GetStmtnum() != -1) {
				cfg_matrix_[map.first][n->GetStmtnum()] = 1;
			}
		}
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (cfg_matrix_[i][j] != 1 && i != j) {
				cfg_matrix_[i][j] = size;
			}
		}
	}

	for (int k = 0; k < size; k++) {
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				if (cfg_matrix_[i][k] + cfg_matrix_[k][j] < cfg_matrix_[i][j])
					cfg_matrix_[i][j] = cfg_matrix_[i][k] + cfg_matrix_[k][j];
			}
		}
	}

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (cfg_matrix_[i][j] == size) {
				cfg_matrix_[i][j] = 0;
			}
		}
	}

	for (int i = 1; i < size; i++) {
		for (int j : GetExecutedBefore(i)) {
			if (IsNextStar(i, j)) {
				cfg_matrix_[i][i] = 2;
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

bool CFG::IsNext(int n1, int n2) {
	if ((cfg_matrix_[n1][n2] == 1) == true) {
		return true;
	}
	return false;
}

bool CFG::IsNextStar(int n1, int n2) {
	return cfg_matrix_[n1][n2] != 0;
}

bool CFG::IsNextEmpty() {
	int size = node_table_.size() + 1;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (cfg_matrix_[i][j] != 0) {
				return false;
			}
		}
	}
	return true;
}

list<int> CFG::GetExecutedBefore(int n) {
	int size = node_table_.size() + 1;
	list<int> output;
	for (int i = 0; i < size; ++i) {
		if (cfg_matrix_[i][n] == 1) {
			output.push_back(i);
		}
	}
	return output;
}

list<int> CFG::GetExecutedAfter(int n) {
	int size = node_table_.size() + 1;
	list<int> output;
	for (int i = 0; i < size; ++i) {
		if (cfg_matrix_[n][i] == 1) {
			output.push_back(i);
		}
	}
	return output;
}

list<int> CFG::GetExecutedBeforeStar(int n) {
	int size = node_table_.size() + 1;
	list<int> output;
	for (int i = 0; i < size; ++i) {
		if (cfg_matrix_[i][n] != 0) {
			output.push_back(i);
		}
	}
	return output;
}

list<int> CFG::GetExecutedAfterStar(int n) {
	int size = node_table_.size() + 1;
	list<int> output;
	for (int i = 0; i < size; ++i) {
		if (cfg_matrix_[n][i] != 0) {
			output.push_back(i);
		}
	}
	return output;
}

Node CFG::GetNodeByStmt(int stmt_num)
{
	if (node_table_[stmt_num] != nullptr) {
		return *(node_table_[stmt_num]);
	}
	else {
		return Node(-1);
	}
}

Node CFG::GetTerminalNodeByStmt(int if_stmt_num)
{
	if (if_to_terminal_table_[if_stmt_num] != nullptr) {
		return *(if_to_terminal_table_[if_stmt_num]);
	}
	else {
		return Node(-10);
	}
}