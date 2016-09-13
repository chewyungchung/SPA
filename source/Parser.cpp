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
	tk = new Tokenizer(fileName);
	stmtLine = 1;
	parentStack.push(NO_PARENT_FLAG);
	followsMaxNestingLevel = 1;
	followsStack.push(followsMaxNestingLevel);
}

Parser::~Parser()
{
	delete tk;
}


PKB * Parser::process()
{
	parseProgram();
	return PKB::getPKB();
}


void Parser::match(string token)
{
	if (next_token == token)
	{
		next_token = tk->getNextToken();
	}
	else
	{
		throw invalid_argument("SIMPLE syntax error, check SIMPLE source");
	}
}


void Parser::parseProgram()
{
	next_token = tk->getNextToken();
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
	// Populate StatementTable
	PKB::getPKB()->addStatement(stmtLine, WHILE_FLAG);

	// Populate ParentTable for current while stmt and set current while stmt as current parent
	PKB::getPKB()->addParent(parentStack.top(), stmtLine);
	parentStack.push(stmtLine);

	// Populate FollowsTable for current while stmt and enter into new nesting level
	PKB::getPKB()->addFollows(stmtLine, followsStack.top());
	followsMaxNestingLevel++;
	followsStack.push(followsMaxNestingLevel);

	// Populate UsesTable for control variable
	// This involves adding current stmtLine as well as any parent/parent* stmtLines
	match(WHILE_FLAG);
	string controlVar = next_token;
	PKB::getPKB()->addUses(controlVar, stmtLine);
	PKB::getPKB()->addUses(stmtLine, controlVar);
	addAllParentsOfUsedVariable(controlVar);
	
	// Recurse on stmtLst
	match(controlVar);
	match(LEFT_BRACES);
	stmtLine++;
	parseStmtLst();
	match(RIGHT_BRACES);
	//stmtLine++;

	// Exiting from while loop: remove from current parent and exit nesting level
	parentStack.pop();
	followsStack.pop();
}

void Parser::parseAssignStmt()
{
	// Populate StatementTable
	PKB::getPKB()->addStatement(stmtLine, ASSIGN_FLAG);

	// Populate ParentTable and FollowsTable for this assign stmt
	PKB::getPKB()->addParent(parentStack.top(), stmtLine);
	PKB::getPKB()->addFollows(stmtLine, followsStack.top());

	// Populate mod table with LHS variable
	// This involves adding current stmtLine as well as any parent/parent* stmtLines
	string LHS = next_token;
	PKB::getPKB()->addModifies(LHS, stmtLine);
	PKB::getPKB()->addModifies(stmtLine, LHS);
	addAllParentsOfModifiedVariable(LHS);

	// Move to RHS and populate uses(var)/const table accordingly
	// Involves adding current stmtLine as well as any parent/parent* stmtLines
	match(LHS);
	match(EQUAL_FLAG);
	parseAssignRHS();
}

void Parser::parseAssignRHS()
{
	string RHS = next_token;
	if (isConstant(RHS))
	{
		int RHSConstant = stoi(RHS);
		PKB::getPKB()->addConstant(RHSConstant, stmtLine);
		addAllParentsOfUsedConstant(RHSConstant);
	}
	else
	{
		PKB::getPKB()->addUses(RHS, stmtLine);
		PKB::getPKB()->addUses(stmtLine, RHS);
		addAllParentsOfUsedVariable(RHS);
	}
	match(RHS);
	if (next_token == PLUS_FLAG)
	{
		match(next_token);
		parseAssignRHS();
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
		PKB::getPKB()->addUses(v, temp);
		PKB::getPKB()->addUses(temp, v);
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
		PKB::getPKB()->addModifies(v, temp);
		PKB::getPKB()->addModifies(temp, v);
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
		PKB::getPKB()->addConstant(c, temp);
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
