#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
	
using namespace std;

#include "PKB.h"


//PKB* PKB::instance = new PKB;
//ModUsesTablebyStmt* PKB::M_U_TableByStmt = new ModUsesTablebyStmt;
//ModUsesTablebyVariable* PKB::M_U_TableByVar = new ModUsesTablebyVariable;
//StatementTable* PKB::S_Table = new StatementTable;
//FollowsTable* PKB::F_Table = new FollowsTable;
//ParentTable* PKB::P_Table = new ParentTable;
//ConstantTable* PKB::C_Table = new ConstantTable;

//PKB* PKB::instance = new PKB();
//ModUsesTablebyStmt* PKB::M_U_TableByStmt = new ModUsesTablebyStmt();
//ModUsesTablebyVariable* PKB::M_U_TableByVar = new ModUsesTablebyVariable();
//StatementTable* PKB::S_Table = new StatementTable();
//FollowsTable* PKB::F_Table = new FollowsTable();
//ParentTable* PKB::P_Table = new ParentTable();
//ConstantTable* PKB::C_Table = new ConstantTable();

int PKB::setProcToAST(PROC p, TNode* r) {
	return NULL;
}

TNode* PKB::getRootAST (PROC p){
	return NULL;
}

PKB::PKB() {
	//instance = new PKB();
	//M_U_TableByStmt = new ModUsesTablebyStmt();
	//M_U_TableByVar = new ModUsesTablebyVariable();
	//S_Table = new StatementTable();
	//F_Table = new FollowsTable();
	//P_Table = new ParentTable();
	//C_Table = new ConstantTable();
}

PKB::~PKB() {
}

//void PKB::destroyInstance() {
//	delete M_U_TableByStmt;
//	delete M_U_TableByVar;
//	delete S_Table;
//	delete F_Table;
//	delete P_Table;
//	delete C_Table;
//	delete instance;
//
//	instance = new PKB();
//	M_U_TableByStmt = new ModUsesTablebyStmt();
//	M_U_TableByVar = new ModUsesTablebyVariable();
//	S_Table = new StatementTable();
//	F_Table = new FollowsTable();
//	P_Table = new ParentTable();
//	C_Table = new ConstantTable();
//}

//PKB * PKB::getPKB()
//{
//	if (instance == NULL) {
//		PKB::PKB();
//	}
//	return instance;
//}

//void PKB::resetPKB() {
//	delete instance;
//	instance = NULL; // so that getPKB still works.
//}

void PKB::addParent(int lineOfParent, int lineNum) {
	P_Table.addParent(lineOfParent, lineNum);
}

int PKB::getParentOf(int stmt) {
	return P_Table.getParentOf(stmt);
}

list<int> PKB::getParentStar(int stmt) {	
	return P_Table.getParentStar(stmt);
}

list<int> PKB::getChildrenOf(int stmt) {
	return P_Table.getChildrenOf(stmt);
}

bool PKB::isParentEmpty() {
	return P_Table.isParentEmpty();
}

bool PKB::isParentOf(int parentStmt, int childStmt)
{
	return P_Table.isParentOf(parentStmt, childStmt);
}

bool PKB::isParentStar(int parent, int child) {
	return P_Table.isParentStar(parent, child);
}

/* The 'Follows' table*/

void PKB::addFollows(int lineNum, int nestingIndex) {
	F_Table.addFollows(lineNum, nestingIndex);
}

int PKB::getFollowedFrom(int stmt) {
	return F_Table.getFollowedFrom(stmt);
}

int PKB::getFollower(int stmt) {
	return F_Table.getFollower(stmt);
}

list<int> PKB::getFollowedFromStar(int stmt) {
	return F_Table.getFollowedFromStar(stmt);
}

list<int> PKB::getFollowerStar(int stmt) {
	return F_Table.getFollowerStar(stmt);
}

bool PKB::isFollowEmpty() {
	return F_Table.isFollowEmpty();
}

bool PKB::isValidFollows(int followedFrom, int follower) {
	return F_Table.isValidFollows(followedFrom, follower);
}

bool PKB::isFollowsStar(int stmt1, int stmt2) {
	return F_Table.isFollowsStar(stmt1, stmt2);
}

/* ModUsesTablebyStatement*/

void PKB::addModifies(int stmtNum, string var) {
	M_U_TableByStmt.addModifies(stmtNum, var);
}

void PKB::addUses(int stmtNum, string var) {
	M_U_TableByStmt.addUses(stmtNum, var);
}

bool PKB::isModified(int stmtNum, string varName) {
	return M_U_TableByStmt.isModified(stmtNum, varName);
}

bool PKB::isUsed(int stmtNum, string varName) {
	return M_U_TableByStmt.isUsed(stmtNum, varName);
}

list<string> PKB::getModifiedBy(int stmtNum) {
	return M_U_TableByStmt.getModifiedBy(stmtNum);
}

list<string> PKB::getUsedBy(int stmtNum) {
	return M_U_TableByStmt.getUsedBy(stmtNum);
}

/* ModUsesTablebyVariable*/

void PKB::addModifies(string var, int stmtNum) {
	M_U_TableByVar.addModifies(var, stmtNum);
}

void PKB::addUses(string var, int stmtNum) {
	M_U_TableByVar.addUses(var, stmtNum);
}

bool PKB::isValidVar(string varName) {
	return M_U_TableByVar.isValidVar(varName);
}

list<int> PKB::getModifiedBy(string varName) {
	return M_U_TableByVar.getModifiedBy(varName);
}

list<int> PKB::getUsedBy(string varName) {
	return M_U_TableByVar.getUsedBy(varName);
}

list<string> PKB::getAllModVar() {
	return M_U_TableByVar.getAllModVar();
}

list<string> PKB::getAllUsedVar() {
	return M_U_TableByVar.getAllUsedVar();
}

list<string> PKB::getVarList() {
	return M_U_TableByVar.getVarList();
}

/* Statement Table */
void PKB::addStatement(int stmtNum, string stmtType) {
	S_Table.addStatement(stmtNum, stmtType);
}

list<int> PKB::getAssignList() {
	return S_Table.getAssignList();
}

list<int> PKB::getWhileList() {
	return S_Table.getWhileList();
}

list<int> PKB::getStmtList() {
	return S_Table.getStmtList();
}

int PKB::getStatementCount() {
	return S_Table.getStatementCount();
}

bool PKB::isValidStmt(int stmtNum) {
	return S_Table.isValidStmt(stmtNum);
}

/* Constant Table */

void PKB::addConstant(int constant, int stmt)
{
	C_Table.addConstant(constant, stmt);
}

list<int> PKB::getConstantList(){
	return C_Table.getConstantList();;
}

list<int> PKB::getStmtlineByConstant(int c) {
	return C_Table.getStmtlineByConstant(c);
}