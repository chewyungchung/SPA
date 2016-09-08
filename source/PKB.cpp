#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "PKB.h"
#include "TNode.h"
#include "ModUsesTablebyVariable.h"

int PKB::setProcToAST(PROC p, TNode* r) {
	return NULL;
}

TNode* PKB::getRootAST (PROC p){
	return NULL;
}

PKB::PKB() {
	instance = new PKB;
	M_U_TableByVar = new ModUsesTablebyVariable;
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
/*
int PKB::getParentOf(int stmt) {
	return 0;
}

vector<int> PKB::getParentStar(int stmt) {
	return vector<int>();
}

vector<int> PKB::getChildrenOf(int stmt) {
	return vector<int>();
}

bool PKB::isParentEmpty() {
	return false;
}

bool PKB::isParentOf(int parentStmt, int childStmt)
{
	return false;
}

bool PKB::isParentStar(int stmt1, int stmt2) {
	return false;
}

int PKB::getFollowedFrom(int stmt) {
	return 0;
}

int PKB::getFollower(int stmt) {
	return 0;
}

vector<int> PKB::getFollowedFromStar(int stmt) {
	return vector<int>();
}

vector<int> PKB::getFollowerStar(int stmt) {
	return vector<int>();
}

bool PKB::isFollowEmpty() {
	return false;
}

bool PKB::isValidFollows(int followedFrom, int follower) {
	return false;
}

bool PKB::isFollowsStar(int stmt1, int stmt2) {
	return false;
}
*/
list<int> PKB::getUsedBy(string name) {
	return M_U_TableByVar->getUsedBy(name);
}
/*
vector<string> PKB::getUses(int stmt) {
	return vector<string>();
}

vector<int, vector<string>> PKB::getAllUses() {
	return vector<int, vector<string>>();
}
*/
bool PKB::isUsed(int stmt, string name) {
	return M_U_TableByVar->isUsed(stmt, name);
}

list<int> PKB::getModifiedBy(string name) {
	return M_U_TableByVar->getModifiedBy(name);
}
/*
vector<string> PKB::getModifies(int stmt) {
	return vector<string>();
}

vector<int, vector<string>> PKB::getAllModifies() {
	return vector<int, vector<string>>();
}
*/
bool PKB::isModified(int stmt, string name) {
	return M_U_TableByVar->isModified(stmt, name);
}
/*
vector<int> PKB::getAssignList() {
	return vector<int>();
}

vector<int> PKB::getWhileList() {
	return vector<int>();
}

vector<string> PKB::getVarList() {
	return vector<string>();
}

int PKB::getStatementCount() {
	return 0;
}

bool PKB::isValidStmt(int stmt) {
	return false;
}

vector<int> PKB::getConstantList(){
	return vector<int>();
}
*/