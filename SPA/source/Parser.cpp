/*
Parser operates by recursive descent. Design abstraction states are tracked by global data structures.
*/
#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <stack>
#include <vector>

using namespace std;

#include "Parser.h"
#include "Tokenizer.h"
#include "PKB.h"

Parser::Parser() {
}

Parser::Parser(string file_name)
{
	pkb_ = PKB();
	tokenizer_ = Tokenizer(file_name);
	curr_stmt_line_ = 1;
	parent_stack_.push(NO_PARENT_FLAG);
	follows_max_nest_lvl_ = 1;
	follows_stack_.push(follows_max_nest_lvl_);
}

PKB Parser::Process() {
	ParseProgram();
	return pkb_;
}

void Parser::Match(string token)
{
	if (next_token_ == token)
	{
		next_token_ = tokenizer_.GetNextToken();
	}
	else
	{
		throw invalid_argument("SIMPLE syntax error, check SIMPLE source");
	}
}

void Parser::ParseProgram()
{
	next_token_ = tokenizer_.GetNextToken();
	ParseProcedure();

	/******** Program parsed ********/
	// Postfixing -> setup design extractor
	list<string> proc_list = pkb_.GetProcedureList();
	int proc_count = proc_list.size();
	design_extractor_ = DesignExtractor(pkb_, proc_count);
	// Check cyclic call
	design_extractor_.BuildCallsGraph(proc_count);
	// Update mod/uses for all procedures if acyclic
	if (!pkb_.IsCallsGraphCyclic())
	{
		design_extractor_.UpdateAllProcModUses();
	}
	else
	{
		throw invalid_argument("SIMPLE syntax error, recursive calls");
	}
	// Now update callStmts for Mod/Uses as well as all parentStar of the callStmts
	design_extractor_.UpdateAllCallStmtModUses();

	// build CFG matrix
    pkb_.BuildCFGMatrix();
}

void Parser::ParseProcedure()
{
	// Add first stmt of proc
	curr_first_stmt_of_proc_ = curr_stmt_line_;
	pkb_.AddProcedureFirstStmt(curr_first_stmt_of_proc_);

	// Add stmt lst first stmt
	pkb_.AddStmtLst(curr_stmt_line_);

	Match(PROCEDURE_FLAG);
	proc_name_ = next_token_;

	pkb_.AddProc(proc_name_);
	pkb_.AddProcCFG();

	Match(proc_name_);
	Match(LEFT_BRACES);
	ParseStmtLst();
	Match(RIGHT_BRACES);
	// _pkb.closeProcCFG();
	if (next_token_ == PROCEDURE_FLAG)
	{
		// Entering into new procedure,
		// thus enter into a new (highest) nestingLevel
		// ParentStack would rightly have only NO_PARENT_FLAG at this point
		follows_max_nest_lvl_++;
		follows_stack_.pop();
		follows_stack_.push(follows_max_nest_lvl_);

		ParseProcedure();
	}
}

void Parser::ParseStmtLst()
{
	if (next_token_ == WHILE_FLAG)
	{
		ParseWhileStmt();
	}
	else if (next_token_ == IF_FLAG)
	{
		ParseIfStmt();
		Match(ELSE_FLAG);
		ParseElseStmt();
	}
	else if (next_token_ == CALL_FLAG)
	{
		ParseCallStmt();
		Match(SEMICOLON_FLAG);
		curr_stmt_line_++;
	}
	else
	{
		ParseAssignStmt();
		Match(SEMICOLON_FLAG);
		curr_stmt_line_++;
	}

	if (next_token_ == RIGHT_BRACES)
	{
		return;
	}
	else
	{
		ParseStmtLst();
	}
}

