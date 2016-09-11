#include "Clause.h"
#include "QueryTable.h"
#include "QueryEvaluator.h"
#include <string>
#include <algorithm>

const string REL_FOLLOWS = "follows";
const string REL_FOLLOWS_STAR = "follows*";
const string REL_PARENT = "parent";
const string REL_PARENT_STAR = "parent*";
const string REL_MODIFIES = "modifies";
const string REL_USES = "uses";

const string STRING_ASSIGN_PATTERN = "ASSIGNPATTERN";

const string ARGTYPE_CONSTANT = "constant";
const string ARGTYPE_ANY = "any";
const string ARGTYPE_BOOLEAN = "boolean";
const string ARGTYPE_VARIABLE = "variable";
const string ARGTYPE_ASSIGN = "assign";
const string ARGTYPE_WHILE = "while";
const string ARGTYPE_STRING = "string";
const string ARGTYPE_STMT = "stmt";
const string ARGTYPE_PROG_LINE = "prog_line";

const string PARAM_ARG1 = "ARG1";
const string PARAM_ARG2 = "ARG2";
const string PARAM_PATTERN = "PATTERN";
const string PARAM_BOOLEAN = "BOOLEAN";


// Default Constructor
QueryEvaluator::QueryEvaluator() {

}
QueryEvaluator::~QueryEvaluator() {
	delete _qt;
}

QueryEvaluator::QueryEvaluator(QueryTable* qt) {
	_qt = qt;
}

QueryTable* QueryEvaluator::evaluate() {
	// Check if the QueryTable given by validator is NULL
	// If (yes), return NULL. Else proceed.
	if (_qt == NULL) {
		return NULL;
	}

	// Process all such that and pattern clause, and finally select clause
	_qt->setSuchThatResult(processSuchThat(_qt->getSuchThatClause())); 
	_qt->setPatternResult(processPattern(_qt->getPatternClause()));
	_qt->setSelectResult(processSelect(_qt->getSelectClause()));
	return _qt;
}

QueryResult* QueryEvaluator::processSelect(Clause* selectClause) {
	// Results that output stmt#: 
	// -> stmt | assign | while | prog_line
	// Other results:
	// -> variable | boolean | constant

	// May not be synonym. Could be BOOLEAN
	string expectedResultSynonym = selectClause->getArg().at(0);
	string expectedResultType = selectClause->getArgType().at(0);

	QueryResult* qr = NULL;

	//qr->insertSynonym(expectedResultSynonym);

	if (expectedResultType == ARGTYPE_VARIABLE) {
		// Results should be returning VAR
		list<string> varList;
		// varList = PKB.getVarList();
		if (varList.empty()) {
			return qr;
		}
		qr = new QueryResult();
		qr->setArgToSynonymMapping(PARAM_ARG1, expectedResultSynonym);
		qr->setIsExist(true);
		for (list<string>::iterator it = varList.begin(); it != varList.end(); it++) {
			qr->insertArg1Result(*it);
		}
	}
	else if (expectedResultType == ARGTYPE_BOOLEAN) {
		// Results should be returning T/F
		// Need to check in projector. qr->isExist will not be validated
		qr = new QueryResult();
		qr->setArgToSynonymMapping(PARAM_ARG1, PARAM_BOOLEAN);
		return qr;
	}
	else if (expectedResultType == ARGTYPE_CONSTANT) {
		// Results should be returning constants
		list<int> constantList;
		// constantList = PKB.getConstantList();
		if (constantList.empty()) {
			return qr;
		}
		qr = new QueryResult();
		qr->setArgToSynonymMapping(PARAM_ARG1, expectedResultSynonym);
		qr->setIsExist(true);
		for (list<int>::iterator it = constantList.begin(); it != constantList.end(); it++) {
			qr->insertArg1Result(to_string(*it));
		}
		return qr;
	}
	else {
		// Results should be returning stmt#
		if (expectedResultType == ARGTYPE_ASSIGN || expectedResultType == ARGTYPE_WHILE) {
			list<int> expectedResultStatements = getList(expectedResultType);
			if (expectedResultStatements.empty()) {
				return qr;
			}
			qr = new QueryResult();
			qr->setArgToSynonymMapping(PARAM_ARG1, expectedResultSynonym);
			qr->setIsExist(true);
			for (list<int>::iterator it = expectedResultStatements.begin(); it != expectedResultStatements.end(); it++) {
				qr->insertArg1Result(to_string(*it));
			}
			return qr;
		} 
		else {
			// expectedResultType should be either stmt OR progline
			int statementCount;
			// statementCount = PKB.getStatementCount();
			// statementCount will always be >= 1
			qr = new QueryResult();
			qr->setArgToSynonymMapping(PARAM_ARG1, expectedResultSynonym);
			qr->setIsExist(true);
			for (int i = 1; i <= statementCount; i++) {
				qr->insertArg1Result(to_string(i));
			}
			return qr;
		}
	}
	return qr;

}

