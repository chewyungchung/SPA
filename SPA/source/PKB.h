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
#include <set>

#include "ModUsesTablebyStmt.h"
#include "ModUsesTablebyVariable.h"
#include "StatementTable.h"
#include "FollowsTable.h"
#include "ParentTable.h"
#include "ConstantTable.h"
#include "CallsTable.h"
#include "CallsGraph.h"
#include "ProcTable.h"
#include "AST.h"
#include "CFG.h"

using namespace std;
typedef short PROC;

class ModUsesTablebyStmt;
class ModUsesTablebyVariable;
class StatementTable;
class FollowsTable;
class ParentTable;
class ConstantTable;
class CallsTable;
class CallsGraph;
class ProcTable;
class AST;
class CFG;

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
	CallsGraph Calls_Graph;
	ProcTable Proc_Table;
	AST Ast;
	CFG Cfg;
	stack<int> while_stack;
	map<int, int> while_stmt_to_visited_counter;
	unordered_map<int, int> stmt_to_proc_begin_table;
	vector<int> procedure_first_stmts_list;
	list<int> all_stmtlst;
	set<pair<int, int>> affects_cache;
	set<pair<int, int>> affects_star_cache;
	bool is_affects_cache_filled = false;
	bool is_affects_star_cache_filled = false;
	PKB();
	~PKB();

	void addStmtLst(int stmt_list_first);
	void addProcedureFirstStmt(int first_stmt_of_procedure);
	void addStmtProcBegin(int stmt_num, int proc_begin);
	void addParent(int lineOfParent, int lineNum);
	void addParentStar(int parentStar, int child);
	int getParentOf(int stmt);
	list<int> getParentStar(int stmt);
	list<int> getChildrenOf(int stmt);
	bool isParentEmpty();
	bool isParentOf(int parentStmt, int childStmt);
	bool isParentStar(int parent, int child);
	list<int> getChildStarOf(int stmt);

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
	void addStatement(int stmtNum, string stmtType, string ctrlvar);
	list<int> getAssignList();
	list<int> getWhileList();
	list<int> getIfList();
	list<int> getCallList();
	list<int> getStmtList();
	int getStatementCount();
	bool isValidStmt(int stmtNum);
	string getProcNameByCallStmt(int callStmt);
	list<int> getIfListWithControlVariable(string control_var);
	list<int> getWhileListWithControlVariable(string control_var);
	string getControlVarWithStmt(int stmtNum);
	void addConstant(int constant, int stmt);
	list<int> getConstantList();
	list<int> getStmtlineByConstant(int c);

	void addCalls(string caller, string callee);
	list<string> getCallee(string caller);
	list<string> getCaller(string callee);
	bool isCallExist();
	bool isCall(string caller, string callee);
	bool isCallStar(string caller, string callee);
	list<string> getCalleeStar(string caller);
	list<string> getCallerStar(string callee);

	void initializeCallsGraph(int vertexCount);
	void addCallsGraphEdge(int i, int j);
	bool hasArcCallsGraph(int i, int j);
	bool isCallsGraphCyclic();

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
	bool isProcedureExist(string procName);
	list<int> getCallByProcName(string procName);
	list<string> getCalledProcNamesList();
	list<string> getProcedureList();

	string makeExpr(string input);
	void addExpr(int stmt, string expr);
	bool isExprExist(string expr);
	bool isSubExprExist(string subExpr);
	list<int> getAssignWithExpression(string expr);
	list<int> getAssignWithSubExpression(string subExpr);

	void addProcCFG();
	void addStmtCFG(int stmtnum, string stmtType);
	void closeIfCFG();
	void closeElseCFG();
	void closeWhileCFG();
	void buildCFGMatrix();

	bool isNext(int n1, int n2);
	bool isNextStar(int n1, int n2);
	bool isNextEmpty();
	list<int> getExecutedBefore(int n);
	list<int> getExecutedAfter(int n);
	list<int> getExecutedBeforeStar(int n);
	list<int> getExecutedAfterStar(int n);
	string getStmtType(int stmtNum);
	int getProcBeginStmt(int stmtNum);
	vector<int> getProcedureFirstStmtList();
	list<int> getAllStmtLst();

	bool isAffects(int assign_stmt1, int assign_stmt2);
	bool isAffectsStar(int assign_stmt1, int assign_stmt2);
	bool isAffectsEmpty();
	bool isAffectsStarEmpty();
	list<int> getAffected(int assign_stmt);
	list<int> getAffectedStar(int assign_stmt);
	list<int> getAffector(int assign_stmt);
	list<int> getAffectorStar(int assign_stmt);
	list<pair<int, int>> getAffectsBothSyn(bool is_fill_cache);
	map<string, set<int>> PKB::getAffectsBothSyn(map<int,map<string,set<int>>> while_map, set<pair<int, int>> &affectsList, map<string, set<int>> modifiesMap, Node currNode, set<int> visited_while);
	list<pair<int, int>> getAffectsStarBothSyn(bool is_fill_cache);
	map<string, set<int>> getAffectsStarBothSyn(map<int,map<string,set<int>>> while_Map, set<pair<int, int>>& result_list, map<string, set<int>> modifies_map, Node curr_node, set<int> visited_while);
private:
	Node getTerminalNodeByStmt(int if_stmt_num);
	void UnionMap(map<string, set<int>>& main_map, map<string, set<int>>& if_then_map, map<string, set<int>>& else_map);
	bool IsMapEqual(map<string, set<int>>& old_map, map<string, set<int>>& current_map);
	bool IsInList(list<string> list, string target);
};