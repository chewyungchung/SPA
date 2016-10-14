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

list<int> PKB::getIfListWithControlVariable(string control_var) {
	return S_Table.getIfListWithControlVariable(control_var);
}

list<int> PKB::getWhileListWithControlVariable(string control_var) {
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

list<string> PKB::getCallees(string caller)
{
	return Calls_Table.getCallees(caller);
}

list<string> PKB::getCallers(string callee)
{
	return Calls_Table.getCallers(callee);
}

bool PKB::isCallsEmpty()
{
	return Calls_Table.isCallsEmpty();
}

bool PKB::isCalls(string caller, string callee)
{
	return Calls_Table.isCalls(caller, callee);
}

bool PKB::isCallsStar(string caller, string callee)
{
	return Calls_Table.isCallsStar(caller, callee);
}

list<string> PKB::getCalleesStar(string caller)
{
	return Calls_Table.getCalleesStar(caller);
}

list<string> PKB::getCallersStar(string callee)
{
	return Calls_Table.getCallersStar(callee);
}

/***************** CallsGraph ******************/

void PKB::buildCallsGraph(int vertexCount, PKB pkb)
{
	this->Calls_Graph = CallsGraph(vertexCount, pkb);
}

bool PKB::isCallsGraphCyclic()
{
	return Calls_Graph.isCallsGraphCyclic();
}

void PKB::updateAllProcModUses()
{
	Calls_Graph.updateAllProcModUses();
}

/***************** CallsTable ******************/
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

list<string> PKB::getCalledProcNamesList()
{
	return Proc_Table.getCalledProcNamesList();
}

list<string> PKB::getProcedureList()
{
	return Proc_Table.getProcedureList();
}