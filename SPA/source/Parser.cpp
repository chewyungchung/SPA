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

Parser::Parser(string fileName)
{
	_pkb = PKB();
	_tk = Tokenizer(fileName);
	stmtLine = 1;
	parentStack.push(NO_PARENT_FLAG);
	followsMaxNestingLevel = 1;
	followsStack.push(followsMaxNestingLevel);
}

PKB Parser::process() {
	parseProgram();
	return _pkb;
}

void Parser::match(string token)
{
	if (next_token == token)
	{
		next_token = _tk.getNextToken();
	}
	else
	{
		throw invalid_argument("SIMPLE syntax error, check SIMPLE source");
	}
}

void Parser::parseProgram()
{
	next_token = _tk.getNextToken();
	parseProcedure();

	// postfixing -> check cyclic call 
	// update moduses for proc if acyclic
	_pkb.buildCallsGraph(_pkb.getStatementCount(), _pkb);
	if (!_pkb.isCallsGraphCyclic())
	{
		_pkb.updateAllProcModUses();
	}
	else
	{
		throw invalid_argument("SIMPLE syntax error, recursive calls");
	}

	// build CFG matrix
	_pkb.buildCFGMatrix();
}

void Parser::parseProcedure()
{
	match(PROCEDURE_FLAG);
	procName = next_token;

	_pkb.addProc(procName);
	_pkb.addProcCFG();

	match(procName);
	match(LEFT_BRACES);
	parseStmtLst();
	match(RIGHT_BRACES);
	_pkb.closeProcCFG();
	if (next_token == PROCEDURE_FLAG)
	{
		// Entering into new procedure,
		// thus enter into a new (highest) nestingLevel
		// ParentStack would rightly have only NO_PARENT_FLAG at this point
		followsMaxNestingLevel++;
		followsStack.pop();
		followsStack.push(followsMaxNestingLevel);

		parseProcedure();
	}
}

void Parser::parseStmtLst()
{
	if (next_token == WHILE_FLAG)
	{
		parseWhileStmt();
	}
	else if (next_token == IF_FLAG)
	{
		parseIfStmt();
		match(ELSE_FLAG);
		parseElseStmt();
	}
	else if (next_token == CALL_FLAG)
	{
		parseCallStmt();
		match(SEMICOLON_FLAG);
		stmtLine++;
	}
	else
	{
		parseAssignStmt();
		match(SEMICOLON_FLAG);
		stmtLine++;
	}

	if (next_token == RIGHT_BRACES)
	{
		return;
	}
	else
	{
		parseStmtLst();
	}
}

void Parser::parseWhileStmt()
{
	// Populate CFG
	_pkb.addStmtCFG(stmtLine, WHILE_FLAG);

	// Populate ParentTable for current while stmt and set current while stmt as current parent
	_pkb.addParent(parentStack.top(), stmtLine);
	parentStack.push(stmtLine);

	// Populate FollowsTable for current while stmt and enter into new nesting level
	_pkb.addFollows(stmtLine, followsStack.top());
	followsMaxNestingLevel++;
	followsStack.push(followsMaxNestingLevel);

	// Populate UsesTable for control variable
	// This involves adding current stmtLine as well as any parent/parent* stmtLines
	match(WHILE_FLAG);
	string controlVar = next_token;
	_pkb.addUses(controlVar, stmtLine);
	_pkb.addUses(stmtLine, controlVar);
	addAllParentsOfUsedVariable(controlVar);

	// Populate ProcModUsesTable
	_pkb.addProcUses(procName, controlVar);

	// Populate StatementTable
	_pkb.addStatement(stmtLine, WHILE_FLAG, controlVar);
	
	// Recurse on stmtLst
	match(controlVar);
	match(LEFT_BRACES);
	stmtLine++;
	parseStmtLst();
	match(RIGHT_BRACES);
	//stmtLine++;

	// Exiting from while loop: remove from current parent and exit nesting level
	// As well inform CFG of exit
	parentStack.pop();
	followsStack.pop();
	_pkb.closeWhileCFG();
}

void Parser::parseIfStmt()
{
	// Populate CFG
	_pkb.addStmtCFG(stmtLine, IF_FLAG);

	// Populate ParentTable for current while stmt and set current while stmt as current parent
	_pkb.addParent(parentStack.top(), stmtLine);
	parentStack.push(stmtLine);

	// Populate FollowsTable for current while stmt and enter into new nesting level
	_pkb.addFollows(stmtLine, followsStack.top());
	followsMaxNestingLevel++;
	followsStack.push(followsMaxNestingLevel);

	// Populate UsesTable for control variable
	// This involves adding current stmtLine as well as any parent/parent* stmtLines
	match(IF_FLAG);
	string controlVar = next_token;
	_pkb.addUses(controlVar, stmtLine);
	_pkb.addUses(stmtLine, controlVar);
	addAllParentsOfUsedVariable(controlVar);

	// Populate ProcModUsesTable
	_pkb.addProcUses(procName, controlVar);

	// Populate StatementTable
	_pkb.addStatement(stmtLine, IF_FLAG, controlVar);

	// Recurse on stmtLst
	match(controlVar);
	match(THEN_FLAG);
	match(LEFT_BRACES);
	stmtLine++;
	parseStmtLst();
	match(RIGHT_BRACES);

	// Exiting from if: exit nesting level
	// As well inform CFG of exit
	followsStack.pop();
	_pkb.closeIfCFG();
}

