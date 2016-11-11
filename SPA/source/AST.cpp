#include "AST.h"
using namespace std;

AST::AST() {
	ast_table_ = unordered_map<int, string>();
}

void AST::AddExpr(int stmt, string expr) {
	ast_table_.insert({ stmt, expr });
}

bool AST::IsExprExist(string expr) {
	for (auto pair : ast_table_) {
		if (pair.second == expr) {
			return true;
		}
	}
	return false;
}

bool AST::IsSubExprExist(string sub_expr) {
	for (auto pair : ast_table_) {
		if (pair.second.find(sub_expr) != string::npos) {
			return true;
		}
	}
	return false;
}

list<int> AST::GetAssignWithExpression(string expr)
{
	list<int> output;
	expr = MakeExpr(expr);
	for (auto pair : ast_table_) {
		if (pair.second == expr) {
			output.push_back(pair.first);
		}
	}
	return output;
}

list<int> AST::GetAssignWithSubExpression(string subExpr)
{
	list<int> output;
	subExpr = MakeExpr(subExpr);
	for (auto pair : ast_table_) {
		if (pair.second.find(subExpr) != string::npos) {
			output.push_back(pair.first);
		}
	}
	return output;
}

string AST::MakeExpr(string infix) {
	std::stringstream postfix;
	std::stack<char> stack;
	stack.push('(');

	for (std::size_t i = 0, l = infix.size(); i < l; ++i) {
		const char current = infix[i];

		if (isspace(current)) {
		}
		else if (isalnum(current) || '.' == current) {
			postfix << current;
		}

		else if ('(' == current) {
			stack.push(current);
		}

		else if (IsOperator(current)) {
			char rightOperator = current;
			while (!stack.empty() && IsOperator(stack.top()) && Precedence(stack.top(), rightOperator)) {
				postfix << ' ' << stack.top();
				stack.pop();
			}
			postfix << ' ';
			stack.push(rightOperator);
		}

		else if (')' == current) {
			while (!stack.empty() && '(' != stack.top()) {
				postfix << ' ' << stack.top();
				stack.pop();
			}
			if (stack.empty()) {
				return NULL;
			}
			stack.pop();
			postfix << ' ';
		}
		else {
			return NULL;
		}
	}

	while (!stack.empty() && '(' != stack.top()) {
		postfix << ' ' << stack.top();
		stack.pop();
	}
	if (stack.empty()) {
		return NULL;
	}
	stack.pop();

	if (!stack.empty()) {
		return NULL;
	}

	return " " + postfix.str() + " ";
}

bool AST::IsOperator(char current_char) {
	switch (current_char) {
	case '+':
	case '-':
	case '*':
		return true;
	default:
		return false;
	}
}

bool AST::Precedence(char left_operator, char right_operator) {
	if (left_operator == '*') {
		return true;
	}
	else if (right_operator == '*') {
		return false;
	}
	return true;
}