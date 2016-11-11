#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "PKB.h"

PKB::PKB()
{
}

PKB::~PKB()
{
}

void PKB::AddStmtLst(int stmt_list_first)
{
	all_stmtlist_.push_back(stmt_list_first);
}

void PKB::AddProcedureFirstStmt(int first_stmt_of_procedure)
{
	procedure_first_stmts_list_.push_back(first_stmt_of_procedure);
}

void PKB::AddStmtProcBegin(int stmt_num, int proc_begin)
{
	stmt_to_proc_begin_table_[stmt_num] = proc_begin;
}

/***************** ParentTable ******************/
void PKB::AddParent(int parent_line, int line_num)
{
	parent_table_.AddParent(parent_line, line_num);
}

void PKB::AddParentStar(int parent_star_line, int line_num)
{
	parent_table_.AddParentStar(parent_star_line, line_num);
}

int PKB::GetParentOf(int stmt)
{
	return parent_table_.GetParentOf(stmt);
}

list<int> PKB::GetParentStar(int stmt)
{
	return parent_table_.GetParentStar(stmt);
}

list<int> PKB::GetChildrenOf(int stmt)
{
	return parent_table_.GetChildrenOf(stmt);
}

bool PKB::IsParentEmpty()
{
	return parent_table_.IsParentEmpty();
}

bool PKB::IsParentOf(int parent_stmt, int child_stmt)
{
	return parent_table_.IsParentOf(parent_stmt, child_stmt);
}

bool PKB::IsParentStar(int parent, int child)
{
	return parent_table_.IsParentStar(parent, child);
}

list<int> PKB::GetChildStarOf(int stmt)
{
	return parent_table_.GetChildStarOf(stmt);
}

/***************** FollowsTable ******************/
void PKB::AddFollows(int line_num, int nest_index)
{
	follows_table_.AddFollows(line_num, nest_index);
}

int PKB::GetFollowedFrom(int stmt)
{
	return follows_table_.GetFollowedFrom(stmt);
}

int PKB::GetFollower(int stmt)
{
	return follows_table_.GetFollower(stmt);
}

list<int> PKB::GetFollowedFromStar(int stmt)
{
	return follows_table_.GetFollowedFromStar(stmt);
}

list<int> PKB::GetFollowerStar(int stmt)
{
	return follows_table_.GetFollowerStar(stmt);
}

bool PKB::IsFollowEmpty()
{
	return follows_table_.IsFollowEmpty();
}

bool PKB::IsValidFollows(int followed_from, int follower)
{
	return follows_table_.IsValidFollows(followed_from, follower);
}

bool PKB::IsFollowsStar(int stmt1, int stmt2)
{
	return follows_table_.IsFollowsStar(stmt1, stmt2);
}

/***************** ModUsesTablebyStatement ******************/
void PKB::AddModifies(int stmt_num, string var)
{
	mod_uses_table_by_stmt_.AddModifies(stmt_num, var);
}

void PKB::AddUses(int stmt_num, string var)
{
	mod_uses_table_by_stmt_.AddUses(stmt_num, var);
}

bool PKB::IsModified(int stmt_num, string var_name)
{
	return mod_uses_table_by_stmt_.IsModified(stmt_num, var_name);
}

bool PKB::IsUsed(int stmt_num, string var_name)
{
	return mod_uses_table_by_stmt_.IsUsed(stmt_num, var_name);
}

list<string> PKB::GetModifiedBy(int stmt_num)
{
	return mod_uses_table_by_stmt_.GetModifiedBy(stmt_num);
}

list<string> PKB::GetUsedBy(int stmt_num)
{
	return mod_uses_table_by_stmt_.GetUsedBy(stmt_num);
}

/***************** ModUsesTablebyVariable ******************/
void PKB::AddModifies(string var, int stmt_num)
{
	mod_uses_table_by_var_.AddModifies(var, stmt_num);
}

void PKB::AddUses(string var, int stmt_num)
{
	mod_uses_table_by_var_.AddUses(var, stmt_num);
}

bool PKB::IsValidVar(string var_name)
{
	return mod_uses_table_by_var_.IsValidVar(var_name);
}

list<int> PKB::GetModifiedBy(string var_name)
{
	return mod_uses_table_by_var_.GetModifiedBy(var_name);
}

