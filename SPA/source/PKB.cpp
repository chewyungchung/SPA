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

void PKB::addStmtLst(int stmt_list_first)
{
	all_stmtlst.push_back(stmt_list_first);
}

void PKB::addProcedureFirstStmt(int first_stmt_of_procedure)
{
	procedure_first_stmts_list.push_back(first_stmt_of_procedure);
}

void PKB::addStmtProcBegin(int stmt_num, int proc_begin)
{
	stmt_to_proc_begin_table[stmt_num] = proc_begin;
}

/***************** ParentTable ******************/
void PKB::addParent(int lineOfParent, int lineNum)
{
	P_Table.addParent(lineOfParent, lineNum);
}

void PKB::addParentStar(int lineOfParentStar, int lineNum)
{
	P_Table.addParentStar(lineOfParentStar, lineNum);
}

int PKB::getParentOf(int stmt)
{
	return P_Table.getParentOf(stmt);
}

list<int> PKB::getParentStar(int stmt)
{
	return P_Table.getParentStar(stmt);
}

list<int> PKB::getChildrenOf(int stmt)
{
	return P_Table.getChildrenOf(stmt);
}

bool PKB::isParentEmpty()
{
	return P_Table.isParentEmpty();
}

bool PKB::isParentOf(int parentStmt, int childStmt)
{
	return P_Table.isParentOf(parentStmt, childStmt);
}

bool PKB::isParentStar(int parent, int child)
{
	return P_Table.isParentStar(parent, child);
}

list<int> PKB::getChildStarOf(int stmt)
{
	return P_Table.getChildStarOf(stmt);
}

/***************** FollowsTable ******************/
void PKB::addFollows(int lineNum, int nestingIndex)
{
	F_Table.addFollows(lineNum, nestingIndex);
}

int PKB::getFollowedFrom(int stmt)
{
	return F_Table.getFollowedFrom(stmt);
}

int PKB::getFollower(int stmt)
{
	return F_Table.getFollower(stmt);
}

list<int> PKB::getFollowedFromStar(int stmt)
{
	return F_Table.getFollowedFromStar(stmt);
}

list<int> PKB::getFollowerStar(int stmt)
{
	return F_Table.getFollowerStar(stmt);
}

bool PKB::isFollowEmpty()
{
	return F_Table.isFollowEmpty();
}

bool PKB::isValidFollows(int followedFrom, int follower)
{
	return F_Table.isValidFollows(followedFrom, follower);
}

bool PKB::isFollowsStar(int stmt1, int stmt2)
{
	return F_Table.isFollowsStar(stmt1, stmt2);
}

/***************** ModUsesTablebyStatement ******************/
void PKB::addModifies(int stmtNum, string var)
{
	M_U_TableByStmt.addModifies(stmtNum, var);
}

void PKB::addUses(int stmtNum, string var)
{
	M_U_TableByStmt.addUses(stmtNum, var);
}

bool PKB::isModified(int stmtNum, string varName)
{
	return M_U_TableByStmt.isModified(stmtNum, varName);
}

bool PKB::isUsed(int stmtNum, string varName)
{
	return M_U_TableByStmt.isUsed(stmtNum, varName);
}

list<string> PKB::getModifiedBy(int stmtNum)
{
	return M_U_TableByStmt.getModifiedBy(stmtNum);
}

list<string> PKB::getUsedBy(int stmtNum)
{
	return M_U_TableByStmt.getUsedBy(stmtNum);
}

/***************** ModUsesTablebyVariable ******************/
void PKB::addModifies(string var, int stmtNum)
{
	M_U_TableByVar.addModifies(var, stmtNum);
}

void PKB::addUses(string var, int stmtNum)
{
	M_U_TableByVar.addUses(var, stmtNum);
}

bool PKB::isValidVar(string varName)
{
	return M_U_TableByVar.isValidVar(varName);
}

