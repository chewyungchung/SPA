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
	// Populate statement table
	PKB::getPKB()->addStatement(stmtLine, WHILE_FLAG);

	// Populate parent for current while stmt and set current while stmt as current parent
	PKB::getPKB()->addParent(parentStack.top(), stmtLine);
	parentStack.push(stmtLine);

	// Populate follows for current while stmt and enter into new nesting level
	PKB::getPKB()->addFollows(stmtLine, followsStack.top());
	followsMaxNestingLevel++;
	followsStack.push(followsMaxNestingLevel);

	// Populate uses table for control variable
	match(WHILE_FLAG);
	PKB::getPKB()->addUses(next_token, stmtLine);
	PKB::getPKB()->addUses(stmtLine, next_token);

	// Recurse on stmtLine
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
	// Populate statement table
	PKB::getPKB()->addStatement(stmtLine, ASSIGN_FLAG);

	// Populate parent and follows for this assign stmt
	PKB::getPKB()->addParent(parentStack.top(), stmtLine);
	PKB::getPKB()->addFollows(stmtLine, followsStack.top());

	// Populate mod table with LHS variable
	string LHS = next_token;
	PKB::getPKB()->addModifies(LHS, stmtLine);
	PKB::getPKB()->addModifies(stmtLine, LHS);

	// Move to RHS and populate uses(var)/const table accordingly
	match(LHS);
	match(EQUAL_FLAG);
	string RHS = next_token;
	if (isConstant(RHS))
	{
		PKB::getPKB()->addConstant(RHS, stmtLine);
	}
	else
	{
		PKB::getPKB()->addUses(RHS, stmtLine);
		PKB::getPKB()->addUses(stmtLine,RHS);
	}
}

bool Parser::isConstant(string s)
{
	return isdigit(s.at(0));
}