list<int> PKB::GetUsedBy(string var_name)
{
	return mod_uses_table_by_var_.GetUsedBy(var_name);
}

list<string> PKB::GetAllModVar()
{
	return mod_uses_table_by_var_.GetAllModVar();
}

list<string> PKB::GetAllUsedVar()
{
	return mod_uses_table_by_var_.GetAllUsedVar();
}

list<string> PKB::GetVarList()
{
	return mod_uses_table_by_var_.GetVarList();
}

/***************** StatementTable ******************/
void PKB::AddStatement(int stmt_num, string stmt_type)
{
	stmt_table_.AddStatement(stmt_num, stmt_type);
}

void PKB::AddStatement(int stmt_num, string stmt_type, string ctrl_var)
{
	stmt_table_.AddStatement(stmt_num, stmt_type, ctrl_var);
}

list<int> PKB::GetAssignList()
{
	return stmt_table_.GetAssignList();
}

list<int> PKB::GetWhileList()
{
	return stmt_table_.GetWhileList();
}

list<int> PKB::GetIfList()
{
	return stmt_table_.GetIfList();
}

list<int> PKB::GetCallList()
{
	return stmt_table_.GetCallList();
}

string PKB::GetProcNameByCallStmt(int call_stmt)
{
	return stmt_table_.GetProcNameByCallStmt(call_stmt);
}

list<int> PKB::GetIfListWithControlVariable(string control_var)
{
	return stmt_table_.GetIfListWithControlVariable(control_var);
}

list<int> PKB::GetWhileListWithControlVariable(string control_var)
{
	return stmt_table_.GetWhileListWithControlVariable(control_var);
}

string PKB::GetControlVarWithStmt(int stmt_num)
{
	return stmt_table_.GetControlVarWithStmt(stmt_num);
}

bool PKB::IsAssign(int stmt_num)
{
	return stmt_table_.IsAssign(stmt_num);
}

list<int> PKB::GetStmtList()
{
	return stmt_table_.GetStmtList();
}

int PKB::GetStatementCount()
{
	return stmt_table_.GetStatementCount();
}

bool PKB::IsValidStmt(int stmt_num)
{
	return stmt_table_.IsValidStmt(stmt_num);
}

/***************** ConstantTable ******************/
void PKB::AddConstant(int constant, int stmt)
{
	constant_table_.AddConstant(constant, stmt);
}

list<int> PKB::GetConstantList()
{
	return constant_table_.GetConstantList();;
}

list<int> PKB::GetStmtlineByConstant(int c)
{
	return constant_table_.GetStmtlineByConstant(c);
}

/***************** CallsTable ******************/
void PKB::AddCalls(string caller, string callee)
{
	calls_table_.AddCalls(caller, callee);
}

list<string> PKB::GetCallee(string caller)
{
	return calls_table_.GetCallee(caller);
}

list<string> PKB::GetCaller(string callee)
{
	return calls_table_.GetCaller(callee);
}

bool PKB::IsCallExist()
{
	return calls_table_.IsCallExist();
}

bool PKB::IsCall(string caller, string callee)
{
	return calls_table_.IsCall(caller, callee);
}

bool PKB::IsCallStar(string caller, string callee)
{
	return calls_table_.IsCallStar(caller, callee);
}

list<string> PKB::GetCalleeStar(string caller)
{
	return calls_table_.GetCalleeStar(caller);
}

list<string> PKB::GetCallerStar(string callee)
{
	return calls_table_.GetCallerStar(callee);
}

/***************** CallsGraph ******************/

void PKB::InitializeCallsGraph(int vertex_count)
{
	calls_graph_.InitializeCallsGraph(vertex_count);
}

void PKB::AddCallsGraphEdge(int i, int j)
{
	calls_graph_.AddCallsGraphEdge(i, j);
}

bool PKB::HasArcCallsGraph(int i, int j)
{
	return calls_graph_.HasArcCallsGraph(i, j);
}

bool PKB::IsCallsGraphCyclic()
{
	return calls_graph_.IsCallsGraphCyclic();
}

/***************** ProcTable ******************/
void PKB::AddProc(string proc_name)
{
	proc_table_.AddProc(proc_name);
}

void PKB::AddProcMod(string proc_name, string var)
{
	proc_table_.AddProcMod(proc_name, var);
}