void Parser::ParseWhileStmt()
{
	// Update stmt_to_proc_begin_table
	pkb_.AddStmtProcBegin(curr_stmt_line_, curr_first_stmt_of_proc_);

	// Populate CFG
	pkb_.AddStmtCFG(curr_stmt_line_, WHILE_FLAG);

	// Populate ParentTable for current while stmt and set current while stmt as current parent
	pkb_.AddParent(parent_stack_.top(), curr_stmt_line_);
	AddAllParentsOfCurrStmt(curr_stmt_line_);
	parent_stack_.push(curr_stmt_line_);

	// Populate FollowsTable for current while stmt and enter into new nesting level
	pkb_.AddFollows(curr_stmt_line_, follows_stack_.top());
	follows_max_nest_lvl_++;
	follows_stack_.push(follows_max_nest_lvl_);

	// Populate UsesTable for control variable
	// This involves adding current stmtLine as well as any parent/parent* stmtLines
	Match(WHILE_FLAG);
	string controlVar = next_token_;
	pkb_.AddUses(controlVar, curr_stmt_line_);
	pkb_.AddUses(curr_stmt_line_, controlVar);
	AddAllParentsOfUsedVariable(controlVar);

	// Populate ProcModUsesTable
	pkb_.AddProcUses(proc_name_, controlVar);

	// Populate StatementTable
	pkb_.AddStatement(curr_stmt_line_, WHILE_FLAG, controlVar);
	
	// Recurse on stmtLst
	Match(controlVar);
	Match(LEFT_BRACES);
	curr_stmt_line_++;

	// Add stmtlst first stmt
	pkb_.AddStmtLst(curr_stmt_line_);

	ParseStmtLst();
	Match(RIGHT_BRACES);
	//stmtLine++;

	// Exiting from while loop: remove from current parent and exit nesting level
	// As well inform CFG of exit
	parent_stack_.pop();
	follows_stack_.pop();
	pkb_.CloseWhileCFG();
}

void Parser::ParseIfStmt()
{
	// Update stmt_to_proc_begin_table
	pkb_.AddStmtProcBegin(curr_stmt_line_, curr_first_stmt_of_proc_);

	// Populate CFG
	pkb_.AddStmtCFG(curr_stmt_line_, IF_FLAG);

	// Populate ParentTable for current if stmt and set current if stmt as current parent
	pkb_.AddParent(parent_stack_.top(), curr_stmt_line_);
	AddAllParentsOfCurrStmt(curr_stmt_line_);
	parent_stack_.push(curr_stmt_line_);

	// Populate FollowsTable for current while stmt and enter into new nesting level
	pkb_.AddFollows(curr_stmt_line_, follows_stack_.top());
	follows_max_nest_lvl_++;
	follows_stack_.push(follows_max_nest_lvl_);

	// Populate UsesTable for control variable
	// This involves adding current stmtLine as well as any parent/parent* stmtLines
	Match(IF_FLAG);
	string ctrl_var = next_token_;
	pkb_.AddUses(ctrl_var, curr_stmt_line_);
	pkb_.AddUses(curr_stmt_line_, ctrl_var);
	AddAllParentsOfUsedVariable(ctrl_var);

	// Populate ProcModUsesTable
	pkb_.AddProcUses(proc_name_, ctrl_var);

	// Populate StatementTable
	pkb_.AddStatement(curr_stmt_line_, IF_FLAG, ctrl_var);

	// Recurse on stmtLst
	Match(ctrl_var);
	Match(THEN_FLAG);
	Match(LEFT_BRACES);
	curr_stmt_line_++;

	// Add stmt lst first stmt
	pkb_.AddStmtLst(curr_stmt_line_);

	ParseStmtLst();
	Match(RIGHT_BRACES);

	// Exiting from if: exit nesting level
	// As well inform CFG of exit
	follows_stack_.pop();
	pkb_.CloseIfCFG();
}

void Parser::ParseElseStmt()
{
	Match(LEFT_BRACES);

	// Enter into new nestingLevel
	follows_max_nest_lvl_++;
	follows_stack_.push(follows_max_nest_lvl_);

	// Add stmtLst first stmt
	pkb_.AddStmtLst(curr_stmt_line_);

	ParseStmtLst();
	Match(RIGHT_BRACES);

	// Exiting else, pop if parent, exit nestingLevel, updateCFG
	parent_stack_.pop();
	follows_stack_.pop();
	pkb_.CloseElseCFG();
}