QueryResult* QueryEvaluator::processSuchThat(Clause* suchThatClause) {
	string relation = suchThatClause->getRelation();

	QueryResult* suchThatResult = NULL;

	if (relation == REL_FOLLOWS) {
		suchThatResult = processFollows(suchThatClause);
	}
	else if (relation == REL_FOLLOWS_STAR) {
		suchThatResult = processFollowsT(suchThatClause);
	}
	else if (relation == REL_PARENT) {
		suchThatResult = processParent(suchThatClause);
	}
	else if (relation == REL_PARENT_STAR) {
		suchThatResult = processParentT(suchThatClause);
	}
	else if (relation == REL_MODIFIES) {
		suchThatResult = processModifies(suchThatClause);
	}
	else if (relation == REL_USES) {
		suchThatResult = processUses(suchThatClause);
	}
	return suchThatResult;
}

QueryResult* QueryEvaluator::processPattern(Clause* patternClause) {
	string arg1 = patternClause->getArg().at(0);
	string arg2 = patternClause->getArg().at(1);
	string arg1Type = patternClause->getArgType().at(0);
	string arg2Type = patternClause->getArgType().at(1);
	string synAssign = patternClause->getArg().at(2);
	string synAssignType = ARGTYPE_ASSIGN;

	list<int> synAssignStatements = getList(synAssignType);
	
	QueryResult* qr = NULL;

	// synAssign is the synonym
	//qr->insertSynonym(synAssign);

	if (arg1Type == ARGTYPE_STRING) {
		// arg1 must be valid varName
		// if (!PKB.isValidVar(arg1)) return qr;

		if (arg2Type == ARGTYPE_ANY) {
			// arg2 is any '_'

			// check if arg1 is modified by anything
			list<int> arg1ModifiedBy;
			// arg1ModifiedBy = PKB.getModifiedBy(arg1);
			if (arg1ModifiedBy.empty()) {
				return qr;
			}

			// if arg1ModifiedBy (made up of assign stmts in ite 1) is non empty, add them all
			// to pattern results
			qr = new QueryResult();
			qr->setIsExist(true);
			for (list<int>::iterator it = arg1ModifiedBy.begin(); it != arg1ModifiedBy.end(); it++) {
				qr->insertPatternResult(to_string(*it));
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_CONSTANT) {
			// TO BE IMPLEMENTED
		}
	}
	else if (arg1Type == ARGTYPE_ANY) {
		// arg1 is any '_'
		if (arg2Type == ARGTYPE_ANY) {
			// arg2 is any '_'

			list<int> assignList;
			// assignList = PKB.getAssignList();
			if (assignList.empty()) {
				return qr;
			}
			// if assignList not empty, add everything to pattern result
			qr = new QueryResult();
			qr->setIsExist(true);
			for (list<int>::iterator it = assignList.begin(); it != assignList.end(); it++) {
				qr->insertPatternResult(to_string(*it));
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_CONSTANT) {
			// TO BE IMPLEMENTED
		}
	}
	else {
		//arg1 is a variable 
		list<string> varList;
		// varList = PKB.getVarList();
		if (varList.empty()) {
			return qr;
		}
		qr = new QueryResult();
		qr->setArgToSynonymMapping(PARAM_ARG1, arg1);
		if (arg2Type == ARGTYPE_ANY) {
			// since synAssignStatements not empty, can assume that query is true
			// as something is modified for sure
			qr->setIsExist(true);
			// for each stmt in synAssignStatements, check each modifies what var
			for (list<int>::iterator it1 = synAssignStatements.begin(); it1 != synAssignStatements.end(); it1++) {
				bool outerIteratorIsValidResult = false;
				list<string> synAssignModifiesVar;
				// synAssignModifiesVar = PKB.getModifies(*it1);
				if (!synAssignModifiesVar.empty()) {
					for (list<string>::iterator it2 = synAssignModifiesVar.begin(); it2 != synAssignModifiesVar.end(); it2++) {
						// if (PKB.isModified(*it1, *it2)) outerIteratorIsValidResult = true; qr->insertArg1Result(*it2);
					}
					if (outerIteratorIsValidResult) {
						qr->insertPatternResult(to_string(*it1));
					}
				}
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_CONSTANT) {
			// TO BE IMPLEMENTED
		}
	}
	return qr;
}

QueryResult* QueryEvaluator::processFollows(Clause* followClause) {
	string arg1 = followClause->getArg().at(0);
	string arg2 = followClause->getArg().at(1);
	string arg1Type = followClause->getArgType().at(0);
	string arg2Type = followClause->getArgType().at(1);

	QueryResult* qr = NULL;

	if (arg1Type == ARGTYPE_CONSTANT) {
		// If arg1 not valid stmt, return empty QueryResult
		// if (!PKB.isValidStmt(stoi(arg1))) return qr;
		
		// If arg1 no follower, return empty QueryResult
		// int arg1Follower = PKB.getFollower(arg1);
		// if (!arg1Follower) return qr;

		if (arg2Type == ARGTYPE_CONSTANT) {
			// If arg2 not valid stmt, return empty QueryResult
			// if (!PKB.isValidStmt(stoi(arg2))) return qr;

			// If follows(arg1, arg2) true, set QueryResult to true and return.
			// Else just return
			// if (PKB.isValidFollows(arg1, arg2)) qr->setIsExist(true); 
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// At this point, arg1 determined to have follower from the first getFollower() check for arg1
			// Hence, return QueryResult set to true
			// qr->setIsExist(true); 
			return qr;
		}
		else {
			// Arg2 is a synonym
			//qr->insertSynonym(arg2);
			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);

			// If program does not have any statements of arg2Type,
			// then this relationship evaluates to false
			if (synonymStatementsArg2.empty()) {
				return qr;
			}

			// Check if any of the statement in synonymStatementsArg2 list is a follower of arg1
			// If found, add to arg2ResultList and set query to true
			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++) {
				// if (PKB.isValidFollow(arg1, *it)) qr->insertArg2Result(to_string(*it)); qr->setIsExist(true);
			}
			return qr;
		}
	}
	else if (arg1Type == ARGTYPE_ANY) {
		if (arg2Type == ARGTYPE_CONSTANT) {
			// If arg2 not valid stmt, return empty QueryResult
			// if (!PKB.isValidStmt(stoi(arg2))) return qr;

			// If have followed from, set QueryResult to true & return
			// int arg2FollowedFrom = PKB.getFollowedFrom(arg2);
			// if (arg2FollowedFrom) qr->setIsExist(true); 
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// If follow table not empty, result exists
			// if (!PKB.isFollowEmpty()) qr->setIsExist(true);
			return qr;
		}
		else {
			// arg2 is a synonym
			//qr->insertSynonym(arg2);
			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2;
			synonymStatementsArg2 = getList(arg2Type);

			// Check all statements of arg2Type for followedFrom
			// Add all followedFrom to result list

			// If program does not have any statements of arg2Type,
			// then this relationship evaluates to false
			if (synonymStatementsArg2.empty()) return qr;

			for (list<int>::iterator it = synonymStatementsArg2.begin(), end = synonymStatementsArg2.end(); it != end; ++it) {
				// int followedFrom = PKB.getFollowedFrom(*it);
				// if (followedFrom) qr->insertArg2Result(to_string(followedFrom));
			}
			return qr;
		}
	}
	else {
		// arg1 is a synonym
		//qr->insertSynonym(arg1);
		qr->setArgToSynonymMapping(PARAM_ARG1, arg1);
		list<int> synonymStatementsArg1;
		synonymStatementsArg1 = getList(arg1Type);

		// If no statements of arg1type exist, query is false
		if (synonymStatementsArg1.empty()) {
			return qr;
		}

		if (arg2Type == ARGTYPE_CONSTANT) {
			// If arg2 not valid stmt, query is false
			// if (!PKB.isValidStmt(stoi(arg2))) return qr;

			// Check if any of the statements in synonymStatementsArg1 is followed by arg2
			// If any of them are true, add it to the query results
			// for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) 
			// {
			//		if (PKB.isValidFollows(*it, arg2)) qr->insertArg1Result(to_string(*it)); qr->setIsExist(true);
			// }
			return qr;

		}
		else if (arg2Type == ARGTYPE_ANY) {
			// Look for followers of all statements of arg1Type
			// For all found, add to qr
			// for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++)
			// {
			//		int arg1Follower = PKB.getFollower(*it);
			//		if (arg1Follower) { qr->insertArg1Result(to_string(arg1Follower)); qr->setIsExist(true); }
			// }
			return qr;
		}
		else {
			// arg2 is a synonym

			// corner case. f(s,s)
			// if same synonym, return empty result
			// if (arg2 == arg1) return qr;

			//qr->insertSynonym(arg2);
			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2;
			synonymStatementsArg2 = getList(arg2Type);

			// If program does not have any statements of arg2Type,
			// then this relationship evaluates to false
			if (synonymStatementsArg2.empty()) return qr;

			// For each statement of arg1Type, check if each statement of arg2Type is its follower
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				bool outerIteratorIsValidResult = false;
				for (list<int>::iterator it2 = synonymStatementsArg2.begin(); it2 != synonymStatementsArg2.end(); it1++) {
					// if (PKB.isValidFollows(*it1, *it2)) { outerIteratorIsValidResult = true; qr->insertArg2Result(to_string(*it2)); qr->setIsExist(true); }
				}
				if (outerIteratorIsValidResult) {
					//qr->insertArg1Result(to_string(*it1));
				}
			}

			return qr;
		}

	}
	return qr; 
}