void PKB::AddProcUses(string proc_name, string var)
{
	proc_table_.AddProcUses(proc_name, var);
}

void PKB::AddProcCalledInStmt(string proc_name, int stmt_line)
{
	proc_table_.AddProcCalledInStmt(proc_name, stmt_line);
}

string PKB::GetProcName(int proc_index)
{
	return proc_table_.GetProcName(proc_index);
}

int PKB::GetProcIndex(string proc_name)
{
	return proc_table_.GetProcIndex(proc_name);
}

bool PKB::IsModifiedByProc(string proc_name, string var_name)
{
	return proc_table_.IsModifiedByProc(proc_name, var_name);
}

bool PKB::IsUsedByProc(string proc_name, string var_name)
{
	return proc_table_.IsUsedByProc(proc_name, var_name);
}

list<string> PKB::GetModifiedByProc(string proc_name)
{
	return proc_table_.GetModifiedByProc(proc_name);
}

list<string> PKB::GetUsedByProc(string proc_name)
{
	return proc_table_.GetUsedByProc(proc_name);
}

list<string> PKB::GetProcedureModifying(string var_name)
{
	return proc_table_.GetProcedureModifying(var_name);
}

list<string> PKB::GetProcedureUsing(string var_name)
{
	return proc_table_.GetProcedureUsing(var_name);
}

bool PKB::IsProcedureExist(string proc_name)
{
	return proc_table_.IsProcedureExist(proc_name);
}

list<int> PKB::GetCallByProcName(string proc_name)
{
	return proc_table_.GetCallByProcName(proc_name);
}

list<string> PKB::GetCalledProcNamesList()
{
	return proc_table_.GetCalledProcNamesList();
}

list<string> PKB::GetProcedureList()
{
	return proc_table_.GetProcedureList();
}

/***************** AST ******************/
string PKB::MakeExpr(string input)
{
	return ast_.MakeExpr(input);
}

void PKB::AddExpr(int stmt, string expr)
{
	ast_.AddExpr(stmt, expr);
}

bool PKB::IsExprExist(string expr)
{
	return ast_.IsExprExist(expr);
}

bool PKB::IsSubExprExist(string sub_expr)
{
	return ast_.IsSubExprExist(sub_expr);
}

list<int> PKB::GetAssignWithExpression(string expr)
{
	return ast_.GetAssignWithExpression(expr);
}

list<int> PKB::GetAssignWithSubExpression(string sub_expr)
{
	return ast_.GetAssignWithSubExpression(sub_expr);
}

/***************** CFG ******************/
void PKB::AddProcCFG()
{
	cfg_.AddProcCFG();
}

void PKB::AddStmtCFG(int stmt_num, string stmt_type)
{
	cfg_.AddStmtCFG(stmt_num, stmt_type);
}
void PKB::CloseIfCFG()
{
	cfg_.CloseIfCFG();
}

void PKB::CloseElseCFG()
{
	cfg_.CloseElseCFG();
}

void PKB::CloseWhileCFG()
{
	cfg_.CloseWhileCFG();
}

void PKB::BuildCFGMatrix()
{
	cfg_.BuildCFGMatrix();
}

bool PKB::IsNext(int n1, int n2)
{
	return cfg_.IsNext(n1, n2);
}

bool PKB::IsNextStar(int n1, int n2)
{
	return cfg_.IsNextStar(n1, n2);
}

bool PKB::IsNextEmpty()
{
	return cfg_.IsNextEmpty();
}

list<int> PKB::GetExecutedBefore(int n)
{
	return cfg_.GetExecutedBefore(n);
}

list<int> PKB::GetExecutedAfter(int n)
{
	return cfg_.GetExecutedAfter(n);
}

list<int> PKB::GetExecutedBeforeStar(int n)
{
	return cfg_.GetExecutedBeforeStar(n);
}

list<int> PKB::GetExecutedAfterStar(int n)
{
	return cfg_.GetExecutedAfterStar(n);
}

string PKB::GetStmtType(int stmt_num)
{
	return stmt_table_.GetStmtType(stmt_num);
}

int PKB::GetProcBeginStmt(int stmt_num)
{
	if (stmt_to_proc_begin_table_[stmt_num] != 0) {
		return stmt_to_proc_begin_table_[stmt_num];
	}
	else {
		return -1;
	}
}

