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
class ModUsesTablebyVariable;

class PKB {

private:
	ModUsesTablebyVariable* M_U_TableByVar;
	PKB();
	~PKB();
	static PKB* instance;
public:
	static VarTable* varTable; 
	static int setProcToAST(PROC p, TNode* r);
	static TNode* getRootAST (PROC p);

	static PKB* getPKB();
	static void resetPKB();
	/*
	int getParentOf(int stmt);
	vector<int> getParentStar(int stmt);
	vector<int> getChildrenOf(int stmt);
	bool isParentEmpty();
	bool isParentOf(int parentStmt, int childStmt);
	bool isParentStar(int stmt1, int stmt2);

	int getFollowedFrom(int stmt);
	int getFollower(int stmt);
	vector<int> getFollowedFromStar(int stmt);
	vector<int> getFollowerStar(int stmt);
	bool isFollowEmpty();
	bool isValidFollows(int followedFrom, int follower);
	bool isFollowsStar(int stmt1, int stmt2);
	*/
	list<int> getUsedBy(string name);
	// vector<string> getUses(int stmt);
	// vector<int, vector<string> > getAllUses();
	bool isUsed(int stmt, string name);
	list<int> getModifiedBy(string name);
	// vector<string> getModifies(int stmt);
	// vector<int, vector<string> > getAllModifies();
	bool isModified(int stmt, string name);
	/*
	vector<int> getAssignList();
	vector<int> getWhileList();

	vector<string> getVarList();

	int getStatementCount();
	bool isValidStmt(int stmt);
	vector<int> getConstantList();
	*/
};