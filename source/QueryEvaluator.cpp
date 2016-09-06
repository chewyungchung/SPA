#include "Clause.h"
#include "QueryTable.h"
#include "QueryEvaluator.h"
#include <string>
#include <algorithm>

const string REL_FOLLOWS = "FOLLOWS";
const string REL_FOLLOWS_STAR = "FOLLOWS*";
const string REL_PARENT = "PARENT";
const string REL_PARENT_STAR = "PARENT*";
const string REL_MODIFIES = "MODIFIES";
const string REL_USES = "USES";

const string STRING_ASSIGN_PATTERN = "ASSIGNPATTERN";

const string ARGTYPE_CONSTANT = "CONSTANT";
const string ARGTYPE_SYN = "SYN";
const string ARGTYPE_ANY = "ANY";
const string ARGTYPE_BOOLEAN = "BOOLEAN";
const string ARGTYPE_VARIABLE = "VARIABLE";
const string ARGTYPE_ASSIGN = "ASSIGN";
const string ARGTYPE_WHILE = "WHILE";


// Default Constructor
QueryEvaluator::QueryEvaluator() {

}

QueryEvaluator::QueryEvaluator(QueryTable qt) {
	_qt = qt;
}

vector<string> QueryEvaluator::evaluate() {
	// Process all such that and pattern clause, and finally select clause.
	processSuchThat(_qt.getSuchThatClause);
	processPattern(_qt.getPatternClause);
	processSelect(_qt.getSelectClause);
	return _result;
}

bool QueryEvaluator::processSelect(Clause selectClause) {
	// Results that output stmt#: 
	// -> stmt | assign | while | variable | constant | prog_line
	// Other results:
	// -> variable | boolean

	string expectedResultType = selectClause.getArgType().at(0);

	if (expectedResultType == ARGTYPE_VARIABLE) 
	{
		// Results should be returning VAR
	}
	else if (expectedResultType == ARGTYPE_BOOLEAN) 
	{
		// Results should be returning T/F
	}
	else if (expectedResultType == ARGTYPE_CONSTANT)
	{
		// Results should be returning constants
	}
	else 
	{
		// Results should be returning stmt#
	}
}

bool QueryEvaluator::processSuchThat(Clause suchThatClause) {
	string relation = suchThatClause.getRelation();

	QueryResult suchThatResult;

	if (relation == REL_FOLLOWS)
	{
		suchThatResult = processFollows(suchThatClause);
	}
	else if (relation == REL_FOLLOWS_STAR)
	{
		suchThatResult = processFollowsT(suchThatClause);
	}
	else if (relation == REL_PARENT)
	{
		suchThatResult = processParent(suchThatClause);
	}
	else if (relation == REL_PARENT_STAR)
	{
		suchThatResult = processParentT(suchThatClause);
	}
	else if (relation == REL_MODIFIES)
	{
		suchThatResult = processModifies(suchThatClause);
	}
	else if (relation == REL_USES)
	{
		suchThatResult = processUses(suchThatClause);
	}

	if (suchThatResult.getIsExist()) 
	{
		return true;
	}

	if (suchThatResult.isArg1ResultEmpty() && suchThatResult.isArg2ResultEmpty()) 
	{
		return false;
	}
	else {
		_qt.setSuchThatResult(suchThatResult);
	}
	return true;
}

bool QueryEvaluator::processPattern(Clause patternClause) {
	string relation = patternClause.getRelation();

	if (relation == STRING_ASSIGN_PATTERN)
	{

	}

	return false;
}