vector<int> PKB::GetProcedureFirstStmtList()
{
	return procedure_first_stmts_list_;
}

list<int> PKB::GetAllStmtLst()
{
	return all_stmtlist_;
}

bool PKB::IsAffects(int assign_stmt1, int assign_stmt2)
{
	set<pair<int, int>> affected_list;
	map<string, set<int>> modifies_map;
	Node proc_begin_of_assign_stmt = cfg_.GetNodeByStmt(GetProcBeginStmt(assign_stmt1));
	GetAffectsBothSyn(affected_list, modifies_map, proc_begin_of_assign_stmt);
	for (auto &affected_pair : affected_list) {
		if (affected_pair.first == assign_stmt1 && affected_pair.second == assign_stmt2) {
			return true;
		}
	}

	return false;
}

bool PKB::IsAffectsStar(int assign_stmt1, int assign_stmt2)
{
	set<pair<int, int>> affected_star_list;
	map<string, set<int>> modifies_map;
	Node proc_begin_of_assign_stmt = cfg_.GetNodeByStmt(GetProcBeginStmt(assign_stmt1));
	GetAffectsStarBothSyn(affected_star_list, modifies_map, proc_begin_of_assign_stmt);
	for (auto &affected_star_pair : affected_star_list) {
		if (affected_star_pair.first == assign_stmt1 && affected_star_pair.second == assign_stmt2) {
			return true;
		}
	}
}

list<int> PKB::GetAffectedStar(int affector_stmt_num)
{
	list<int> results;
	set<pair<int, int>> affected_star_list;
	map<string, set<int>> modifies_map;
	Node proc_begin_of_assign_stmt = cfg_.GetNodeByStmt(GetProcBeginStmt(affector_stmt_num));
	GetAffectsStarBothSyn(affected_star_list, modifies_map, proc_begin_of_assign_stmt);
	for (auto &affected_star_pair : affected_star_list) {
		if (affected_star_pair.first == affector_stmt_num) {
			results.push_back(affected_star_pair.second);
		}
	}

	return results;
}

list<int> PKB::GetAffector(int assign_stmt)
{
	list<int> results;
	set<pair<int, int>> affected_list;
	map<string, set<int>> modifies_map;
	Node proc_begin_of_assign_stmt = cfg_.GetNodeByStmt(GetProcBeginStmt(assign_stmt));
	GetAffectsBothSyn(affected_list, modifies_map, proc_begin_of_assign_stmt);
	for (auto &affected_pair : affected_list) {
		if (affected_pair.second == assign_stmt) {
			results.push_back(affected_pair.first);
		}
	}

	return results;
}

list<int> PKB::GetAffectorStar(int assign_stmt) {
	list<int> results;
	set<pair<int, int>> affected_star_list;
	map<string, set<int>> modifies_map;
	Node proc_begin_of_assign_stmt = cfg_.GetNodeByStmt(GetProcBeginStmt(assign_stmt));
	GetAffectsStarBothSyn(affected_star_list, modifies_map, proc_begin_of_assign_stmt);
	for (auto &affected_star_pair : affected_star_list) {
		if (affected_star_pair.second == assign_stmt) {
			results.push_back(affected_star_pair.first);
		}
	}

	return results;
}

list<pair<int, int>> PKB::GetAffectsBothSyn(bool is_fill_cache)
{
	list<pair<int, int>> results;
	set<pair<int, int>> result_list;
	map<string, set<int>> modifies_map;
	vector<int> start_stmt_list = GetProcedureFirstStmtList();

	if (is_affects_cache_filled_) {
		for (auto &result_pair : affects_cache_) {
			results.push_back(result_pair);
		}
		return results;
	}
	else {
		for (vector<int>::iterator it = start_stmt_list.begin(); it != start_stmt_list.end(); ++it)
		{
			set<pair<int, int>> affects_list;
			Node curr_node = cfg_.GetNodeByStmt(*it);
			GetAffectsBothSyn(affects_list, modifies_map, curr_node);
			for (auto &result_pair : affects_list) {
				results.push_back(result_pair);
				if (is_fill_cache && !is_affects_cache_filled_) {
					affects_cache_.insert(result_pair);
				}
			}
		}
	}

	if (is_fill_cache) {
		is_affects_cache_filled_ = true;
	}

	return results;
}