QueryResult* QueryEvaluator::processFollowsT(Clause* followTClause) {
	string arg1 = followTClause->getArg().at(0);
	string arg2 = followTClause->getArg().at(1);
	string arg1Type = followTClause->getArgType().at(0);
	string arg2Type = followTClause->getArgType().at(1);

	QueryResult* qr = NULL;

	if (arg1Type == ARGTYPE_CONSTANT) {
		// arg1 must be valid stmt
		// if (!PKB.isValidStmt(stoi(arg1))) return qr;

		list<int> arg1FollowerStar;
		// if arg1 has no followerStar, false and return qr
		// arg1FollowerStar = PKB.getFollowerStar();
		// if (arg1FollowerStar.empty()) return qr;

		if (arg2Type == ARGTYPE_CONSTANT) {
			//arg2 must be valid stmt
			// if (!PKB1.isValidStmt(stoi(arg2)))return qr;

			//if arg2 in arg1FollowerStar list, true and return
			if (find(arg1FollowerStar.begin(), arg1FollowerStar.end(), arg2) != arg1FollowerStar.end()) {
				qr->setIsExist(true);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// since arg1FollowerStar is non empty, we can say that query is true
			qr->setIsExist(true);
			return qr;
		}
		else {
			// arg2 is synonym
			//qr->insertSynonym(arg2);
			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) {
				return qr;
			}

			// for each follower in synonymStatementsArg2, check if they belong in arg1FollowerStar
			// if true, add to result and query is true
			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++) {
				if (find(arg1FollowerStar.begin(), arg1FollowerStar.end(), *it) != arg1FollowerStar.end()) {
					qr->insertArg2Result(to_string(*it));
					qr->setIsExist(true);
				}
			}
			return qr;
		}
	}
	else if (arg1Type == ARGTYPE_ANY) {
		if (arg2Type == ARGTYPE_CONSTANT) {
			// arg2 must be valid stmt
			// if (!PKB.isValidStmt(stoi(arg2))) return qr;

			// check if arg2's followedFromStar list got any elements. if no have, query is false
			list<int> arg2FollowedFromStar;
			// arg2FollowedFromStar = PKB.getFollowedFromStar(arg2);
			if (!arg2FollowedFromStar.empty()) {
				qr->setIsExist(true);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// if follow table not empty then is all good
			// if (!PKB.isFollowEmpty()) qr->setIsExist(true);
			return qr;
		}
		else {
			// arg2 is synonym
			//qr->insertSynonym(arg2);
			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) return qr;

			// for each follower in synonymStatementsArg2, check if got followedFromStar list. If have, add all the elements in that list to arg2Results
			for (list<int>::iterator it1 = synonymStatementsArg2.begin(); it1 != synonymStatementsArg2.end(); it1++) {
				list<int> arg2FollowedFromStar;
				// arg2FollowedFromStar = PKB.getFollowedFromStar(*it1);
				if (!arg2FollowedFromStar.empty()) {
					qr->setIsExist(true);
					for (list<int>::iterator it2 = arg2FollowedFromStar.begin(); it2 != arg2FollowedFromStar.end(); it2++) {
						qr->insertArg2Result(to_string(*it2));
					}
				}
			}
			return qr;
		}
	}
	else {
		// arg1 is synonym
		//qr->insertSynonym(arg1);
		qr->setArgToSynonymMapping(PARAM_ARG1, arg1);
		list<int> synonymStatementsArg1 = getList(arg1Type);
		if (synonymStatementsArg1.empty()) {
			return qr;
		}

		if(arg2Type == ARGTYPE_CONSTANT) {
			// arg2 must be valid stmt
			// if (!PKB.isValidStmt(stoi(arg2))) return qr;

			// for each element in synonymStatementsArg1, check if arg1,arg2 is valid follow* rel
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				// if (PKB.isFollowStar(*it, arg2)) qr->setIsExist(true); qr->insertArg1Result(to_string(*it));
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// for all element in synonymStatementsArg1, check if they each have followerStar. If have, add to results
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				list<int> arg1FollowerStar;
				// arg1FollowerStar = PKB.getFollowerStar(*it1);
				if (!arg1FollowerStar.empty()) {
					qr->setIsExist(true);
					for (list<int>::iterator it2 = arg1FollowerStar.begin(); it2 != arg1FollowerStar.end(); it2++) {
						qr->insertArg1Result(to_string(*it2));
					}
				}
			}
			return qr;
		}
		else {
			// arg2 is synonym
			// corner f*(s,s)
			if (arg1 == arg2) {
				return qr;
			}

			//qr->insertSynonym(arg2);
			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) {
				return qr;
			}

			// for each element in synonymStatementsArg1, check if each element in synonymStatementsArg2 fulfil isFollowStar(arg1, arg2)
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg2.end(); it1++) {
				bool outerIteratorIsValidResult = false;
				for (list<int>::iterator it2 = synonymStatementsArg2.begin(); it2 != synonymStatementsArg2.end(); it2++) {
					// if (PKB.isFollowStar(*it1, *it2)) outerIteratorIsValidResult = true; qr->insertArg2Result(to_string(*it2));
				}
				if (outerIteratorIsValidResult) {
					qr->setIsExist(true);
					qr->insertArg1Result(to_string(*it1));
				}
			}
			return qr;
		}
	}
	return qr;
}