list<int> PKB::getModifiedBy(string varName)
{
	return M_U_TableByVar.getModifiedBy(varName);
}

list<int> PKB::getUsedBy(string varName)
{
	return M_U_TableByVar.getUsedBy(varName);
}

list<string> PKB::getAllModVar()
{
	return M_U_TableByVar.getAllModVar();
}

list<string> PKB::getAllUsedVar()
{
	return M_U_TableByVar.getAllUsedVar();
}

list<string> PKB::getVarList()
{
	return M_U_TableByVar.getVarList();
}

/***************** StatementTable ******************/
void PKB::addStatement(int stmtNum, string stmtType)
{
	S_Table.addStatement(stmtNum, stmtType);
}

void PKB::addStatement(int stmtNum, string stmtType, string ctrlvar)
{
	S_Table.addStatement(stmtNum, stmtType, ctrlvar);
}

list<int> PKB::getAssignList()
{
	return S_Table.getAssignList();
}

list<int> PKB::getWhileList()
{
	return S_Table.getWhileList();
}

list<int> PKB::getIfList()
{
	return S_Table.getIfList();
}

list<int> PKB::getCallList()
{
	return S_Table.getCallList();
}

string PKB::getProcNameByCallStmt(int callStmt)
{
	return S_Table.getProcNameByCallStmt(callStmt);
}

list<int> PKB::getIfListWithControlVariable(string control_var)
{
	return S_Table.getIfListWithControlVariable(control_var);
}

list<int> PKB::getWhileListWithControlVariable(string control_var)
{
	return S_Table.getWhileListWithControlVariable(control_var);
}

string PKB::getControlVarWithStmt(int stmtNum)
{
	return S_Table.getControlVarWithStmt(stmtNum);
}

list<int> PKB::getStmtList()
{
	return S_Table.getStmtList();
}

int PKB::getStatementCount()
{
	return S_Table.getStatementCount();
}

bool PKB::isValidStmt(int stmtNum)
{
	return S_Table.isValidStmt(stmtNum);
}

/***************** ConstantTable ******************/
void PKB::addConstant(int constant, int stmt)
{
	C_Table.addConstant(constant, stmt);
}

list<int> PKB::getConstantList()
{
	return C_Table.getConstantList();;
}

list<int> PKB::getStmtlineByConstant(int c)
{
	return C_Table.getStmtlineByConstant(c);
}

/***************** CallsTable ******************/
void PKB::addCalls(string caller, string callee)
{
	Calls_Table.addCalls(caller, callee);
}

list<string> PKB::getCallee(string caller)
{
	return Calls_Table.getCallee(caller);
}

list<string> PKB::getCaller(string callee)
{
	return Calls_Table.getCaller(callee);
}

bool PKB::isCallExist()
{
	return Calls_Table.isCallExist();
}

bool PKB::isCall(string caller, string callee)
{
	return Calls_Table.isCall(caller, callee);
}

bool PKB::isCallStar(string caller, string callee)
{
	return Calls_Table.isCallStar(caller, callee);
}

list<string> PKB::getCalleeStar(string caller)
{
	return Calls_Table.getCalleeStar(caller);
}

list<string> PKB::getCallerStar(string callee)
{
	return Calls_Table.getCallerStar(callee);
}

/***************** CallsGraph ******************/

void PKB::initializeCallsGraph(int vertexCount)
{
	Calls_Graph.initializeCallsGraph(vertexCount);
}

void PKB::addCallsGraphEdge(int i, int j)
{
	Calls_Graph.addCallsGraphEdge(i, j);
}

bool PKB::hasArcCallsGraph(int i, int j)
{
	return Calls_Graph.hasArcCallsGraph(i, j);
}

bool PKB::isCallsGraphCyclic()
{
	return Calls_Graph.isCallsGraphCyclic();
}

/***************** ProcTable ******************/
void PKB::addProc(string procName)
{
	Proc_Table.addProc(procName);
}

