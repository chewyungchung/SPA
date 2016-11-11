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
	ModUsesTablebyStmt mod_uses_table_by_stmt_;
	ModUsesTablebyVariable mod_uses_table_by_var_;
	StatementTable stmt_table_;
	FollowsTable follows_table_;
	ParentTable parent_table_;
	ConstantTable constant_table_;
	CallsTable calls_table_;
	CallsGraph calls_graph_;
	ProcTable proc_table_;
	AST ast_;
	CFG cfg_;
	stack<int> while_stack_;
	stack <map<string, set<int>>> working_set_stack_;
	unordered_map<int, int> stmt_to_proc_begin_table_;
	vector<int> procedure_first_stmts_list_;
	list<int> all_stmtlist_;
	set<pair<int, int>> affects_cache_;
	set<pair<int, int>> affects_star_cache_;
	bool is_affects_cache_filled_ = false;
	bool is_affects_star_cache_filled_ = false;
	PKB();
	~PKB();

	void AddStmtLst(int stmt_list_first);
	void AddProcedureFirstStmt(int first_stmt_of_procedure);
	void AddStmtProcBegin(int stmt_num, int proc_begin);
	void AddParent(int parent_line, int line_num);
	void AddParentStar(int parent_star, int child);
	int GetParentOf(int stmt);
	list<int> GetParentStar(int stmt);
	list<int> GetChildrenOf(int stmt);
	bool IsParentEmpty();
	bool IsParentOf(int parent_stmt, int child_stmt);
	bool IsParentStar(int parent, int child);
	list<int> GetChildStarOf(int stmt);

	void AddFollows(int line_num, int nesting);
	int GetFollowedFrom(int stmt);
	int GetFollower(int stmt);
	list<int> GetFollowedFromStar(int stmt);
	list<int> GetFollowerStar(int stmt);
	bool IsFollowEmpty();
	bool IsValidFollows(int followed_from, int follower);
	bool IsFollowsStar(int stmt1, int stmt2);

	void AddModifies(int stmt_num, string var);
	void AddUses(int stmt_num, string var);
	bool IsModified(int stmt_num, string var_name);
	bool IsUsed(int stmt_num, string var_name);
	list<string> GetModifiedBy(int stmt_num);
	list<string> GetUsedBy(int stmt_num);

	void AddModifies(string var, int stmt_num);
	void AddUses(string var, int stmt_num);
	bool IsValidVar(string var_name);
	list<int> GetModifiedBy(string var_name);
	list<int> GetUsedBy(string var_name);
	list<string> GetAllModVar();
	list<string> GetAllUsedVar();
	list<string> GetVarList();

	void AddStatement(int stmt_num, string stmt_type);
	void AddStatement(int stmt_num, string stmt_type, string ctrl_var);
	list<int> GetAssignList();
	list<int> GetWhileList();
	list<int> GetIfList();
	list<int> GetCallList();
	list<int> GetStmtList();
	int GetStatementCount();
	bool IsValidStmt(int stmt_num);
	string GetProcNameByCallStmt(int call_stmt);
	list<int> GetIfListWithControlVariable(string control_var);
	list<int> GetWhileListWithControlVariable(string control_var);
	string GetControlVarWithStmt(int stmt_num);
	bool IsAssign(int stmt_num);

	void AddConstant(int constant, int stmt);
	list<int> GetConstantList();
	list<int> GetStmtlineByConstant(int c);

	void AddCalls(string caller, string callee);
	list<string> GetCallee(string caller);
	list<string> GetCaller(string callee);
	bool IsCallExist();
	bool IsCall(string caller, string callee);
	bool IsCallStar(string caller, string callee);
	list<string> GetCalleeStar(string caller);
	list<string> GetCallerStar(string callee);

	void InitializeCallsGraph(int vertex_count);
	void AddCallsGraphEdge(int i, int j);
	bool HasArcCallsGraph(int i, int j);
	bool IsCallsGraphCyclic();

	void AddProc(string proc_name);
	void AddProcMod(string proc_name, string var);
	void AddProcUses(string proc_name, string var);
	void AddProcCalledInStmt(string proc_name, int stmt_line);
	string GetProcName(int proc_index);
	int GetProcIndex(string proc_name);
	bool IsModifiedByProc(string proc_name, string var_name);
	bool IsUsedByProc(string proc_name, string var_name);
	list<string> GetModifiedByProc(string proc_name);
	list<string> GetUsedByProc(string proc_name);
	list<string> GetProcedureModifying(string var_name);
	list<string> GetProcedureUsing(string var_name);
	bool IsProcedureExist(string proc_name);
	list<int> GetCallByProcName(string proc_name);
	list<string> GetCalledProcNamesList();
	list<string> GetProcedureList();

	string MakeExpr(string input);
	void AddExpr(int stmt, string expr);
	bool IsExprExist(string expr);
	bool IsSubExprExist(string sub_expr);
	list<int> GetAssignWithExpression(string expr);
	list<int> GetAssignWithSubExpression(string sub_expr);

	void AddProcCFG();
	void AddStmtCFG(int stmt_num, string stmt_type);
	void CloseIfCFG();
	void CloseElseCFG();
	void CloseWhileCFG();
	void BuildCFGMatrix();

	bool IsNext(int n1, int n2);
	bool IsNextStar(int n1, int n2);
	bool IsNextEmpty();
	list<int> GetExecutedBefore(int n);
	list<int> GetExecutedAfter(int n);
	list<int> GetExecutedBeforeStar(int n);
	list<int> GetExecutedAfterStar(int n);
	string GetStmtType(int stmt_num);
	int GetProcBeginStmt(int stmt_num);
	vector<int> GetProcedureFirstStmtList();
	list<int> GetAllStmtLst();

	bool IsAffects(int assign_stmt1, int assign_stmt2);
	bool IsAffectsStar(int assign_stmt1, int assign_stmt2);
	list<int> GetAffected(int assign_stmt);
	list<int> GetAffectedStar(int assign_stmt);
	list<int> GetAffector(int assign_stmt);
	list<int> GetAffectorStar(int assign_stmt);
	list<pair<int, int>> GetAffectsBothSyn(bool is_fill_cache);
	map<string, set<int>> PKB::GetAffectsBothSyn(set<pair<int, int>> &result_list, map<string, set<int>> modifies_map, Node curr_node);
	list<pair<int, int>> GetAffectsStarBothSyn(bool is_fill_cache);
	map<string, set<int>> GetAffectsStarBothSyn(set<pair<int, int>>& result_list, map<string, set<int>> modifies_map, Node curr_node);
private:
	Node GetTerminalNodeByStmt(int if_stmt_num);
	void UnionMap(map<string, set<int>>& main_map, map<string, set<int>>& if_then_map, map<string, set<int>>& else_map);
	bool IsMapEqual(map<string, set<int>>& old_map, map<string, set<int>>& current_map);
	bool IsInList(list<string> list, string target);

};