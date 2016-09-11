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

Parser::Parser(string fileName)
{
	Tokenizer tk(fileName);
	stmtLine = 1;
	parentStack.push(NO_PARENT_FLAG);
	followsMaxNestingLevel = 1;
	followsStack.push(followsMaxNestingLevel);
}


void Parser::process()
{
	parseProgram();
}


void Parser::match(string token)
{
	if (next_token == token)
	{
		next_token = tk.getNextToken();
	}
	else
	{
		throw invalid_argument("SIMPLE syntax error, check SIMPLE source");
	}
}


void Parser::parseProgram()
{
	next_token = tk.getNextToken();
	parseProcedure();
}

void Parser::parseProcedure()
{
	match(PROCEDURE_FLAG);
	procName = next_token;
	match(procName);
	match(LEFT_BRACES);
	parseStmtLst();
	match(RIGHT_BRACES);
}

void Parser::parseStmtLst()
{
	if (next_token == WHILE_FLAG)
	{
		parseWhileStmt();
	}
	else
	{
		parseAssignStmt();
	}
	match(SEMICOLON_FLAG);
	stmtLine++;
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
	// Populate StatementTable
	PKB.addStatement(stmtLine, WHILE_FLAG);

	// Populate ParentTable for current while stmt and set current while stmt as current parent
	PKB.addParent(parentStack.top(), stmtLine);
	parentStack.push(stmtLine);

	// Populate FollowsTable for current while stmt and enter into new nesting level
	PKB.addFollows(stmtLine, followsStack.top());
	followsMaxNestingLevel++;
	followsStack.push(followsMaxNestingLevel);

	// Populate UsesTable for control variable
	// This involves adding current stmtLine as well as any parent/parent* stmtLines
	match(WHILE_FLAG);
	PKB.addUsesbyVar(next_token, stmtLine);
	PKB.addUsesbyStmt(stmtLine, next_token);
	addAllParentsOfUsedVariable(next_token);

	// Recurse on stmtLst
	match(LEFT_BRACES);
	stmtLine++;
	parseStmtLst();
	match(RIGHT_BRACES);
	stmtLine++;

	// Exiting from while loop: remove from current parent and exit nesting level
	parentStack.pop();
	followsStack.pop();
}

void Parser::parseAssignStmt()
{
	// Populate StatementTable
	PKB.addStatement(stmtLine, ASSIGN_FLAG);

	// Populate ParentTable and FollowsTable for this assign stmt
	PKB.addParent(parentStack.top(), stmtLine);
	PKB.addFollows(stmtLine, followsStack.top());

	// Populate mod table with LHS variable
	// This involves adding current stmtLine as well as any parent/parent* stmtLines
	string LHS = next_token;
	PKB.addModifiesbyVar(LHS, stmtLine);
	PKB.addModifiesbyStmt(stmtLine, LHS);
	addAllParentsOfModifiedVariable(next_token);

	// Move to RHS and populate uses(var)/const table accordingly
	// Involves adding current stmtLine as well as any parent/parent* stmtLines
	match(LHS);
	match(EQUAL_FLAG);
	string RHS = next_token;
	if (isConstant(RHS))
	{
		int RHSConstant = stoi(RHS);
		PKB.addConstant(RHSConstant, stmtLine);
		addAllParentsOfUsedConstant(RHSConstant);
	}
	else
	{
		PKB.addUsesbyVar(RHS, stmtLine);
		PKB.addUsesbyStmt(stmtLine,RHS);
		addAllParentsOfUsedVariable(next_token);
	}
}

bool Parser::isConstant(string s)
{
	return isdigit(s.at(0));
}

void Parser::addAllParentsOfUsedVariable(string v)
{
	stack<int> tempStack;
	int temp;
	while (parentStack.top != NO_PARENT_FLAG)
	{
		temp = parentStack.top();
		PKB.addUsesbyVar(v, temp);
		PKB.addUsesbyStmt(temp, v);
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
	while (parentStack.top != NO_PARENT_FLAG)
	{
		temp = parentStack.top();
		PKB.addModifiesbyVar(v, temp);
		PKB.addModifiesbyStmt(temp, v);
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
	while (parentStack.top != NO_PARENT_FLAG)
	{
		temp = parentStack.top();
		PKB.addConstant(c, temp);
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