QueryResult* QueryEvaluator::processParent(Clause* parentClause) {
	string arg1 = parentClause->getArg().at(0);
	string arg2 = parentClause->getArg().at(1);
	string arg1Type = parentClause->getArgType().at(0);
	string arg2Type = parentClause->getArgType().at(1);

	QueryResult* qr = NULL;

	if (arg1Type == ARGTYPE_CONSTANT)
	{
		// arg1 must be valid stmt
		// if (!PKB.isValidStmt(stoi(arg1))) return qr;

		// arg1 must have child
		// if (!PKB.getChildOf(arg1)) return qr;

		if (arg2Type == ARGTYPE_CONSTANT)
		{
			// arg2 must be valid stmt
			// if (!PKB.isValidStmt(stoi(arg2))) return qr;

			// check if valid parent
			// if (PKB.isParentOf(arg1, arg2)) qr->setIsExist(true); 
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY)
		{
			// at this point determined that arg1 has child.
			qr->setIsExist(true);
			return qr;
		}
		else
		{
			// Arg2 is a synonym
			//qr->insertSynonym(arg2);
			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) return qr;

			// Check if any statement in synonymStatementsArg2 is a child of arg1
			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++)
			{
				// if (PKB.isParentOf(arg1, *it)) qr->insertArg2Result(to_string(*it)); qr->setIsExist(true);
			}
			return qr;
		}
	}
	else if (arg1Type == ARGTYPE_ANY)
	{
		if (arg2Type == ARGTYPE_CONSTANT)
		{
			// arg2 must be valid stmt
			// if (!PKB.isValidStmt(stoi(arg2))) return qr;

			//arg2 must have parent
			// if (PKB.getParentOf(arg2)) qr->setIsExist(true)

			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY)
		{
			// If ParentTable is non empty (with relationships), existential query
			// if (!PKB.isParentEmpty()) qr->setIsExist(true);
			return qr;
		}
		else
		{
			// Arg2 is a synonym
			//qr->insertSynonym(arg2);
			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) return qr;

			// Check if any statement in synonymStatementsArg2 got parent. If have, add to result
			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++)
			{
				// int parentOfArg2  = PKB.getParentOf(*it);
				// if (parentOfArg2) qr->insertArg2Result(to_string(*it)); qr->setIsExist(true);
			}
			return qr;
		}
	}
	else 
	{
		// arg1 is a synonym

		// assume validator already check arg1Type == CONTAINER

		//qr->insertSynonym(arg1);
		qr->setArgToSynonymMapping(PARAM_ARG1, arg1);
		list<int> synonymStatementsArg1 = getList(arg1Type);
		if (synonymStatementsArg1.empty()) return qr;

		if (arg2Type == ARGTYPE_CONSTANT)
		{
			// arg2 must be valid stmt
			// if (!PKB.isValidStmt(stoi(arg2))) return qr;

			// arg2 must have parent in synonymStatementsArg1
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++)
			{
				// if (PKB.isParentOf(*it, arg2)) qr->insertArg1Result(to_string(*it)); qr->setIsExist(true); return qr;
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
				// if (childOfArg1) qr->insertArg1Result(to_string(*it)); qr->setIsExist(true);
			}
			return qr;
		}
		else
		{
			// Arg2 is a synonym
			// Probably wrong implementation now... Gotta redesign QueryResult to handle 2 synonym cases
			
			// corner case. p(x,x)
			// if (arg1 == arg2) return qr;
			//qr->insertSynonym(arg2);
			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) return qr;

			// check for all combination of arg1 and arg2 if exist parental relationship
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				bool outerIteratorIsValidResult = false;
				for (list<int>::iterator it2 = synonymStatementsArg2.begin(); it2 != synonymStatementsArg2.end(); it2++) {
					// if (PKB.isParentOf(*it1, *it2)) outerIteratorIsValidResult = true; qr->insertArg2Result(to_string(*it2)); qr->setIsExist(true);
				}
				// if (outerIteratorIsValidResult) qr->insertArg1Result(to_string(arg1));
			}

			return qr;
		}
	}
	return qr;
}

