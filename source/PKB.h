#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>

using namespace std;
typedef short PROC;

class TNode;

class VarTable;  // no need to #include "VarTable.h" as all I need is pointer
class ModUsesTablebyStmt;
class ModUsesTablebyVariable;
class StatementTable;
class FollowsTable;
class ParentTable;

class PKB {

private:
	ModUsesTablebyStmt* M_U_TableByStmt;
	ModUsesTablebyVariable* M_U_TableByVar;
	StatementTable* S_Table;
	FollowsTable* F_Table;
	ParentTable* P_Table;
	PKB();
	~PKB();
	static PKB* instance;
public:
	static VarTable* varTable; 
	static int setProcToAST(PROC p, TNode* r);
	static TNode* getRootAST (PROC p);

	static PKB* getPKB();
	static void resetPKB();
	
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
	bool isValidStmt(int stmtNum);
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
	list<int> getStmtList();
	int getStatementCount();

	/*
	vector<int> getConstantList();
	*/
};