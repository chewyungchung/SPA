#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "PKB.h"
#include "TNode.h"
#include "ModUsesTablebyStmt.h"
#include "ModUsesTablebyVariable.h"
#include "StatementTable.h"
#include "FollowsTable.h"
#include "ParentTable.h"

int PKB::setProcToAST(PROC p, TNode* r) {
	return NULL;
}

TNode* PKB::getRootAST (PROC p){
	return NULL;
}

PKB::PKB() {
	instance = new PKB;
	M_U_TableByStmt = new ModUsesTablebyStmt;
	M_U_TableByVar = new ModUsesTablebyVariable;
	S_Table = new StatementTable;
	F_Table = new FollowsTable;
	P_Table = new ParentTable;
}

PKB::~PKB() {
	delete M_U_TableByVar;
}

PKB * PKB::getPKB()
{
	if (instance == NULL) {
		PKB::PKB();
	}
	return instance;
}

void PKB::resetPKB() {
	delete instance;
	instance = NULL; // so that getPKB still works.
}

int PKB::getParentOf(int stmt) {
	return P_Table->getParentOf(stmt);
}
/*
vector<int> PKB::getParentStar(int stmt) {
	return vector<int>();
}
*/
list<int> PKB::getChildrenOf(int stmt) {
	return P_Table->getChildrenOf(stmt);
}

bool PKB::isParentEmpty() {
	return P_Table->isParentEmpty();
}

bool PKB::isParentOf(int parentStmt, int childStmt)
{
	return P_Table->isParentOf(parentStmt, childStmt);
}
/*
bool PKB::isParentStar(int stmt1, int stmt2) {
	return false;
}
*/
int PKB::getFollowedFrom(int stmt) {
	return F_Table->getFollowedFrom(stmt);
}

int PKB::getFollower(int stmt) {
	return F_Table->getFollower(stmt);
}
/*
vector<int> PKB::getFollowedFromStar(int stmt) {
	return vector<int>();
}

vector<int> PKB::getFollowerStar(int stmt) {
	return vector<int>();
}
*/
bool PKB::isFollowEmpty() {
	return F_Table->isFollowEmpty();
}

bool PKB::isValidFollows(int followedFrom, int follower) {
	return F_Table->isValidFollows(followedFrom, follower);
}
/*
bool PKB::isFollowsStar(int stmt1, int stmt2) {
	return false;
}
*/

/* ModUsesTablebyStatement*/

void PKB::addModifies(int stmtNum, string var) {
	M_U_TableByStmt->addModifies(stmtNum, var);
}

void PKB::addUses(int stmtNum, string var) {
	M_U_TableByStmt->addUses(stmtNum, var);
}

bool PKB::isValidStmt(int stmtNum) {
	return M_U_TableByStmt->isValidStmt(stmtNum);
}

bool PKB::isModified(int stmtNum, string varName) {
	return M_U_TableByStmt->isModified(stmtNum, varName);
}

bool PKB::isUsed(int stmtNum, string varName) {
	return M_U_TableByStmt->isUsed(stmtNum, varName);
}

list<string> PKB::getModifiedBy(int stmtNum) {
	return M_U_TableByStmt->getModifiedBy(stmtNum);
}

list<string> PKB::getUsedBy(int stmtNum) {
	return M_U_TableByStmt->getUsedBy(stmtNum);
}

/* ModUsesTablebyVariable*/

void PKB::addModifies(string var, int stmtNum) {
	M_U_TableByVar->addModifies(var, stmtNum);
}

void PKB::addUses(string var, int stmtNum) {
	M_U_TableByVar->addUses(var, stmtNum);
}

bool PKB::isValidVar(string varName) {
	return M_U_TableByVar->isValidVar(varName);
}

list<int> PKB::getModifiedBy(string varName) {
	return M_U_TableByVar->getModifiedBy(varName);
}

list<int> PKB::getUsedBy(string varName) {
	return M_U_TableByVar->getUsedBy(varName);
}

list<string> PKB::getAllModVar() {
	return M_U_TableByVar->getAllModVar();
}

list<string> PKB::getAllUsedVar() {
	return M_U_TableByVar->getAllUsedVar();
}

list<string> PKB::getVarList() {
	return M_U_TableByVar->getVarList();
}

/* Statement Table */
void PKB::addStatement(int stmtNum, string stmtType) {
	S_Table->addStatement(stmtNum, stmtType);
}

list<int> PKB::getAssignList() {
	return S_Table->getAssignList();
}

list<int> PKB::getWhileList() {
	return S_Table->getWhileList();
}

list<int> PKB::getStmtList() {
	return S_Table->getStmtList();
}

int PKB::getStatementCount() {
	return S_Table->getStatementCount();
}