QueryResult* QueryEvaluator::processParentT(Clause* parentTClause) {
	string arg1 = parentTClause->getArg().at(0);
	string arg2 = parentTClause->getArg().at(1);
	string arg1Type = parentTClause->getArgType().at(0);
	string arg2Type = parentTClause->getArgType().at(1);

	QueryResult* qr = NULL;

	if (arg1Type == ARGTYPE_CONSTANT) {
		//arg1 must be valid stmt
		// if (!PKB.isValidStmt(stoi(arg1))) return qr;
		
		list<int> arg1Children;
		// arg1Children = PKB.getChildrenOf(arg1);
		if (arg1Children.empty()) {
			return qr;
		}

		if (arg2Type == ARGTYPE_CONSTANT) {
			// arg2 nmust be valid stmt
			// if (!PKB.isValidStmt(stoi(arg2))) return qr;

			// check if arg2 is in arg1Children. If yes, query is true
			if (find(arg1Children.begin(), arg1Children.end(), arg2) != arg1Children.end()) {
				qr->setIsExist(true);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// established that arg1 has children. hence, query is true
			qr->setIsExist(true);
			return qr;
		}
		else {
			// arg2 is synonym
			//qr->insertSynonym(arg2);
			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) {
				return qr;
			}

			// for each element in synonymStatementsArg2, check if arg2 is in arg1Children. If yes, add to results
			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++) {
				if (find(arg1Children.begin(), arg1Children.end(), *it) != arg1Children.end()) {
					qr->insertArg2Result(to_string(*it));
					qr->setIsExist(true);
				}
			}
			return qr;
		}
	}
	else if (arg1Type == ARGTYPE_ANY) {
		if (arg2Type == ARGTYPE_CONSTANT) {
			// arg2 must be valid stmt
			// if (!PKB.isValidStmt(stoi(arg2))) return qr;

			// check if arg2 has non empty parentStar
			list<int> arg2ParentStar;
			// arg2ParentStar = PKB.getParentStar(arg2);
			if (!arg2ParentStar.empty()) {
				qr->setIsExist(true);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// check if parent table non empty
			// if (!PKB.isParentEmpty()) qr->setIsExist(true);
			return qr;
		}
		else {
			// arg2 is synonym
			//qr->insertSynonym(arg2);
			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) {
				return qr;
			}

			// for each child in synonymStatementsArg2, check the parentStar and add them in results accordingly
			for (list<int>::iterator it1 = synonymStatementsArg2.begin(); it1 != synonymStatementsArg2.end(); it1++) {
				list<int> arg2ParentStar;
				// arg2ParentStar = PKB.getParentStar(*it);
				if (!arg2ParentStar.empty()) {
					qr->setIsExist(true);
					for (list<int>::iterator it2 = arg2ParentStar.begin(); it2 != arg2ParentStar.end(); it2++) {
						qr->insertArg2Result(to_string(*it2));
					}
				}
			}
			return qr;
		}
	}
	else {
		// arg1 is synonym
		qr->setArgToSynonymMapping(PARAM_ARG1, arg1);
		list<int> synonymStatementsArg1 = getList(arg1Type);
		if (synonymStatementsArg1.empty()) {
			return qr;
		}

		if (arg2Type == ARGTYPE_CONSTANT) {
			//arg must be valid stmt
			// if (!PKB.isValidStmt(stoi(arg2))) return qr;

			// for each element in synonymStatementsArg1, check if they are the Parent* of arg2
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				// if (PKB.isParentStar(*it1,arg2)) qr->insertArg1Result(to_string(*it1)); qr->setIsExist(true);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// for each element in synonymStatementsArg1, check if arg1 has children. if have, add results accordingly
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				list<int> arg1Children;
				// arg1Children = PKB.getChildren(arg1);
				if (!arg1Children.empty()) {
					qr->setIsExist(true);
					qr->insertArg1Result(to_string(*it));
				}
			}
			return qr;
		}
		else {
			// arg2 is synonym
			// corner Parent*(s,s)
			if (arg1 == arg2) {
				return qr;
			}

			qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty()) {
				return qr;
			}

			// for each element in synonymStatementsArg1, check if each element in synonymStatementsArg2 fulfil isFollowStar(arg1, arg2)
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg2.end(); it1++) {
				bool outerIteratorIsValidResult = false;
				for (list<int>::iterator it2 = synonymStatementsArg2.begin(); it2 != synonymStatementsArg2.end(); it2++) {
					// if (PKB.isParentStar(*it1, *it2)) outerIteratorIsValidResult = true; qr->insertArg2Result(to_string(*it2));
				}
				if (outerIteratorIsValidResult) {
					qr->setIsExist(true);
					qr->insertArg1Result(to_string(*it1));
				}
			}
			return qr;
		}
	}
	return qr;
}