map<string, set<int>> PKB::GetAffectsBothSyn(set<pair<int, int>>& result_list, map<string, set<int>> modifies_map, Node curr_node)
{
	int curr_stmt_num = curr_node.GetStmtnum();
	string curr_stmt_type = GetStmtType(curr_stmt_num);
	if (curr_stmt_type == "assign")
	{
		// Get used vars of assign and check against mod_map
		string curr_modified_var = GetModifiedBy(curr_stmt_num).front();
		list<string> curr_stmt_used_vars = GetUsedBy(curr_stmt_num);
		for (auto &used_var : curr_stmt_used_vars) {
			if (modifies_map.find(used_var) != modifies_map.end()) {
				for (auto &used_var_modified_stmt_num : modifies_map[used_var]) {
					result_list.insert(pair<int, int>(used_var_modified_stmt_num, curr_stmt_num));
				}
			}
		}
		modifies_map[curr_modified_var] = set<int>();
		modifies_map[curr_modified_var].insert(curr_stmt_num);

		// If got next node, recurse. Else, return map
		if (curr_node.GetNextList().empty()) {
			return modifies_map;
		}
		else {
			Node next_node = *(curr_node.GetNextList().front());
			int next_node_stmt_num = next_node.GetStmtnum();
			if (!while_stack_.empty() && while_stack_.top() == next_node_stmt_num) {
				while_stack_.pop();
				return modifies_map;
			}
			return GetAffectsBothSyn(result_list, modifies_map, next_node);
		}
	}
	else if (curr_stmt_type == "call")
	{
		// Update modifiesMap (delete modifiedVar that are modified by this call statement)
		list<string> modified_by_call = GetModifiedBy(curr_stmt_num);
		for (auto &modified_variable : modified_by_call) {
			if (modifies_map.find(modified_variable) != modifies_map.end()) {
				modifies_map.erase(modified_variable);
			}
		}

		// Go next
		if (curr_node.GetNextList().empty())
		{
			return modifies_map;
		}
		else
		{
			Node next_node = *(curr_node.GetNextList().front());
			int next_node_stmt_num = next_node.GetStmtnum();
			if (!while_stack_.empty() && while_stack_.top() == next_node_stmt_num) {
				while_stack_.pop();
				return modifies_map;
			}
			return GetAffectsBothSyn(result_list, modifies_map, next_node);
		}
	}
	else if (curr_stmt_type == "if")
	{
		Node if_then_node = *(curr_node.GetNextList().at(0));
		Node else_node = *(curr_node.GetNextList().at(1));
		map<string, set<int>> merged_map;

		int if_then_node_stmt_num = if_then_node.GetStmtnum();
		int else_node_stmt_num = else_node.GetStmtnum();

		// Process the if and else chains separately, merge them later
		map<string, set<int>> ifMap = GetAffectsBothSyn(result_list, modifies_map, if_then_node);
		map<string, set<int>> elseMap = GetAffectsBothSyn(result_list, modifies_map, else_node);
		UnionMap(merged_map, ifMap, elseMap);
		// Go next
		Node terminalNode = GetTerminalNodeByStmt(curr_stmt_num);
		if (terminalNode.GetNextList().empty()) {
			return merged_map;
		}
		else {
			Node next_node = *(terminalNode.GetNextList().front());
			int next_node_stmt_num = next_node.GetStmtnum();
			if (!while_stack_.empty() && while_stack_.top() == next_node_stmt_num) {
				while_stack_.pop();
				return merged_map;
			}
			return GetAffectsBothSyn(result_list, merged_map, next_node);
		}
	}
	else if (curr_stmt_type == "while")
	{
		bool has_out = curr_node.GetNextList().size() == 2;
		Node into_while_node = *(curr_node.GetNextList().at(0));
		Node out_of_while_node; 

		if (has_out) {
			out_of_while_node = *(curr_node.GetNextList().at(1));
		}

		map<string, set<int>> merged_set;
		working_set_stack_.push(modifies_map);
		while_stack_.push(curr_stmt_num);
		map<string, set<int>> into_while_set = GetAffectsBothSyn(result_list, modifies_map, into_while_node);
		map<string, set<int>> popped_set = working_set_stack_.top();
		working_set_stack_.pop();
		UnionMap(merged_set, popped_set, into_while_set);
		while (IsMapEqual(popped_set, merged_set) == false) {
			while_stack_.push(curr_stmt_num);
			working_set_stack_.push(merged_set);
			modifies_map = GetAffectsBothSyn(result_list, merged_set, into_while_node);
			popped_set = working_set_stack_.top();
			working_set_stack_.pop();
			UnionMap(merged_set, modifies_map, popped_set);
		}

		if (has_out) {
			if (!while_stack_.empty() && while_stack_.top() == out_of_while_node.GetStmtnum()) {
				while_stack_.pop();
				return merged_set;
			}
			else {
				return GetAffectsBothSyn(result_list, merged_set, out_of_while_node);
			}
		}
		else {
			return merged_set;
		}
	}
	else {
		// Should only be terminal node
		return modifies_map;
	}
}