void Parser::ParseCallStmt()
{
	// Update stmt_to_proc_begin_table
	pkb_.AddStmtProcBegin(curr_stmt_line_, curr_first_stmt_of_proc_);

	// Populate ParentTable and FollowsTable for this call stmt
	pkb_.AddParent(parent_stack_.top(), curr_stmt_line_);
	AddAllParentsOfCurrStmt(curr_stmt_line_);
	pkb_.AddFollows(curr_stmt_line_, follows_stack_.top());

	// Populate CFG
	pkb_.AddStmtCFG(curr_stmt_line_, CALL_FLAG);

	Match(CALL_FLAG);

	string caller = proc_name_;
	string callee = next_token_;

	// Populate ProcTable
	pkb_.AddProcCalledInStmt(callee, curr_stmt_line_);

	// Populate StatementTable
	pkb_.AddStatement(curr_stmt_line_, CALL_FLAG, callee);

	Match(callee);

	// Populate CallTable
	pkb_.AddCalls(caller, callee);
}

void Parser::ParseAssignStmt()
{
	// Update stmt_to_proc_begin_table
	pkb_.AddStmtProcBegin(curr_stmt_line_, curr_first_stmt_of_proc_);

	// Populate StatementTable
	pkb_.AddStatement(curr_stmt_line_, ASSIGN_FLAG);

	// Populate CFG
	pkb_.AddStmtCFG(curr_stmt_line_, ASSIGN_FLAG);

	// Populate ParentTable and FollowsTable for this assign stmt
	pkb_.AddParent(parent_stack_.top(), curr_stmt_line_);
	AddAllParentsOfCurrStmt(curr_stmt_line_);
	pkb_.AddFollows(curr_stmt_line_, follows_stack_.top());

	// Populate mod table with LHS variable
	// This involves adding current stmtLine as well as any parent/parent* stmtLines
	string LHS = next_token_;
	pkb_.AddModifies(LHS, curr_stmt_line_);
	pkb_.AddModifies(curr_stmt_line_, LHS);
	AddAllParentsOfModifiedVariable(LHS);

	// Populate ProcTable
	pkb_.AddProcMod(proc_name_, LHS);

	// Move to RHS and populate uses(var)/const table accordingly
	// Involves adding current stmtLine as well as any parent/parent* stmtLines
	Match(LHS);
	Match(EQUAL_FLAG);
	curr_rhs_ = "";
	ParseExpression();
	//parseAssignRHS();
	// Check brackets are correct
	if (!bracket_stack_.empty())
	{
		throw invalid_argument("SIMPLE syntax error, check parenthesis");
	}
	string postFixRHS = pkb_.MakeExpr(curr_rhs_);
	pkb_.AddExpr(curr_stmt_line_, postFixRHS);
}

void Parser::ParseAssignRHS()
{
	string RHS = next_token_;
	curr_rhs_ += RHS;
	curr_rhs_ += " ";

	if (IsConstant(RHS))
	{
		int RHSConstant = stoi(RHS);
		pkb_.AddConstant(RHSConstant, curr_stmt_line_);
		//addAllParentsOfUsedConstant(RHSConstant);
	}
	else
	{
		pkb_.AddUses(RHS, curr_stmt_line_);
		pkb_.AddUses(curr_stmt_line_, RHS);
		AddAllParentsOfUsedVariable(RHS);

		pkb_.AddProcUses(proc_name_, RHS);
	}
	Match(RHS);
	if (next_token_ == PLUS_FLAG || next_token_ == MINUS_FLAG || next_token_ == TIMES_FLAG)
	{
		curr_rhs_ += next_token_;
		curr_rhs_ += " ";
		Match(next_token_);
		ParseAssignRHS();
	}
	else if (next_token_ == LEFT_PARENTHESIS)
	{
		bracket_stack_.push(next_token_);
		curr_rhs_ += next_token_;
		curr_rhs_ += " ";
		Match(next_token_);
		ParseAssignRHS();
	}
	else if (next_token_ == RIGHT_PARENTHESIS)
	{
		if (!bracket_stack_.empty())
		{
			bracket_stack_.pop();
			curr_rhs_ += next_token_;
			curr_rhs_ += " ";
			Match(next_token_);
			ParseAssignRHS();
		}
		else
		{
			throw invalid_argument("SIMPLE syntax error, check parenthesis");

		}
	}
	else
	{
		return;
	}
}