QueryResult* QueryEvaluator::processModifies(Clause* modifiesClause) {
	string arg1 = modifiesClause->getArg().at(0);
	string arg2 = modifiesClause->getArg().at(1);
	string arg1Type = modifiesClause->getArgType().at(0);
	string arg2Type = modifiesClause->getArgType().at(1);
	
	QueryResult* qr = NULL;

	if (arg2Type == ARGTYPE_STRING) {
		// arg2 must be valid var
		// if (!PKB.isValidVar(arg2)) return qr;

		list<int> arg2ModifiedBy;
		// arg2ModifiedBy = PKB.getModifiedBy(arg2);
		if (arg2ModifiedBy.empty()) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			// if (!PKB.isValidStmt(stoi(arg1))) return qr;

			// check if arg1 in arg2ModifiedBy list
			if (find(arg2ModifiedBy.begin(), arg2ModifiedBy.end(), arg1) != arg2ModifiedBy.end()) {
				qr->setIsExist(true);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			// Actually, an illegal argument for final iteration because it will be ambiguous
			// Legal in iteration1
			// at this point, if arg2ModifiedBy is not empty, this query is true;
			qr->setIsExist(true);
			return qr;
		}
		else {
			// arg1 is a synonym
			qr->setArgToSynonymMapping(PARAM_ARG1, arg1);
			list<int> synonymStatementsArg1 = getList(arg1Type); //For now, this should only be assign|while
			if (synonymStatementsArg1.empty()) {
				return qr;
			}

			// for each stmt in synonymStatementsArg1, check if they modify arg2 and add results accordingly
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				// if (PKB.isModified(*it, arg2)) qr->setIsExist(true); qr->insertArg1Result(to_string(*it));
			}
			return qr;
		}
	}
	else if (arg2Type == ARGTYPE_VARIABLE) {
		// arg2 is a variable synonym
		qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
		list<string> allVarList;
		// allVarList = PKB.getVarList();
		if (allVarList.empty()) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			// if (!PKB.isValidStmt(stoi(arg1))) return qr;
			
			// check if arg1 modifies anything
			list<string> arg1ModifiedVar;
			// arg1ModifiedVar = PKB.getModifies(arg1)
			if (arg1ModifiedVar.empty()) {
				return qr;
			}

			// query is true since arg1ModifiedVar is non empty
			qr->setIsExist(true);

			// add everything in arg1ModifiedVar to results in arg2
			for (list<string>::iterator it = arg1ModifiedVar.begin(); it != arg1ModifiedVar.end(); it++) {
				qr->insertArg2Result(*it);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			// since allVarList is non-empty, add everything into the reuslts for arg2
			for (list<string>::iterator it = allVarList.begin(); it != allVarList.end(); it++) {
				qr->insertArg2Result(*it);
			}
		}
		else {
			// arg1 is a synonym
			qr->setArgToSynonymMapping(PARAM_ARG1, arg1);
			list<int> synonymStatementsArg1 = getList(arg1Type);
			if (synonymStatementsArg1.empty()) {
				return qr;
			}

			// for each statement in synonymStatementsArg1, check if they modify any of the var
			// add results accordingly
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				bool outerIteratorIsValidResult = false;
				for (list<string>::iterator it2 = allVarList.begin(); it2 != allVarList.end(); it2++) {
					// if (PKB.isModified(*it1, *it2)) outerIteratorIsValidResult = true; qr->insertArg2Result(*it2);
				}
				if (outerIteratorIsValidResult) {
					qr->setIsExist(true);
					qr->insertArg1Result(to_string(*it1));
				}
			}
			return qr;
		}
	}
	else {
		// arg2Type is any '_', explicitly is a variable
		list<string> allVarList;
		// allVarList = PKB.getVarList();
		if (allVarList.empty()) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			// if (!PKB.isValidStmt(arg1)) return qr;

			// check if arg1 modifies anything
			list<string> arg1Modifies;
			// arg1Modifies = PKB.getModifies(arg1);
			if (!arg1Modifies.empty()) {
				qr->setIsExist(true);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			// since varList is non empty, something is modified
			qr->setIsExist(true);
			return qr;
		}
		else {
			// arg1 is a synonym. as of now, this should only be assign type statements
			qr->setArgToSynonymMapping(PARAM_ARG1, arg1);;
			list<int> synonymStatementsArg1 = getList(arg1Type);
			if (synonymStatementsArg1.empty()) {
				return qr;
			}

			// for each statement in synonymStatementsArg1, check if it modifies anything
			// add results accordingly
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				list<string> arg1ModifiesVar;
				// arg1ModifiesVar = PKB.getModifies(*it);
				if (!arg1ModifiesVar.empty()) {
					qr->insertArg1Result(to_string(*it));
					qr->setIsExist(true);
				}
			}
			return qr;
		}
	}
	return qr;
}