void Parser::parseElseStmt()
{
	match(LEFT_BRACES);

	// Enter into new nestingLevel
	followsMaxNestingLevel++;
	followsStack.push(followsMaxNestingLevel);

	parseStmtLst();
	match(RIGHT_BRACES);

	// Exiting else, pop parent, exit nestingLevel, updateCFG
	parentStack.pop();
	followsStack.pop();
	_pkb.closeElseCFG();
}

void Parser::parseCallStmt()
{
	// Populate StatementTable
	_pkb.addStatement(stmtLine, CALL_FLAG);

	// Populate ProcTable
	_pkb.addProcCalledInStmt(procName, stmtLine);

	// Populate CFG
	_pkb.addStmtCFG(stmtLine, CALL_FLAG);

	string caller = procName;
	string callee = next_token;

	// Populate CallTable
	_pkb.addCalls(caller, callee);
}

void Parser::parseAssignStmt()
{
	// Populate StatementTable
	_pkb.addStatement(stmtLine, ASSIGN_FLAG);

	// Populate CFG
	_pkb.addStmtCFG(stmtLine, ASSIGN_FLAG);

	// Populate ParentTable and FollowsTable for this assign stmt
	_pkb.addParent(parentStack.top(), stmtLine);
	_pkb.addFollows(stmtLine, followsStack.top());

	// Populate mod table with LHS variable
	// This involves adding current stmtLine as well as any parent/parent* stmtLines
	string LHS = next_token;
	_pkb.addModifies(LHS, stmtLine);
	_pkb.addModifies(stmtLine, LHS);
	addAllParentsOfModifiedVariable(LHS);

	// Populate ProcTable
	_pkb.addProcMod(procName, LHS);

	// Move to RHS and populate uses(var)/const table accordingly
	// Involves adding current stmtLine as well as any parent/parent* stmtLines
	match(LHS);
	match(EQUAL_FLAG);
	currRHS = "";
	parseAssignRHS();
	// Check brackets are correct
	if (!bracketStack.empty())
	{
		throw invalid_argument("SIMPLE syntax error, check parenthesis");
	}
	string postFixRHS = _pkb.makeExpr(currRHS);
	_pkb.addExpr(stmtLine, postFixRHS);
}

void Parser::parseAssignRHS()
{
	string RHS = next_token;
	currRHS += RHS;
	currRHS += " ";
	if (isConstant(RHS))
	{
		int RHSConstant = stoi(RHS);
		_pkb.addConstant(RHSConstant, stmtLine);
		//addAllParentsOfUsedConstant(RHSConstant);
	}
	else
	{
		_pkb.addUses(RHS, stmtLine);
		_pkb.addUses(stmtLine, RHS);
		addAllParentsOfUsedVariable(RHS);

		_pkb.addProcUses(procName, RHS);
	}
	match(RHS);
	if (next_token == PLUS_FLAG || next_token == MINUS_FLAG || next_token == TIMES_FLAG)
	{
		currRHS += next_token;
		currRHS += " ";
		match(next_token);
		parseAssignRHS();
	}
	else if (next_token == LEFT_PARENTHESIS)
	{
		bracketStack.push(next_token);
		match(next_token);
		parseAssignRHS();
	}
	else if (next_token == RIGHT_PARENTHESIS)
	{
		if (!bracketStack.empty())
		{
			bracketStack.pop();
			match(next_token);
			parseAssignRHS();
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

bool Parser::isConstant(string s)
{
	if (isdigit(s.at(0))) {
		return true;
	}
	return false;
}

void Parser::addAllParentsOfUsedVariable(string v)
{
	stack<int> tempStack;
	int temp;
	while (parentStack.top() != NO_PARENT_FLAG)
	{
		temp = parentStack.top();
		_pkb.addUses(v, temp);
		_pkb.addUses(temp, v);
		parentStack.pop();
		tempStack.push(temp);
	}
	while (!tempStack.empty())
	{
		temp = tempStack.top();
		parentStack.push(temp);
		tempStack.pop();
	}
}

void Parser::addAllParentsOfModifiedVariable(string v)
{
	stack<int> tempStack;
	int temp;
	while (parentStack.top() != NO_PARENT_FLAG)
	{
		temp = parentStack.top();
		_pkb.addModifies(v, temp);
		_pkb.addModifies(temp, v);
		parentStack.pop();
		tempStack.push(temp);
	}
	while (!tempStack.empty())
	{
		temp = tempStack.top();
		parentStack.push(temp);
		tempStack.pop();
	}
}

void Parser::addAllParentsOfUsedConstant(int c)
{
	stack<int> tempStack;
	int temp;
	while (parentStack.top() != NO_PARENT_FLAG)
	{
		temp = parentStack.top();
		_pkb.addConstant(c, temp);
		parentStack.pop();
		tempStack.push(temp);
	}
	while (!tempStack.empty())
	{
		temp = tempStack.top();
		parentStack.push(temp);
		tempStack.pop();
	}
}