void Parser::ParseExpression()
{
	if (next_token_ == PLUS_FLAG || next_token_ == MINUS_FLAG) {
		curr_rhs_ += next_token_;
		Match(next_token_);
	}
	ParseTerm();
	while (next_token_ == PLUS_FLAG || next_token_ == MINUS_FLAG) {
		curr_rhs_ += next_token_;
		Match(next_token_);
		ParseTerm();
	}
}

void Parser::ParseTerm()
{
	ParseFactor();
	while (next_token_ == TIMES_FLAG) {
		curr_rhs_ += TIMES_FLAG;
		Match(TIMES_FLAG);
		ParseFactor();
	}

}

void Parser::ParseFactor()
{
	if (IsConstant(next_token_) == true) {
		// INT
		curr_rhs_ += next_token_;
		int RHSConstant = stoi(next_token_);
		pkb_.AddConstant(RHSConstant, curr_stmt_line_);
		Match(next_token_);
	}
	else if (next_token_ == LEFT_PARENTHESIS) {
		// (
		curr_rhs_ += LEFT_PARENTHESIS;
		Match(LEFT_PARENTHESIS);
		ParseExpression();
		curr_rhs_ += RIGHT_PARENTHESIS;
		Match(RIGHT_PARENTHESIS);
	}
	else {
		// IDENT
		curr_rhs_ += next_token_;
		pkb_.AddUses(next_token_, curr_stmt_line_);
		pkb_.AddUses(curr_stmt_line_, next_token_);
		AddAllParentsOfUsedVariable(next_token_);
		pkb_.AddProcUses(proc_name_, next_token_);
		Match(next_token_);
	}
}

bool Parser::IsConstant(string s)
{
	if (isdigit(s.at(0))) {
		return true;
	}
	return false;
}

void Parser::AddAllParentsOfCurrStmt(int stmtLine)
{
	stack<int> temp_stack;
	int temp;
	while (parent_stack_.top() != NO_PARENT_FLAG)
	{
		temp = parent_stack_.top();
		pkb_.AddParentStar(temp, stmtLine);
		parent_stack_.pop();
		temp_stack.push(temp);
	}
	while (!temp_stack.empty())
	{
		temp = temp_stack.top();
		parent_stack_.push(temp);
		temp_stack.pop();
	}
}

void Parser::AddAllParentsOfUsedVariable(string v)
{
	stack<int> temp_stack;
	int temp;
	while (parent_stack_.top() != NO_PARENT_FLAG)
	{
		temp = parent_stack_.top();
		pkb_.AddUses(v, temp);
		pkb_.AddUses(temp, v);
		parent_stack_.pop();
		temp_stack.push(temp);
	}
	while (!temp_stack.empty())
	{
		temp = temp_stack.top();
		parent_stack_.push(temp);
		temp_stack.pop();
	}
}

void Parser::AddAllParentsOfModifiedVariable(string v)
{
	stack<int> temp_stack;
	int temp;
	while (parent_stack_.top() != NO_PARENT_FLAG)
	{
		temp = parent_stack_.top();
		pkb_.AddModifies(v, temp);
		pkb_.AddModifies(temp, v);
		parent_stack_.pop();
		temp_stack.push(temp);
	}
	while (!temp_stack.empty())
	{
		temp = temp_stack.top();
		parent_stack_.push(temp);
		temp_stack.pop();
	}
}

void Parser::AddAllParentsOfUsedConstant(int c)
{
	stack<int> temp_stack;
	int temp;
	while (parent_stack_.top() != NO_PARENT_FLAG)
	{
		temp = parent_stack_.top();
		pkb_.AddConstant(c, temp);
		parent_stack_.pop();
		temp_stack.push(temp);
	}
	while (!temp_stack.empty())
	{
		temp = temp_stack.top();
		parent_stack_.push(temp);
		temp_stack.pop();
	}
}
