#pragma once
#include <unordered_map>
#include <stdexcept>
#include <cctype>
#include <sstream>
#include <stack>
using namespace std;

class AST {
public:
	AST();
	string makeExpr(string input);
	void AST::addExpr(int stmt, string expr);
	bool isExprExist(string expr);
	bool isSubExprExist(string subExpr);
	list<int> getAssignWithExpression(string expr);
	list<int> getAssignWithSubExpression(string subExpr);

private:
	unordered_map<int, string> ASTTable;
	bool isOperator(char currentChar);
	bool precedence(char leftOperator, char rightOperator);
};