QueryResult QueryEvaluator::processFollows(Clause followClause) {
	string arg1 = followClause.getArg().at(0);
	string arg2 = followClause.getArg().at(1);
	string arg1Type = followClause.getArgType().at(0);
	string arg2Type = followClause.getArgType().at(1);

	QueryResult qr;

	if (arg1Type == ARGTYPE_CONSTANT)
	{
		// If arg1 not valid stmt, return empty QueryResult
		// if (!PKB.isValidStmt(arg1)) return qr;
		
		// If arg1 no follower, return empty QueryResult
		// int arg1Follower = PKB.getFollower(arg1);
		// if (!arg1Follower) return qr;

		if (arg2Type == ARGTYPE_CONSTANT)
		{
			// If arg2 not valid stmt, return empty QueryResult
			// if (!PKB.isValidStmt(arg2)) return qr;

			// If follows(arg1, arg2) true, set QueryResult to true and return.
			// Else just return
			// if (PKB.isValidFollows(arg1, arg2)) qr.setIsExist(true); return qr;
		}
		else if (arg2Type == ARGTYPE_ANY)
		{
			// At this point, arg1 determined to have follower
			// Hence, return QueryResult set to true
			// qr.setIsExist(true); return qr;
		}
		else
		{
			// Arg2 is a synonym
			// Remind JIAFENG to change design of QueryTable
			// Remove syn-entity map. Instead, during query parsing, allocate
			// design-entity typing to synonym

			list<int> synonymStatementsArg2;
			if (arg2Type == ARGTYPE_ASSIGN)
			{
				// list<int> synonymStatementsArg2 = PKB.getAssignList();
			}
			else if (arg2Type == ARGTYPE_WHILE)
			{
				// list<int> synonymStatementsArg2 = PKB.getWhileList();
			}
			// Check if follower is in while list
			// If found, add follower to result list + add synonym to synonym list
			// if (isInlist(synonymStatementsArg2, arg2)) 
			// qr.insertResult(arg2); qr.insertSynonym(arg2); qr.setIsExist(true); return qr;
		}
	}
	else if (arg1Type == ARGTYPE_ANY)
	{
		if (arg2Type == ARGTYPE_CONSTANT)
		{
			// If arg2 not valid stmt, return empty QueryResult
			// if (!PKB.isValidStmt(arg2)) return qr;

			// If have followed from, set QueryResult to true & return
			// int arg2FollowedFrom = PKB.getFollowedFrom(arg2);
			// if (arg2FollowedFrom) qr.setIsExist(true); 
			// return qr;
		}
		else if (arg2Type == ARGTYPE_ANY)
		{
			// If follow table not empty, result exists
			// if (!PKB.isFollowEmpty()) qr.setIsExist(true);
			// return qr;
		}
		else
		{
			// arg2 is a synonym
			// Check all statements of arg2Type for followedFrom
			// Add all followedFrom to result list + add synonym to synonym list
			list<int> synonymStatementsArg2;
			synonymStatementsArg2 = getList(arg2Type);
			
			// If program does not have any statements of arg2Type,
			// then this relationship evaluates to false
			if (synonymStatementsArg2.empty()) return qr;
			// else qr.insertSynonym(arg2);

			for (list<int>::iterator it = synonymStatementsArg2.begin(), end = synonymStatementsArg2.end(); it != end; ++it)
			{
				// int followedFrom = PKB.getFollowedFrom(*it);
				// if (followedFrom) qr.insertResult(followedFrom);
			}
			return qr;
		}
	}
	else 
	{
		// arg1 is a synonym
		list<int> synonymStatementsArg1;
		synonymStatementsArg1 = getList(arg1Type);

		// If no statements of arg1type exist, query is false
		if (synonymStatementsArg1.empty()) return qr;

		// At this point, insert the synonym into the result since it is involved
		// qr.insertSynonym(arg1);

		if (arg2Type == ARGTYPE_CONSTANT)
		{
			// If arg2 not valid stmt, query is false
			// if (!PKB.isValidStmt(arg2)) return qr;

			// Check if any of the statements in synonymStatementsArg1 is followed by arg2
			// If any of them are true, add it to the query results
			// for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) 
			// {
			//		if (PKB.isValidFollows(*it, arg2)) qr.insertResult(*it); qr.setIsExist(true);
			// }
			return qr;

		}
		else if (arg2Type == ARGTYPE_ANY)
		{
			// Look for followers of all statements of arg1Type
			// For all found, add to qr
			// for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++)
			// {
			//		int arg1Follower = *it;
			//		if (arg1Follower) { qr.insertResult(arg1Follower); qr.setIsExist(true); }
			// }
			return qr;
		}
		else
		{
			// arg2 is a synonym
			list<int> synonymStatementsArg2;
			synonymStatementsArg2 = getList(arg2Type);

			// If program does not have any statements of arg2Type,
			// then this relationship evaluates to false
			if (synonymStatementsArg2.empty()) return qr;
			// else qr.insertSynonym(arg2);

			// For each statement of arg1Type, check if each statement of arg2Type is its follower
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) 
			{
				for (list<int>::iterator it2 = synonymStatementsArg2.begin(); it2 != synonymStatementsArg2.end(); it1++)
				{
					// if (PKB.isValidFollows(*it1, *it2)) { qr.insertResult(*it1); qr.insertResult(*it2); qr.setIsExist(true); }
				}
			}
			return qr;
		}

	}
	return qr; 
}

