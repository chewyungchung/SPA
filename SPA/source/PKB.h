/*
PKB contains tables and other structures (AST, CFG) to store concrete design abstractions
PKB itself is a facade class
*/
#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>

#include "ModUsesTablebyStmt.h"
#include "ModUsesTablebyVariable.h"
#include "StatementTable.h"
#include "FollowsTable.h"
#include "ParentTable.h"
#include "ConstantTable.h"
#include "CallsTable.h"
#include "ProcTable.h"
#include "CFG.h"

using namespace std;
typedef short PROC;

//class TNode;

//class VarTable;  // no need to #include "VarTable.h" as all I need is pointer
class ModUsesTablebyStmt;
class ModUsesTablebyVariable;
class StatementTable;
class FollowsTable;
class ParentTable;
class ConstantTable;
class CallsTable;
class ProcTable;

class PKB 
{
public:

	ModUsesTablebyStmt M_U_TableByStmt;
	ModUsesTablebyVariable M_U_TableByVar;
	StatementTable S_Table;
	FollowsTable F_Table;
	ParentTable P_Table;
	ConstantTable C_Table;
	CallsTable Calls_Table;
	ProcTable Proc_Table;
	CFG Cfg;
	PKB();
	~PKB();

	void addParent(int lineOfParent, int lineNum);
	int getParentOf(int stmt);
	list<int> getParentStar(int stmt);
	list<int> getChildrenOf(int stmt);
	bool isParentEmpty();
	bool isParentOf(int parentStmt, int childStmt);
	bool isParentStar(int parent, int child);
	
	void addFollows(int lineNum, int nesting);
	int getFollowedFrom(int stmt);
	int getFollower(int stmt);
	list<int> getFollowedFromStar(int stmt);
	list<int> getFollowerStar(int stmt);
	bool isFollowEmpty();
	bool isValidFollows(int followedFrom, int follower);
	bool isFollowsStar(int stmt1, int stmt2);
	
	void addModifies(int stmtNum, string var);
	void addUses(int stmtNum, string var);
	bool isModified(int stmtNum, string varName);
	bool isUsed(int stmtNum, string varName);
	list<string> getModifiedBy(int stmtNum);
	list<string> getUsedBy(int stmtNum);

	void addModifies(string var, int stmtNum);
	void addUses(string var, int stmtNum);
	bool isValidVar(string varName);
	list<int> getModifiedBy(string varName);
	list<int> getUsedBy(string varName);
	list<string> getAllModVar();
	list<string> getAllUsedVar();
	list<string> getVarList();

	void addStatement(int stmtNum, string stmtType);
	list<int> getAssignList();
	list<int> getWhileList();
	list<int> getIfList();
	list<int> getStmtList();
	int getStatementCount();
	bool isValidStmt(int stmtNum);

	void addConstant(int constant, int stmt);
	list<int> getConstantList();
	list<int> getStmtlineByConstant(int c);

	void addCalls(string caller, string callee);
	list<string> getCallees(string caller);
	list<string> getCallers(string callee);
	bool isCallsEmpty();
	bool isCalls(string caller, string callee);
	bool isCallsStar(string caller, string callee);
	list<string> getCalleesStar(string caller);
	list<string> getCallersStar(string callee);

	void addProc(string procName);
	void addProcMod(string procName, string var);
	void addProcUses(string procName, string var);
	void addProcCalledInStmt(string procName, int stmtLine);
	string getProcName(int procIndex);
	int getProcIndex(string procName);
	bool isModifiedByProc(string procName, string varName);
	bool isUsedByProc(string procName, string varName);
	list<string> getModifiedByProc(string procName);
	list<string> getUsedByProc(string procName);
	list<string> getProcedureModifying(string varName);
	list<string> getProcedureUsing(string varName);
	list<string> getCalledProcNamesList();
	list<string> getProcedureList();

	list<int> getIfListWithControlVariable(string control_var);
	list<int> getWhileListWithControlVariable(string control_var);

	void addProcCFG();
	void addStmtCFG(int stmtnum, string stmtType);
	void closeIfCFG();
	void closeElseCFG();
	void closeWhileCFG();
	void buildCFGMatrix();
private:

};