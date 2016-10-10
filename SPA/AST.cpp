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
				throw std::runtime_error("missing left paren");
			}
			stack.pop();
			postfix << ' ';
		}
		else {
			throw std::runtime_error("invalid input character");
		}
	}

	while (!stack.empty() && '(' != stack.top()) {
		postfix << ' ' << stack.top();
		stack.pop();
	}
	if (stack.empty()) {
		throw std::runtime_error("missing left paren");
	}
	stack.pop();

	if (!stack.empty()) {
		throw std::runtime_error("missing right paren");
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