void PKB::addProcMod(string procName, string var)
{
	Proc_Table.addProcMod(procName, var);
}

void PKB::addProcUses(string procName, string var)
{
	Proc_Table.addProcUses(procName, var);
}

void PKB::addProcCalledInStmt(string procName, int stmtLine)
{
	Proc_Table.addProcCalledInStmt(procName, stmtLine);
}

string PKB::getProcName(int procIndex)
{
	return Proc_Table.getProcName(procIndex);
}

int PKB::getProcIndex(string procName)
{
	return Proc_Table.getProcIndex(procName);
}

bool PKB::isModifiedByProc(string procName, string varName)
{
	return Proc_Table.isModifiedByProc(procName, varName);
}

bool PKB::isUsedByProc(string procName, string varName)
{
	return Proc_Table.isUsedByProc(procName, varName);
}

list<string> PKB::getModifiedByProc(string procName)
{
	return Proc_Table.getModifiedByProc(procName);
}

list<string> PKB::getUsedByProc(string procName)
{
	return Proc_Table.getUsedByProc(procName);
}

list<string> PKB::getProcedureModifying(string varName)
{
	return Proc_Table.getProcedureModifying(varName);
}

list<string> PKB::getProcedureUsing(string varName)
{
	return Proc_Table.getProcedureUsing(varName);
}

bool PKB::isProcedureExist(string procName)
{
	return Proc_Table.isProcedureExist(procName);
}

list<int> PKB::getCallByProcName(string procName)
{
	return Proc_Table.getCallByProcName(procName);
}

list<string> PKB::getCalledProcNamesList()
{
	return Proc_Table.getCalledProcNamesList();
}

list<string> PKB::getProcedureList()
{
	return Proc_Table.getProcedureList();
}

/***************** AST ******************/
string PKB::makeExpr(string input)
{
	return Ast.makeExpr(input);
}

void PKB::addExpr(int stmt, string expr)
{
	Ast.addExpr(stmt, expr);
}

bool PKB::isExprExist(string expr)
{
	return Ast.isExprExist(expr);
}

bool PKB::isSubExprExist(string subExpr)
{
	return Ast.isSubExprExist(subExpr);
}

list<int> PKB::getAssignWithExpression(string expr)
{
	return Ast.getAssignWithExpression(expr);
}

list<int> PKB::getAssignWithSubExpression(string subExpr)
{
	return Ast.getAssignWithSubExpression(subExpr);
}

/***************** CFG ******************/
void PKB::addProcCFG()
{
	Cfg.addProcCFG();
}

void PKB::addStmtCFG(int stmtnum, string stmtType)
{
	Cfg.addStmtCFG(stmtnum, stmtType);
}
void PKB::closeIfCFG()
{
	Cfg.closeIfCFG();
}

void PKB::closeElseCFG()
{
	Cfg.closeElseCFG();
}

void PKB::closeWhileCFG()
{
	Cfg.closeWhileCFG();
}

void PKB::buildCFGMatrix()
{
	Cfg.buildCFGMatrix();
}

bool PKB::isNext(int n1, int n2)
{
	return Cfg.isNext(n1, n2);
}

bool PKB::isNextStar(int n1, int n2)
{
	return Cfg.isNextStar(n1, n2);
}

bool PKB::isNextEmpty()
{
	return Cfg.isNextEmpty();
}

list<int> PKB::getExecutedBefore(int n)
{
	return Cfg.getExecutedBefore(n);
}

list<int> PKB::getExecutedAfter(int n)
{
	return Cfg.getExecutedAfter(n);
}

list<int> PKB::getExecutedBeforeStar(int n)
{
	return Cfg.getExecutedBeforeStar(n);
}

list<int> PKB::getExecutedAfterStar(int n)
{
	return Cfg.getExecutedAfterStar(n);
}

string PKB::getStmtType(int stmtNum)
{
	return S_Table.getStmtType(stmtNum);
}

