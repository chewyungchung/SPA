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

bool PKB::IsAffects(int stmt1, int stmt2)
{
	if (!isNextStar(stmt1,stmt2)) {
		return false;
	}
	unordered_map<int, Node*> nodeTable = Cfg.getNodeTable();
	string var = getModifiedBy(stmt1).front();
	list<int> used;
	list<int> search;
	search.push_back(stmt1);
	while (!search.empty()) {
		int stmt = search.front();
		search.pop_front();
		Node* n = nodeTable.at(stmt);
		list<Node*> nextList = n->getNextList();
		for (Node* next : nextList) {
			int stmtline = next->getStmtnum();
			if (isUsed(stmtline,var)) {
				if (stmtline == stmt2) {
					return true;
				}
			}
			else if(!isModified(stmtline,var)){
				if (std::find(search.begin(), search.end(), stmtline) == search.end()) {
					search.push_back(stmtline);
				}
			}
		}
	}
	return false;
}

bool PKB::IsAffectsEmpty()
{
	return false;
}

list<int> PKB::GetAffected(int stmt)
{
	list<int> output;
	unordered_map<int, Node*> nodeTable = Cfg.getNodeTable();
	string var = getModifiedBy(stmt).front();
	list<int> used;
	list<int> search;
	search.push_back(stmt);
	while (!search.empty()) {
		int stmt = search.front();
		search.pop_front();
		Node* n = nodeTable.at(stmt);
		list<Node*> nextList = n->getNextList();
		for (Node* next : nextList) {
			int stmtline = next->getStmtnum();
			if (isUsed(stmtline, var)) {
				output.push_back(stmtline);
			}
			else if (!isModified(stmtline, var)) {
				if (std::find(search.begin(), search.end(), stmtline) == search.end()) {
					search.push_back(stmtline);
				}
			}
		}
	}
	return output;
}

list<int> PKB::GetAffector(int assign_stmt)
{
	return list<int>();
}

list<pair<int, int>> PKB::GetAffectsBothSyn()
{
	return list<pair<int, int>>();
}

bool PKB::IsAffectsStar(int assign_stmt1, int assign_stmt2)
{
	return false;
}

bool PKB::IsAffectsStarEmpty()
{
	return false;
}

list<int> PKB::GetAffectedStar(int assign_stmt)
{
	return list<int>();
}

list<int> PKB::GetAffectorStar(int assign_stmt)
{
	return list<int>();
}