list<pair<int, int>> PKB::GetAffectsStarBothSyn(bool is_fill_cache)
{
	list<pair<int,int>> results;
	map<string, set<int>> modifies_map;
	vector<int> procedure_first_stmts_list_ = GetProcedureFirstStmtList();

	if (is_affects_star_cache_filled_) {
		for (auto &result_pair : affects_star_cache_) {
			results.push_back(result_pair);
		}
		return results;
	}
	else {
		for (auto &proc_first_stmt : procedure_first_stmts_list_) {
			set<pair<int, int>> affected_star_list;
			Node curr_node = cfg_.GetNodeByStmt(proc_first_stmt);
			GetAffectsStarBothSyn(affected_star_list, modifies_map, curr_node);
			for (auto &result_pair : affected_star_list) {
				results.push_back(result_pair);
				if (is_fill_cache && !is_affects_star_cache_filled_) {
					affects_star_cache_.insert(result_pair);
				}
			}
		}
	}

	if (is_fill_cache) {
		is_affects_star_cache_filled_ = true;
	}

	return results;
}

map<string, set<int>> PKB::GetAffectsStarBothSyn(set<pair<int, int>>& result_list, map<string, set<int>> modifies_map, Node curr_node)
{
	int curr_stmt_num = curr_node.GetStmtnum();
	string curr_stmt_type = GetStmtType(curr_stmt_num);
	if (curr_stmt_type == "assign") {
		// Get used vars of assign and check against mod_map
		string curr_modified_var = GetModifiedBy(curr_stmt_num).front();
		set<int> new_set_for_curr_modified_var;
		list<string> curr_stmt_used_vars = GetUsedBy(curr_stmt_num);
		bool is_modified_being_used = find(curr_stmt_used_vars.begin(), curr_stmt_used_vars.end(), curr_modified_var) != curr_stmt_used_vars.end();

		for (auto &used_var : curr_stmt_used_vars) {
			if (modifies_map.find(used_var) != modifies_map.end()) {
				for (auto &used_var_modified_stmt_num : modifies_map[used_var]) {
					result_list.insert(pair<int, int>(used_var_modified_stmt_num, curr_stmt_num));
					new_set_for_curr_modified_var.insert(used_var_modified_stmt_num);
				}
			}
		}
		if (!is_modified_being_used) {
			modifies_map[curr_modified_var] = new_set_for_curr_modified_var;
		}
		else {
			for (auto &new_thing : new_set_for_curr_modified_var) {
				modifies_map[curr_modified_var].insert(new_thing);
			}
		}
		modifies_map[curr_modified_var].insert(curr_stmt_num);

		// If got next node, recurse. Else, return map
		if (curr_node.GetNextList().empty()) {
			return modifies_map;
		}
		else {
			Node next_node = *(curr_node.GetNextList().front());
			int next_node_stmt_num = next_node.GetStmtnum();
			if (!while_stack_.empty() && while_stack_.top() == next_node_stmt_num) {
				while_stack_.pop();
				return modifies_map;
			}
			return GetAffectsStarBothSyn(result_list, modifies_map, next_node);
		}
	}
	else if (curr_stmt_type == "if") {
		Node if_then_node = *(curr_node.GetNextList().at(0));
		Node else_node = *(curr_node.GetNextList().at(1));
		map<string, set<int>> merged_set;
		map<string, set<int>> if_then_map = GetAffectsStarBothSyn(result_list, modifies_map, if_then_node);
		map<string, set<int>> else_map = GetAffectsStarBothSyn(result_list, modifies_map, else_node);
		UnionMap(merged_set, if_then_map, else_map);
		Node terminal_node = GetTerminalNodeByStmt(curr_stmt_num);
		if (terminal_node.GetNextList().empty()) {
			return merged_set;
		}
		else {
			Node next_node = *(terminal_node.GetNextList().front());
			int next_node_stmt_num = next_node.GetStmtnum();
			if (!while_stack_.empty() && while_stack_.top() == next_node_stmt_num) {
				while_stack_.pop();
				return merged_set;
			}
			return GetAffectsStarBothSyn(result_list, merged_set, next_node);
		}
	}
	else if (curr_stmt_type == "while") {
		bool has_out = curr_node.GetNextList().size() == 2;
		Node into_while_node = *(curr_node.GetNextList().at(0));
		Node out_of_while_node;

		if (has_out) {
			out_of_while_node = *(curr_node.GetNextList().at(1));
		}

		map<string, set<int>> merged_set;
		working_set_stack_.push(modifies_map);
		while_stack_.push(curr_stmt_num);
		map<string, set<int>> into_while_set = GetAffectsStarBothSyn(result_list, modifies_map, into_while_node);
		map<string, set<int>> popped_set = working_set_stack_.top();
		working_set_stack_.pop();
		UnionMap(merged_set, popped_set, into_while_set);
		while (IsMapEqual(popped_set, merged_set) == false) {
			while_stack_.push(curr_stmt_num);
			working_set_stack_.push(merged_set);
			modifies_map = GetAffectsStarBothSyn(result_list, merged_set, into_while_node);
			popped_set = working_set_stack_.top();
			working_set_stack_.pop();
			UnionMap(merged_set, modifies_map, popped_set);
		}

		if (has_out) {
			if (!while_stack_.empty() && while_stack_.top() == out_of_while_node.GetStmtnum()) {
				while_stack_.pop();
				return merged_set;
			}
			else {
				return GetAffectsStarBothSyn(result_list, merged_set, out_of_while_node);
			}
		}
		else {
			return merged_set;
		}
	}
	else if (curr_stmt_type == "call") {
		list<string> curr_modified_vars = GetModifiedBy(curr_stmt_num);
		for (auto &modified_var : curr_modified_vars) {
			if (modifies_map.find(modified_var) != modifies_map.end()) {
				modifies_map.erase(modified_var);
			}
		}

		// If got next node, recurse. Else, return map
		if (curr_node.GetNextList().empty()) {
			return modifies_map;
		}
		else {
			Node next_node = *(curr_node.GetNextList().front());
			int next_node_stmt_num = next_node.GetStmtnum();
			if (!while_stack_.empty() && while_stack_.top() == next_node_stmt_num) {
				while_stack_.pop();
				return modifies_map;
			}
			return GetAffectsStarBothSyn(result_list, modifies_map, next_node);
		}
	}
	else {
		// Should only be terminal node
		return modifies_map;
	}
}