int PKB::getProcBeginStmt(int stmtNum)
{
	if (stmt_to_proc_begin_table[stmtNum] != 0) {
		return stmt_to_proc_begin_table[stmtNum];
	}
	else {
		return -1;
	}
}

vector<int> PKB::getProcedureFirstStmtList()
{
	return procedure_first_stmts_list;
}

list<int> PKB::getAllStmtLst()
{
	return all_stmtlst;
}

bool PKB::isAffects(int assign_stmt1, int assign_stmt2)
{
	set<pair<int, int>> affected_list;
	map<string, set<int>> modifies_map;
	Node proc_begin_of_assign_stmt = Cfg.getNodeByStmt(getProcBeginStmt(assign_stmt1));
	getAffectsBothSyn(affected_list, modifies_map, proc_begin_of_assign_stmt);
	for (auto &affected_pair : affected_list) {
		if (affected_pair.first == assign_stmt1 && affected_pair.second == assign_stmt2) {
			return true;
		}
	}

	return false;
}

bool PKB::isAffectsStar(int assign_stmt1, int assign_stmt2)
{
	set<pair<int, int>> affected_star_list;
	map<string, set<int>> modifies_map;
	Node proc_begin_of_assign_stmt = Cfg.getNodeByStmt(getProcBeginStmt(assign_stmt1));
	getAffectsStarBothSyn(affected_star_list, modifies_map, proc_begin_of_assign_stmt);
	for (auto &affected_star_pair : affected_star_list) {
		if (affected_star_pair.first == assign_stmt1 && affected_star_pair.second == assign_stmt2) {
			return true;
		}
	}

	return false;
}

bool PKB::isAffectsEmpty()
{
	return false;
}

bool PKB::isAffectsStarEmpty()
{
	return false;
}

list<int> PKB::getAffected(int assign_stmt)
{
	list<int> results;
	set<pair<int, int>> affected_list;
	map<string, set<int>> modifies_map;
	Node proc_begin_of_assign_stmt = Cfg.getNodeByStmt(getProcBeginStmt(assign_stmt));
	getAffectsBothSyn(affected_list, modifies_map, proc_begin_of_assign_stmt);
	for (auto &affected_pair : affected_list) {
		if (affected_pair.first == assign_stmt) {
			results.push_back(affected_pair.second);
		}
	}

	return results;
}

list<int> PKB::getAffectedStar(int affector_stmt_num)
{
	list<int> results;
	set<pair<int, int>> affected_star_list;
	map<string, set<int>> modifies_map;
	Node proc_begin_of_assign_stmt = Cfg.getNodeByStmt(getProcBeginStmt(affector_stmt_num));
	getAffectsStarBothSyn(affected_star_list, modifies_map, proc_begin_of_assign_stmt);
	for (auto &affected_star_pair : affected_star_list) {
		if (affected_star_pair.first == affector_stmt_num) {
			results.push_back(affected_star_pair.second);
		}
	}

	return results;
}

list<int> PKB::getAffector(int assign_stmt)
{
	list<int> results;
	set<pair<int, int>> affected_list;
	map<string, set<int>> modifies_map;
	Node proc_begin_of_assign_stmt = Cfg.getNodeByStmt(getProcBeginStmt(assign_stmt));
	getAffectsBothSyn(affected_list, modifies_map, proc_begin_of_assign_stmt);
	for (auto &affected_pair : affected_list) {
		if (affected_pair.second == assign_stmt) {
			results.push_back(affected_pair.first);
		}
	}

	return results;
}

list<int> PKB::getAffectorStar(int assign_stmt)
{
	list<int> results;
	set<pair<int, int>> affected_star_list;
	map<string, set<int>> modifies_map;
	Node proc_begin_of_assign_stmt = Cfg.getNodeByStmt(getProcBeginStmt(assign_stmt));
	getAffectsStarBothSyn(affected_star_list, modifies_map, proc_begin_of_assign_stmt);
	for (auto &affected_star_pair : affected_star_list) {
		if (affected_star_pair.second == assign_stmt) {
			results.push_back(affected_star_pair.first);
		}
	}

	return results;
}