QueryResult QueryEvaluator::processFollowsT(Clause followTClause) {
	return _result;
}

QueryResult QueryEvaluator::processParent(Clause parentClause) {
	string arg1 = parentClause.getArg().at(0);
	string arg2 = parentClause.getArg().at(1);
	string arg1Type = parentClause.getArgType().at(0);
	string arg2Type = parentClause.getArgType().at(1);

	QueryResult qr;

	if (arg1Type == ARGTYPE_CONSTANT)
	{
		// arg1 must be valid stmt
		// if (!PKB.isValidStmt(arg1)) return qr;

		// arg1 must have child
		// if (!PKB.getChildOf(arg1)) return qr;

		if (arg2Type == ARGTYPE_CONSTANT)
		{
			// arg2 must be valid stmt
			// if (!PKB.isValidStmt(arg2)) return qr;

			// check if valid parent
			// if (PKB.isParentOf(arg1, arg2)) qr.setIsExist(true); 
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY)
		{
			// at this point determined that arg1 has child.
			qr.setIsExist(true);
			return qr;
		}
		else
		{
			// Arg2 is a synonym
			qr.insertSynonym(arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) return qr;

			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++)
			{
				// if (PKB.isParentOf(arg1, *it)) qr.insertResult(*it); qr.setIsExist(true);
			}
			return qr;
		}
	}
	else if (arg1Type == ARGTYPE_ANY)
	{
		if (arg2Type == ARGTYPE_CONSTANT)
		{
			// arg2 must be valid stmt
			// if (!PKB.isValidStmt(arg2)) return qr;

			//arg2 must have parent
			// if (PKB.getParentOf(arg2)) qr.setIsExist(true)

			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY)
		{
			// If ParentTable is non empty (with relationships), existential query
			// if (!PKB.isParentEmpty()) qr.setIsExist(true);
			return qr;
		}
		else
		{
			// Arg2 is a synonym
			qr.insertSynonym(arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) return qr;

			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++)
			{
				// int parentOfArg2  = PKB.getParentOf(*it);
				// if (parentOfArg2) qr.insertResult(*it); qr.setIsExist(true);
			}
			return qr;
		}
	}
	else 
	{
		// arg1 is a synonym
		qr.insertSynonym(arg1);
		list<int> synonymStatementsArg1 = getList(arg1Type);
		if (synonymStatementsArg1.empty()) return qr;

		if (arg2Type == ARGTYPE_CONSTANT)
		{
			// arg2 must be valid stmt
			// if (!PKB.isValidStmt(arg2)) return qr;

			// arg2 must have parent in synonymStatementsArg1
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++)
			{
				// if (PKB.isParentOf(*it, arg2)) qr.insertResult(*it); qr.setIsExist(true); return qr;
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY)
		{
			// if there exists at least 1 parent in synonymStatementsArg1 that has a child, query evaluates to true
			// for each synonymStatement that has a kid, add to resultList in qr
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++)
			{
				// int childOfArg1 = PKB.getChildOf(*it);
				// if (childOfArg1) qr.insertResult(*it); qr.setIsExist(true);
			}
			return qr;
		}
		else
		{
			// Arg2 is a synonym
			// Probably wrong implementation now... Gotta redesign QueryResult to handle 2 synonym cases
			qr.insertSynonym(arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) return qr;

			// To add.
		}
	}

	return _result;
}

QueryResult QueryEvaluator::processParentT(Clause parentTClause) {
	return _result;
}

QueryResult QueryEvaluator::processModifies(Clause modifiesClause) {
	return _result;
}

QueryResult QueryEvaluator::processUses(Clause usesClause) {
	return _result;
}


bool QueryEvaluator::isInList(list<int> inList, int item) {
	return find(inList.begin(), inList.end(), item) != inList.end();
}

bool QueryEvaluator::isListEmpty(list<int> inList) {
	return inList.empty();
}

list<int> QueryEvaluator::getList(string argType) {
	list<int> wantedList;
	if (argType == ARGTYPE_ASSIGN) {
		// wantedList = PKB.getAssignList();
	}
	else if (argType == ARGTYPE_WHILE) {
		// wantedList = PKB.getAssignList();
	}
	return wantedList;
}