Node PKB::GetTerminalNodeByStmt(int if_stmt_num)
{
	return cfg_.GetTerminalNodeByStmt(if_stmt_num);
}

void PKB::UnionMap(map<string, set<int>>& main_map, map<string, set<int>>& if_then_map, map<string, set<int>>& else_map)
{
	for (auto &entry : if_then_map) {
		for (auto &value : entry.second) {
			main_map[entry.first].insert(value);
		}
	}
	for (auto &entry : else_map) {
		for (auto &value : entry.second) {
			main_map[entry.first].insert(value);
		}
	}
}

bool PKB::IsMapEqual(map<string, set<int>>& old_map, map<string, set<int>>& current_map)
{
	if (old_map.size() == current_map.size()) {
		for (auto &current_map_entry : current_map) {
			if (old_map.find(current_map_entry.first) != old_map.end()) {
				set<int>* old_map_set = &old_map[current_map_entry.first];
				if (old_map_set->size() == current_map_entry.second.size()) {
					for (auto &current_entry_set_element : current_map_entry.second) {
						if (old_map_set->find(current_entry_set_element) == old_map_set->end()) {
							return false;
						}
					}
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}
		// Everything is equal at this point
		return true;
	}
	else {
		return false;
	}
}

bool PKB::IsInList(list<string> list, string target)
{
	if (find(list.begin(), list.end(), target) != list.end()) {
		return true;
	}
	return false;
}