QueryResult* QueryEvaluator::processUses(Clause* usesClause) {
	string arg1 = usesClause->getArg().at(0);
	string arg2 = usesClause->getArg().at(1);
	string arg1Type = usesClause->getArgType().at(0);
	string arg2Type = usesClause->getArgType().at(1);

	QueryResult* qr = NULL;

	if (arg2Type == ARGTYPE_STRING) {
		// arg2 must be valid var
		// if (!PKB.isValidVar(arg2)) return qr;

		// check if arg2 is being used by anything
		list<int> arg2UsedBy;
		// arg2UsedBy = PKB.getUsedBy(arg2);
		if (arg2UsedBy.empty()) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			// if (!PKB.isValidStmt(arg1)) return qr;

			// check if arg1 is in arg2UsedBy
			if (find(arg2UsedBy.begin(), arg2UsedBy.end(), arg1) != arg2UsedBy.end()) {
				qr->setIsExist(true);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			// since arg2UsedBy is non empty, this query is true
			qr->setIsExist(true);
			return qr;
		}
		else {
			// arg1 is a synonym. at this point in time, should just be assign type statements
			qr->setArgToSynonymMapping(PARAM_ARG1, arg1);
			list<int> synonymStatementsArg1 = getList(arg1Type);
			if (synonymStatementsArg1.empty()) {
				return qr;
			}

			// for each statement in synonymStatementsArg1, if any statement is using arg2
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				// if (PKB.isUsed(*it, arg2)) qr->setIsExist(true); qr->insertArg1Result(to_string(*it));
			}
			return qr;
		}
	}
	else if (arg2Type == ARGTYPE_VARIABLE) {
		// arg2 is a variable synonym
		qr->setArgToSynonymMapping(PARAM_ARG2, arg2);
		list<string> allVarList;
		// allVarList = PKB.getVarlist();
		if (allVarList.empty()) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			// if (PKB.isValidStmt(arg1)) return qr;
			
			// check if arg1 uses anything
			list<string> arg1UsesVar;
			// arg1UsesVar = PKB.getUses(arg1);
			if (arg1UsesVar.empty()) {
				return qr;
			}

			// if arg1UsesVar non empty, add everything in arg1UsesVar to results
			qr->setIsExist(true);
			for (list<string>::iterator it = arg1UsesVar.begin(); it != arg1UsesVar.end(); it++) {
				qr->insertArg2Result(*it);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			// REQUEST API
			list<string> allUsedVar;
			// allUsedVar = PKB.getAllUsedVar();
			if (allUsedVar.empty()) {
				return qr;
			}
			// add all used var into results
			qr->setIsExist(true);
			for (list<string>::iterator it = allUsedVar.begin(); it != allUsedVar.end(); it++) {
				qr->insertArg2Result(*it);
			}
			return qr;
		}
		else {
			// arg1 is a synonym (can be assign or while)
			qr->setArgToSynonymMapping(PARAM_ARG1, arg1);
			list<int> synonymStatementsArg1 = getList(arg1Type);
			if (synonymStatementsArg1.empty()) {
				return qr;
			}

			// for each statement in synonymStatementsArg1, check if they use anything. if yes, add to result
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				bool outerIteratorIsValidResult = false;
				for (list<string>::iterator it2 = allVarList.begin(); it2 != allVarList.end(); it2++) {
					// if (PKB.isUsedBy(*it1, *it2)) outerIteratorIsValidResult = true; qr->insertArg2Result(*it2);
				}
				if (outerIteratorIsValidResult) {
					qr->setIsExist(true);
					qr->insertArg1Result(to_string(*it1));
				}
			}
			return qr;
		}
	}
	else {
		// arg2 is any, '_'
		list<string> allVarList;
		// allVarList = PKB.getVarList();
		if (allVarList.empty()) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			// if (!PKB.isValidStmt(arg1)) return qr;

			// check if arg1 use anything
			list<string> arg1UsesVar;
			// arg1UsesVar = PKB.getUses(arg1);
			if (!arg1UsesVar.empty()) {
				qr->setIsExist(true);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			list<string> allUsedVar;
			// allUsedVar = PKB.getAllUsedVar();
			if (!allUsedVar.empty()) {
				qr->setIsExist(true);
			}
			return qr;
		}
		else {
			// arg1 is a synonym
			qr->setArgToSynonymMapping(PARAM_ARG1, arg1);
			list<int> synonymStatementsArg1 = getList(arg1Type);
			if (synonymStatementsArg1.empty()) {
				return qr;
			}

			// for each statement in synonymStatementsArg1, check if each of them uses something
			// add results accordingly
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				list<string> arg1UsesVar;
				// arg1UsesVar = PKB.getUses(*it);
				if (!arg1UsesVar.empty()) {
					qr->insertArg1Result(to_string(*it));
					qr->setIsExist(true);
				}
			}
			return qr;
		}
	}
	return qr;
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
	else if (argType == ARGTYPE_STMT || argType == ARGTYPE_PROG_LINE) {
		// wantedList = PKB.getStatementList();
	}
	return wantedList;
}