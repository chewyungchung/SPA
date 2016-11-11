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
	string MakeExpr(string input);
	void AST::AddExpr(int stmt, string expr);
	bool IsExprExist(string expr);
	bool IsSubExprExist(string sub_expr);
	list<int> GetAssignWithExpression(string expr);
	list<int> GetAssignWithSubExpression(string sub_expr);

private:
	unordered_map<int, string> ast_table_;
	bool IsOperator(char current_char);
	bool Precedence(char left_operator, char right_operator);
};
