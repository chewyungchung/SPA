#include "AST.h"
using namespace std;

AST::AST() {
	ASTTable = unordered_map<int, string>();
}

void AST::addExpr(int stmt, string expr) {
	ASTTable.insert({ stmt, expr });
}

bool AST::isExprExist(string expr) {
	for (auto pair : ASTTable) {
		if (pair.second == expr) {
			return true;
		}
	}
	return false;
}

bool AST::isSubExprExist(string subExpr) {
	for (auto pair : ASTTable) {
		if (pair.second.find(subExpr) != string::npos) {
			return true;
		}
	}
	return false;
}

list<int> AST::getAssignWithExpression(string expr)
{
	list<int> output;
	for (auto pair : ASTTable) {
		if (pair.second == expr) {
			output.push_back(pair.first);
		}
	}
	return output;
}

list<int> AST::getAssignWithSubExpression(string subExpr)
{
	list<int> output;
	for (auto pair : ASTTable) {
		if (pair.second.find(subExpr) != string::npos) {
			output.push_back(pair.first);
		}
	}
	return output;
}

string AST::makeExpr(string infix) {
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

		else if (isOperator(current)) {
			char rightOperator = current;
			while (!stack.empty() && isOperator(stack.top()) && precedence(stack.top(), rightOperator)) {
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

	return postfix.str();
}

bool AST::isOperator(char currentChar) {
	switch (currentChar) {
	case '+':
	case '-':
	case '*':
		return true;
	default:
		return false;
	}
}

bool AST::precedence(char leftOperator, char rightOperator) {
	if (leftOperator == '*') {
		return true;
	}
	else if (rightOperator == '*') {
		return false;
	}
	return true;
}