list<pair<int, int>> PKB::getAffectsBothSyn(bool is_fill_cache)
{
	list<pair<int, int>> results;
	set<pair<int, int>> affects_list;
	map<string, set<int>> modifiesMap;
	vector<int> startStmtList = getProcedureFirstStmtList();

	if (is_affects_cache_filled) {
		for (auto &result_pair : affects_cache) {
			results.push_back(result_pair);
		}
		return results;
	}
	else {
		for (vector<int>::iterator it = startStmtList.begin(); it != startStmtList.end(); ++it)
		{
			set<pair<int, int>> affects_list;
			Node currNode = Cfg.getNodeByStmt(*it);
			getAffectsBothSyn(affects_list, modifiesMap, currNode);
			for (auto &result_pair : affects_list) {
				results.push_back(result_pair);
				if (is_fill_cache && !is_affects_cache_filled) {
					affects_cache.insert(result_pair);
				}
			}
		}
	}

	if (is_fill_cache) {
		is_affects_cache_filled = true;
	}

	return results;
}

map<string, set<int>> PKB::getAffectsBothSyn(set<pair<int, int>>& affectsList, map<string, set<int>> modifiesMap, Node currNode)
{
	int currStmt = currNode.getStmtnum();
	string nodeType = getStmtType(currStmt);
	if (nodeType == "assign")
	{
		// Get used vars of assign and check against mod_map
		string curr_modified_var = getModifiedBy(currStmt).front();
		list<string> curr_stmt_used_vars = getUsedBy(currStmt);
		for (auto &used_var : curr_stmt_used_vars) {
			if (modifiesMap.find(used_var) != modifiesMap.end()) {
				for (auto &used_var_modified_stmt_num : modifiesMap[used_var]) {
					affectsList.insert(pair<int, int>(used_var_modified_stmt_num, currStmt));
				}
			}
		}
		modifiesMap[curr_modified_var] = set<int>();
		modifiesMap[curr_modified_var].insert(currStmt);

		// If got next node, recurse. Else, return map
		if (currNode.getNextList().empty()) {
			return modifiesMap;
		}
		else {
			Node next_node = *(currNode.getNextList().front());
			int next_node_stmt_num = next_node.getStmtnum();
			if (!while_stack.empty() && while_stack.top() == next_node_stmt_num) {
				while_stack.pop();
				return modifiesMap;
			}
			return getAffectsBothSyn(affectsList, modifiesMap, next_node);
		}
	}
	else if (nodeType == "call")
	{
		// Update modifiesMap (delete modifiedVar that are modified by this call statement)
		list<string> modifiedByCall = getModifiedBy(currStmt);
		for (auto &modified_variable : modifiedByCall) {
			if (modifiesMap.find(modified_variable) != modifiesMap.end()) {
				modifiesMap.erase(modified_variable);
			}
		}

		// Go next
		if (currNode.getNextList().empty())
		{
			return modifiesMap;
		}
		else
		{
			Node nextNode = *(currNode.getNextList().front());
			int next_node_stmt_num = nextNode.getStmtnum();
			if (!while_stack.empty() && while_stack.top() == next_node_stmt_num) {
				while_stack.pop();
				return modifiesMap;
			}
			return getAffectsBothSyn(affectsList, modifiesMap, nextNode);
		}
	}
	else if (nodeType == "if")
	{
		Node if_then_node = *(currNode.getNextList().at(0));
		Node else_node = *(currNode.getNextList().at(1));
		map<string, set<int>> merged_map;

		int if_then_node_stmt_num = if_then_node.getStmtnum();
		int else_node_stmt_num = else_node.getStmtnum();

		// Process the if and else chains separately, merge them later
		map<string, set<int>> ifMap = getAffectsBothSyn(affectsList, modifiesMap, if_then_node);
		map<string, set<int>> elseMap = getAffectsBothSyn(affectsList, modifiesMap, else_node);
		UnionMap(merged_map, ifMap, elseMap);
		// Go next
		Node terminalNode = getTerminalNodeByStmt(currStmt);
		if (terminalNode.getNextList().empty()) {
			return merged_map;
		}
		else {
			Node nextNode = *(terminalNode.getNextList().front());
			int next_node_stmt_num = nextNode.getStmtnum();
			if (!while_stack.empty() && while_stack.top() == next_node_stmt_num) {
				while_stack.pop();
				return merged_map;
			}
			return getAffectsBothSyn(affectsList, merged_map, nextNode);
		}
	}
	else if (nodeType == "while")
	{
		bool has_out = currNode.getNextList().size() == 2;
		Node into_while_node = *(currNode.getNextList().at(0));
		Node out_of_while_node; 

		if (has_out) {
			out_of_while_node = *(currNode.getNextList().at(1));
		}

		map<string, set<int>> merged_set;
		working_set_stack.push(modifiesMap);
		while_stack.push(currStmt);
		map<string, set<int>> into_while_set = getAffectsBothSyn(affectsList, modifiesMap, into_while_node);
		map<string, set<int>> popped_set = working_set_stack.top();
		working_set_stack.pop();
		UnionMap(merged_set, popped_set, into_while_set);
		while (IsMapEqual(popped_set, merged_set) == false) {
			while_stack.push(currStmt);
			working_set_stack.push(merged_set);
			modifiesMap = getAffectsBothSyn(affectsList, merged_set, into_while_node);
			popped_set = working_set_stack.top();
			working_set_stack.pop();
			UnionMap(merged_set, modifiesMap, popped_set);
		}

		if (has_out) {
			if (!while_stack.empty() && while_stack.top() == out_of_while_node.getStmtnum()) {
				while_stack.pop();
				return merged_set;
			}
			else {
				return getAffectsBothSyn(affectsList, merged_set, out_of_while_node);
			}
		}
		else {
			return merged_set;
		}
	}
	else {
		// Should only be terminal node
		return modifiesMap;
	}
}

list<pair<int, int>> PKB::getAffectsStarBothSyn(bool is_fill_cache)
{
	list<pair<int,int>> results;
	map<string, set<int>> modifies_map;
	vector<int> procedure_first_stmts_list = getProcedureFirstStmtList();

	if (is_affects_star_cache_filled) {
		for (auto &result_pair : affects_star_cache) {
			results.push_back(result_pair);
		}
		return results;
	}
	else {
		for (auto &proc_first_stmt : procedure_first_stmts_list) {
			set<pair<int, int>> affected_star_list;
			Node curr_node = Cfg.getNodeByStmt(proc_first_stmt);
			getAffectsStarBothSyn(affected_star_list, modifies_map, curr_node);
			for (auto &result_pair : affected_star_list) {
				results.push_back(result_pair);
				if (is_fill_cache && !is_affects_star_cache_filled) {
					affects_star_cache.insert(result_pair);
				}
			}
		}
	}

	if (is_fill_cache) {
		is_affects_star_cache_filled = true;
	}

	return results;
}

map<string, set<int>> PKB::getAffectsStarBothSyn(set<pair<int, int>>& result_list, map<string, set<int>> modifies_map, Node curr_node)
{
	int curr_stmt_num = curr_node.getStmtnum();
	string curr_stmt_type = getStmtType(curr_stmt_num);
	if (curr_stmt_type == "assign") {
		// Get used vars of assign and check against mod_map
		string curr_modified_var = getModifiedBy(curr_stmt_num).front();
		set<int> new_set_for_curr_modified_var;
		list<string> curr_stmt_used_vars = getUsedBy(curr_stmt_num);
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
		if (curr_node.getNextList().empty()) {
			return modifies_map;
		}
		else {
			Node next_node = *(curr_node.getNextList().front());
			int next_node_stmt_num = next_node.getStmtnum();
			if (!while_stack.empty() && while_stack.top() == next_node_stmt_num) {
				while_stack.pop();
				return modifies_map;
			}
			return getAffectsStarBothSyn(result_list, modifies_map, next_node);
		}
	}
	else if (curr_stmt_type == "if") {
		Node if_then_node = *(curr_node.getNextList().at(0));
		Node else_node = *(curr_node.getNextList().at(1));
		map<string, set<int>> merged_set;
		map<string, set<int>> if_then_map = getAffectsStarBothSyn(result_list, modifies_map, if_then_node);
		map<string, set<int>> else_map = getAffectsStarBothSyn(result_list, modifies_map, else_node);
		UnionMap(merged_set, if_then_map, else_map);
		Node terminal_node = getTerminalNodeByStmt(curr_stmt_num);
		if (terminal_node.getNextList().empty()) {
			return merged_set;
		}
		else {
			Node next_node = *(terminal_node.getNextList().front());
			int next_node_stmt_num = next_node.getStmtnum();
			if (!while_stack.empty() && while_stack.top() == next_node_stmt_num) {
				while_stack.pop();
				return merged_set;
			}
			return getAffectsStarBothSyn(result_list, merged_set, next_node);
		}
	}
	else if (curr_stmt_type == "while") {
		bool has_out = curr_node.getNextList().size() == 2;
		Node into_while_node = *(curr_node.getNextList().at(0));
		Node out_of_while_node;

		if (has_out) {
			out_of_while_node = *(curr_node.getNextList().at(1));
		}

		map<string, set<int>> merged_set;
		working_set_stack.push(modifies_map);
		while_stack.push(curr_stmt_num);
		map<string, set<int>> into_while_set = getAffectsStarBothSyn(result_list, modifies_map, into_while_node);
		map<string, set<int>> popped_set = working_set_stack.top();
		working_set_stack.pop();
		UnionMap(merged_set, popped_set, into_while_set);
		while (IsMapEqual(popped_set, merged_set) == false) {
			while_stack.push(curr_stmt_num);
			working_set_stack.push(merged_set);
			modifies_map = getAffectsStarBothSyn(result_list, merged_set, into_while_node);
			popped_set = working_set_stack.top();
			working_set_stack.pop();
			UnionMap(merged_set, modifies_map, popped_set);
		}

		if (has_out) {
			if (!while_stack.empty() && while_stack.top() == out_of_while_node.getStmtnum()) {
				while_stack.pop();
				return merged_set;
			}
			else {
				return getAffectsStarBothSyn(result_list, merged_set, out_of_while_node);
			}
		}
		else {
			return merged_set;
		}
	}
	else if (curr_stmt_type == "call") {
		list<string> curr_modified_vars = getModifiedBy(curr_stmt_num);
		for (auto &modified_var : curr_modified_vars) {
			if (modifies_map.find(modified_var) != modifies_map.end()) {
				modifies_map.erase(modified_var);
			}
		}

		// If got next node, recurse. Else, return map
		if (curr_node.getNextList().empty()) {
			return modifies_map;
		}
		else {
			Node next_node = *(curr_node.getNextList().front());
			int next_node_stmt_num = next_node.getStmtnum();
			if (!while_stack.empty() && while_stack.top() == next_node_stmt_num) {
				while_stack.pop();
				return modifies_map;
			}
			return getAffectsStarBothSyn(result_list, modifies_map, next_node);
		}
	}
	else {
		// Should only be terminal node
		return modifies_map;
	}
}

Node PKB::getTerminalNodeByStmt(int if_stmt_num)
{
	return Cfg.getTerminalNodeByStmt(if_stmt_num);
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