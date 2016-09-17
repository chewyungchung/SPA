#include "QueryEvaluator.h"

const string REL_FOLLOWS = "follows";
const string REL_FOLLOWS_STAR = "follows*";
const string REL_PARENT = "parent";
const string REL_PARENT_STAR = "parent*";
const string REL_MODIFIES = "modifies";
const string REL_USES = "uses";

const string ARGTYPE_CONSTANT = "constant";
const string ARGTYPE_ANY = "any";
const string ARGTYPE_intEAN = "intean";
const string ARGTYPE_VARIABLE = "variable";
const string ARGTYPE_ASSIGN = "assign";
const string ARGTYPE_WHILE = "while";
const string ARGTYPE_STRING = "string";
const string ARGTYPE_STMT = "stmt";
const string ARGTYPE_PROG_LINE = "prog_line";

const string PARAM_ARG1 = "ARG1";
const string PARAM_ARG2 = "ARG2";
const string PARAM_PATTERN = "PATTERN";
const string PARAM_intEAN = "intEAN";

QueryEvaluator::QueryEvaluator() {
}

QueryEvaluator::~QueryEvaluator() {
}

QueryEvaluator::QueryEvaluator(QueryValidator qv, PKB pkb, list<string>* results) {
	_pkb = pkb;
	_qt = qv.parse();
	QueryResultProjector qrp = evaluate();
	results = &qrp.getResults();
}

QueryEvaluator::QueryEvaluator(QueryTable qt, PKB pkb) {
	_qt = qt;
	_pkb = pkb;
}

QueryTable QueryEvaluator::evaluate() {
	// Check if the QueryTable given by validator is NULL. Return NULL if 1
	if (_qt.isNullQuery() == 1) {
		return _qt;
	}

	// Process all such that and pattern clause, and finally select clause
	if (_qt.getSuchThatClause().isClauseNull() == -1) {
		_qt.setSuchThatResult(processSuchThat(_qt.getSuchThatClause()));
	}
	if (_qt.getPatternClause().isClauseNull() == -1) {
		_qt.setPatternResult(processPattern(_qt.getPatternClause()));
	}
	if (_qt.getSelectClause().isClauseNull() == -1) {
		_qt.setSelectResult(processSelect(_qt.getSelectClause()));
	}
	return _qt;
}

QueryResult QueryEvaluator::processSelect(Clause selectClause) {
	// Results that output stmt#: 
	// -> stmt | assign | while | prog_line
	// Other results:
	// -> variable | intean | constant

	string expectedResultSynonym = selectClause.getArg().at(0);
	string expectedResultType = selectClause.getArgType().at(0);
	QueryResult qr;

	qr.setArgToSynonymMapping(PARAM_ARG1, expectedResultSynonym);

	if (expectedResultType == ARGTYPE_VARIABLE) {
		list<string> varList = _pkb.getVarList();
		if (varList.empty() == true) {
			return qr;
		}
		qr.setArgToSynonymMapping(PARAM_ARG1, expectedResultSynonym);
		qr.setIsExist(1);
		for (list<string>::iterator it = varList.begin(); it != varList.end(); it++) {
			qr.insertArg1Result(*it);
		}
	}
	else if (expectedResultType == ARGTYPE_intEAN) {
		qr.setArgToSynonymMapping(PARAM_ARG1, PARAM_intEAN);
		qr.setIsExist(1);
		return qr;
	}
	else if (expectedResultType == ARGTYPE_CONSTANT) {
		list<int> constantList = _pkb.getConstantList();
		if (constantList.empty() == true) {
			return qr;
		}
		qr.setArgToSynonymMapping(PARAM_ARG1, expectedResultSynonym);
		qr.setIsExist(1);
		for (list<int>::iterator it = constantList.begin(); it != constantList.end(); it++) {
			qr.insertArg1Result(to_string(*it));
		}
		return qr;
	}
	else {
		// Results should be returning stmt#
		if (expectedResultType == ARGTYPE_ASSIGN || expectedResultType == ARGTYPE_WHILE) {
			list<int> expectedResultStatements = getList(expectedResultType);
			if (expectedResultStatements.empty() == true) {
				return qr;
			}
			qr.setArgToSynonymMapping(PARAM_ARG1, expectedResultSynonym);
			qr.setIsExist(1);
			for (list<int>::iterator it = expectedResultStatements.begin(); it != expectedResultStatements.end(); it++) {
				qr.insertArg1Result(to_string(*it));
			}
			return qr;
		} 
		else {
			// expectedResultType should be either stmt OR progline
			int statementCount = _pkb.getStatementCount(); // Should always be >= 1
			qr.setArgToSynonymMapping(PARAM_ARG1, expectedResultSynonym);
			qr.setIsExist(1);
			for (int i = 1; i <= statementCount; i++) {
				qr.insertArg1Result(to_string(i));
			}
			return qr;
		}
	}
	return qr;
}

QueryResult QueryEvaluator::processSuchThat(Clause suchThatClause) {
	string relation = suchThatClause.getRelation();
	QueryResult suchThatResult;

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

QueryResult QueryEvaluator::processPattern(Clause patternClause) {
	string arg1 = patternClause.getArg().at(0);
	string arg2 = patternClause.getArg().at(1);
	string arg1Type = patternClause.getArgType().at(0);
	string arg2Type = patternClause.getArgType().at(1);
	string synAssign = patternClause.getArg().at(2);
	string synAssignType = ARGTYPE_ASSIGN;
	list<int> synAssignStatements = getList(synAssignType);

	QueryResult qr;

	// Check if there are any assign. If none, can return empty
	if (synAssignStatements.empty() == true) {
		return qr;
	}

	// synAssign is the synonym
	qr.setArgToSynonymMapping(PARAM_PATTERN, synAssign);
	if (arg1Type == ARGTYPE_STRING) {
		// arg1 must be valid varName
		if (_pkb.isValidVar(arg1) == false) {
			return qr;
		}
		if (arg2Type == ARGTYPE_ANY) {
			// arg2 is any '_'
			// Check if arg1 is modified by anything
			list<int> arg1ModifiedBy = _pkb.getModifiedBy(arg1);
			if (arg1ModifiedBy.empty() == true) {
				return qr;
			}
			// If arg1ModifiedBy (made up of assign stmts in iteration1) is non empty, add them all to pattern results
			qr.setIsExist(1);
			for (list<int>::iterator it = arg1ModifiedBy.begin(); it != arg1ModifiedBy.end(); it++) {
				qr.insertPatternResult(to_string(*it));
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_CONSTANT) {
			list<int> arg1ModifiedBy = _pkb.getModifiedBy(arg1);
			list<int> statementsWithArg2 = _pkb.getStmtlineByConstant(stoi(arg2));

			for (list<int>::iterator it1 = arg1ModifiedBy.begin(); it1 != arg1ModifiedBy.end(); ++it1) {
				bool insideAlreadyTrue = false;
				for (list<int>::iterator it2 = statementsWithArg2.begin(); it2 != statementsWithArg2.end(); ++it2) {
					if (*it1 == *it2) {
						qr.setIsExist(1);
						insideAlreadyTrue = true;
						break;
					}
				}
				if (insideAlreadyTrue == true) {
					break;
				}
			}
			return qr;
		}
		else {
			// arg2Type is a string --> For now it can only be a variable
			// Check if arg2 is a valid var
			if (_pkb.isValidVar(arg2) == false) {
				return qr;
			}
			// Get the list of statements that modifies arg1
			list<int> arg1ModifiedBy = _pkb.getModifiedBy(arg1);
			if (arg1ModifiedBy.empty() == true) {
				return qr;
			}

			// For each statement that modifies arg1, check if any of them uses arg2
			for (list<int>::iterator it = arg1ModifiedBy.begin(); it != arg1ModifiedBy.end(); it++) {
				if (_pkb.isUsed(*it, arg2) == true) {
					qr.setIsExist(1);
					qr.insertPatternResult(to_string(*it));
				}
			}
			return qr;
		}
	}
	else if (arg1Type == ARGTYPE_ANY) {
		// arg1 is any '_'
		if (arg2Type == ARGTYPE_ANY) {
			// arg2 is any '_'
			list<int> assignList = _pkb.getAssignList();
			if (assignList.empty() == true) {
				return qr;
			}
			// If assignList not empty, add everything to pattern 
			qr.setIsExist(1);
			for (list<int>::iterator it = assignList.begin(); it != assignList.end(); it++) {
				qr.insertPatternResult(to_string(*it));
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_CONSTANT) {
			// Need to get all the statements with constant
			list<int> stmtWithConstants = _pkb.getStmtlineByConstant(stoi(arg2));
			stmtWithConstants.unique();
			if (stmtWithConstants.empty() == false) {
				qr.setIsExist(1);
				for (list<int>::iterator it = stmtWithConstants.begin(); it != stmtWithConstants.end(); it++) {
					qr.insertPatternResult(to_string(*it));
				}
			}
			return qr;
		}
		else {
			// arg2Type is a string --> For now it can only be a variable
			// Check if arg2 is a valid var
			if (_pkb.isValidVar(arg2) == false) {
				return qr;
			}
			// Get the list of statements that uses arg2
			list<int> arg2UsedBy = _pkb.getUsedBy(arg2);
			if (arg2UsedBy.empty() == true) {
				return qr;
			}
			else {
				for (list<int>::iterator it = arg2UsedBy.begin(); it != arg2UsedBy.end(); it++) {
					qr.setIsExist(1);
					qr.insertPatternResult(to_string(*it));
				}
				return qr;
			}
		}
	}
	else {
		// arg1 is a variable synonym
		list<string> varList = _pkb.getVarList();
		if (varList.empty() == true) {
			return qr;
		}
		qr.setArgToSynonymMapping(PARAM_ARG1, arg1);
		if (arg2Type == ARGTYPE_ANY) {
			// Assume that query is 1 since synAssignStatements not empty as something is modified for sure
			qr.setIsExist(1);
			// For each stmt in synAssignStatements, check each modifies what var
			for (list<int>::iterator it1 = synAssignStatements.begin(); it1 != synAssignStatements.end(); it1++) {
				int outerIteratorIsValidResult = -1;
				list<string> synAssignModifiesVar = _pkb.getModifiedBy(*it1);
				if (!synAssignModifiesVar.empty()) {
					for (list<string>::iterator it2 = synAssignModifiesVar.begin(); it2 != synAssignModifiesVar.end(); it2++) {
						if (_pkb.isModified(*it1, *it2) == true) {
							outerIteratorIsValidResult = 1; 
							qr.insertArg1Result(*it2);
						}
					}
					if (outerIteratorIsValidResult == 1) {
						qr.insertPatternResult(to_string(*it1));
					}
				}
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_CONSTANT) {
			list<int> stmtlineByConstantList = _pkb.getStmtlineByConstant(stoi(arg2));
			stmtlineByConstantList.unique();
			if (stmtlineByConstantList.empty() == true) {
				return qr;
			}

			// For each variable, get the list of statements that modifies them. Then check
			for (list<string>::iterator it1 = varList.begin(); it1 != varList.end(); it1++) {
				int firstNestValid = -1;
				list<int> arg1ModifiedBy = _pkb.getModifiedBy(*it1);
				if (!arg1ModifiedBy.empty() == true) {
					// For each statement that modified each variable, check if they are equal to any of the statement that contain the constant arg2
					for (list<int>::iterator it2 = arg1ModifiedBy.begin(); it2 != arg1ModifiedBy.end(); it2++) {
						int secondNestValid = -1;
						for (list<int>::iterator it3 = stmtlineByConstantList.begin(); it3 != stmtlineByConstantList.end(); it3++) {
							if (*it2 == *it3) {
								firstNestValid = 1;
								secondNestValid = 1;
								break;
							}
						}
						if (secondNestValid == 1) {
							qr.insertPatternResult(to_string(*it2));
						}
					}
					if (firstNestValid == 1) {
						qr.insertArg1Result(*it1);
					}
				}
			}
		}
		else {
			// arg2Type is a string --> For now it can only be a variable
			// Check if arg2 is a valid var
			if (_pkb.isValidVar(arg2) == false) {
				return qr;
			}
			
			// For each variable, get the list of statements that modifies them, then check for each of those statement, if they uses arg2
			for (list<string>::iterator it1 = varList.begin(); it1 != varList.end(); it1++) {
				list<int> arg1ModifiedBy = _pkb.getModifiedBy(*it1);
				int outerIteratorIsValidResult = -1;
				if (arg1ModifiedBy.empty() == true) {
					return qr;
				}
				else {
					for (list<int>::iterator it2 = arg1ModifiedBy.begin(); it2 != arg1ModifiedBy.end(); it2++) {
						if (_pkb.isUsed(*it2, arg2) == true) {
							outerIteratorIsValidResult = 1;
							qr.setIsExist(1);
							qr.insertPatternResult(to_string(*it2));
							break;
						}
					}
					if (outerIteratorIsValidResult == 1) {
						qr.insertArg1Result(*it1);
					}
				}
			}
			return qr;
		}
	}
	return qr;
}

QueryResult QueryEvaluator::processFollows(Clause followClause) {
	string arg1 = followClause.getArg().at(0);
	string arg2 = followClause.getArg().at(1);
	string arg1Type = followClause.getArgType().at(0);
	string arg2Type = followClause.getArgType().at(1);

	QueryResult qr;

	if (arg1Type == ARGTYPE_CONSTANT) {
		//If arg1 not valid stmt, return empty QueryResult
		if (_pkb.isValidStmt(stoi(arg1)) == false) {
			return qr;
		}
		// If arg1 no follower, return empty QueryResult
		int arg1Follower = _pkb.getFollower(stoi(arg1));
		if (arg1Follower == -1) {
			return qr;
		}
		if (arg2Type == ARGTYPE_CONSTANT) {
			// If arg2 <= arg1, impossibru
			if (arg2 <= arg1) {
				return qr;
			}

			// If arg2 not valid stmt, return empty QueryResult
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}
			if (_pkb.isValidFollows(stoi(arg1), stoi(arg2)) == true) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// At this point, arg1 determined to have follower from the first getFollower() check for. Hence, return QueryResult set to 1
			qr.setIsExist(1); 
			return qr;
		}
		else {
			// arg2 is a synonym
			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);

			// If program does not have any statements of arg2Type, then this relationship evaluates to -1
			if (synonymStatementsArg2.empty() == true) {
				return qr;
			}

			// Check if any of the statement in synonymStatementsArg2 list is a follower of arg1
			// If found, add to arg2ResultList and set query to 1
			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); ++it) {
				if (_pkb.isValidFollows(stoi(arg1), *it) == true) {
					qr.insertArg2Result(to_string(*it)); 
					qr.setIsExist(1);
				}
			}
			return qr;
		}
	}
	else if (arg1Type == ARGTYPE_ANY) {
		if (arg2Type == ARGTYPE_CONSTANT) {
			// If arg2 not valid stmt, return empty QueryResult
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}
			// If have followed from, set QueryResult to 1 & return
			int arg2FollowedFrom = _pkb.getFollowedFrom(stoi(arg2));
			if (arg2FollowedFrom == -1) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// If follow table not empty, result exists
			if (_pkb.isFollowEmpty() == false) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else {
			// arg2 is a synonym
			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);

			// If program does not have any statements of arg2Type, then this relationship evaluates to -1
			if (synonymStatementsArg2.empty() == true) return qr;
			
			// Check all statements of arg2Type for followedFrom. Add all followedFrom to result list
			for (list<int>::iterator it = synonymStatementsArg2.begin(), end = synonymStatementsArg2.end(); it != end; ++it) {
				int followedFrom = _pkb.getFollowedFrom(*it);
				if (followedFrom != -1) {
					qr.insertArg2Result(to_string(*it));
					qr.setIsExist(1);
				}
			}
			return qr;
		}
	}
	else {
		// arg1 is a synonym
		qr.setArgToSynonymMapping(PARAM_ARG1, arg1);
		list<int> synonymStatementsArg1 = getList(arg1Type);

		// If no statements of arg1type exist, query is -1
		if (synonymStatementsArg1.empty() == true) {
			return qr;
		}

		if (arg2Type == ARGTYPE_CONSTANT) {
			// If arg2 not valid stmt, query is -1
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}

			// Check if any of the statements in synonymStatementsArg1 is followed by arg2. If any of them are 1, add it to the query results
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				if (_pkb.isValidFollows(*it, stoi(arg2)) == true) {
					qr.insertArg1Result(to_string(*it)); 
					qr.setIsExist(1);
				}
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// Look for followers of all statements of arg1Type. For all found, add to qr
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				int arg1Follower = _pkb.getFollower(*it);
				if (arg1Follower) { 
					qr.insertArg1Result(to_string(arg1Follower)); 
					qr.setIsExist(1); 
				}
			}
			return qr;
		}
		else {
			// arg2 is a synonym

			// Corner case. F(s,s)
			if (arg2 == arg1) {
				return qr;
			}
			
			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);

			// If program does not have any statements of arg2Type, then this relationship evaluates to -1
			if (synonymStatementsArg2.empty() == true) {
				return qr;
			}

			// For each statement of arg1Type, check if each statement of arg2Type is its follower
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				int outerIteratorIsValidResult = -1;
				for (list<int>::iterator it2 = synonymStatementsArg2.begin(); it2 != synonymStatementsArg2.end(); it2++) {
					if (_pkb.isValidFollows(*it1, *it2) == true) { 
						outerIteratorIsValidResult = 1; 
						qr.insertArg2Result(to_string(*it2)); 
						qr.setIsExist(1); 
					}
				}
				if (outerIteratorIsValidResult == 1) {
					qr.insertArg1Result(to_string(*it1));
				}
			}
			return qr;
		}
	}
	return qr; 
}

QueryResult QueryEvaluator::processFollowsT(Clause followTClause) {
	string arg1 = followTClause.getArg().at(0);
	string arg2 = followTClause.getArg().at(1);
	string arg1Type = followTClause.getArgType().at(0);
	string arg2Type = followTClause.getArgType().at(1);

	QueryResult qr;

	if (arg1Type == ARGTYPE_CONSTANT) {
		// arg1 must be valid stmt
		if (_pkb.isValidStmt(stoi(arg1)) == false) {
			return qr;
		}
		// If arg1 has no followerStar, -1 and return qr
		list<int> arg1FollowerStar = _pkb.getFollowerStar(stoi(arg1));
		if (arg1FollowerStar.empty() == true) {
			return qr;
		}
		if (arg2Type == ARGTYPE_CONSTANT) {
			// arg2 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}
			// If arg2 in arg1FollowerStar list, 1 and return
			if (find(arg1FollowerStar.begin(), arg1FollowerStar.end(), stoi(arg2)) != arg1FollowerStar.end()) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// Query is 1 since arg1FollowerStar is non empty
			qr.setIsExist(1);
			return qr;
		}
		else {
			// arg2 is synonym
			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty() == true) {
				return qr;
			}

			// For each follower in synonymStatementsArg2, check if they belong in arg1FollowerStar, if 1, add to result and query is 1
			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++) {
				if (find(arg1FollowerStar.begin(), arg1FollowerStar.end(), *it) != arg1FollowerStar.end()) {
					qr.insertArg2Result(to_string(*it));
					qr.setIsExist(1);
				}
			}
			return qr;
		}
	}
	else if (arg1Type == ARGTYPE_ANY) {
		if (arg2Type == ARGTYPE_CONSTANT) {
			// arg2 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}
			// Check if arg2's followedFromStar list got any elements. if no have, query is -1
			list<int> arg2FollowedFromStar = _pkb.getFollowedFromStar(stoi(arg2));
			if (!arg2FollowedFromStar.empty() == true) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// If follow table not empty then is all good
			if (_pkb.isFollowEmpty() == false) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else {
			// arg2 is synonym
			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty() == true) {
				return qr;
			}
			// For each follower in synonymStatementsArg2, check followedFromStar list. Add all the elements in that list to arg2Results if 1
			for (list<int>::iterator it1 = synonymStatementsArg2.begin(); it1 != synonymStatementsArg2.end(); it1++) {
				list<int> arg2FollowedFromStar = _pkb.getFollowedFromStar(*it1);
				if (!arg2FollowedFromStar.empty() == true) {
					qr.setIsExist(1);
					for (list<int>::iterator it2 = arg2FollowedFromStar.begin(); it2 != arg2FollowedFromStar.end(); it2++) {
						qr.insertArg2Result(to_string(*it2));
					}
				}
			}
			return qr;
		}
	}
	else {
		// arg1 is synonym
		qr.setArgToSynonymMapping(PARAM_ARG1, arg1);
		list<int> synonymStatementsArg1 = getList(arg1Type);
		if (synonymStatementsArg1.empty() == true) {
			return qr;
		}

		if(arg2Type == ARGTYPE_CONSTANT) {
			// arg2 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}
			// For each element in synonymStatementsArg1, check if arg1,arg2 is valid follow* rel
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				if (_pkb.isFollowsStar(*it, stoi(arg2))) {
					qr.setIsExist(1);
					qr.insertArg1Result(to_string(*it));
				}
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// For all element in synonymStatementsArg1, check if they each have followerStar. If have, add to results
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				list<int> arg1FollowerStar = _pkb.getFollowerStar(*it1);
				if (!arg1FollowerStar.empty() == true) {
					qr.setIsExist(1);
					for (list<int>::iterator it2 = arg1FollowerStar.begin(); it2 != arg1FollowerStar.end(); it2++) {
						qr.insertArg1Result(to_string(*it2));
					}
				}
			}
			return qr;
		}
		else {
			// arg2 is synonym
			// Corner case F*(s,s)
			if (arg1 == arg2) {
				return qr;
			}

			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty() == true) {
				return qr;
			}

			// For each element in synonymStatementsArg1, check if each element in synonymStatementsArg2 fulfil isFollowStar(arg1, arg2)
			vector<string> arg1ResultList;
			vector<string> arg2ResultList;
			set<string> arg2Result;

			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				int outerIteratorIsValidResult = -1;
				for (list<int>::iterator it2 = synonymStatementsArg2.begin(); it2 != synonymStatementsArg2.end(); it2++) {
					if (_pkb.isFollowsStar(*it1, *it2)) {
						outerIteratorIsValidResult = 1; 
						//qr.insertArg2Result(to_string(*it2)); Legacy
						arg2ResultList.push_back(to_string(*it2));
					}
				}
				if (outerIteratorIsValidResult == 1) {
					qr.setIsExist(1);
					//qr.insertArg1Result(to_string(*it1));
					arg1ResultList.push_back(to_string(*it1));
				}
			}
			
			for (vector<string>::iterator it = arg2ResultList.begin(); it != arg2ResultList.end(); ++it) {
				arg2Result.insert(*it);
			}

			vector<string> formattedArg2Result;
			for (set<string>::iterator it = arg2Result.begin(); it != arg2Result.end(); ++it) {
				formattedArg2Result.push_back(*it);
			}

			qr.setArg1ResultList(arg1ResultList);
			qr.setArg2ResultList(formattedArg2Result);

			return qr;
		}
	}
	return qr;
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
		if (_pkb.isValidStmt(stoi(arg1)) == false) {
			return qr;
		}

		// arg1 must have child
		list<int> arg1Children = _pkb.getChildrenOf(stoi(arg1));
		if (arg1Children.empty() == true) {
			return qr;
		}
		if (arg2Type == ARGTYPE_CONSTANT) {
			// arg2 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}
			// Check if valid parent
			if (_pkb.isParentOf(stoi(arg1), stoi(arg2))) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// Query is 1 since arg1 has children
			qr.setIsExist(1);
			return qr;
		}
		else {
			// Arg2 is a synonym
			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty() == true) {
				return qr;
			}
			// Check if any statement in synonymStatementsArg2 is a child of arg1
			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++) {
				if (_pkb.isParentOf(stoi(arg1), *it)) {
					qr.insertArg2Result(to_string(*it)); qr.setIsExist(1);
				}
			}
			return qr;
		}
	}
	else if (arg1Type == ARGTYPE_ANY) {
		if (arg2Type == ARGTYPE_CONSTANT) {
			// arg2 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}

			// arg2 must have parent
			if (_pkb.getParentOf(stoi(arg2))) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// If ParentTable is non empty (with relationships), existential query
			if (!_pkb.isParentEmpty() == true) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else {
			// Arg2 is a synonym
			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty() == true) {
				return qr;
			}

			// Check if any statement in synonymStatementsArg2 got parent. If have, add to result
			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++) {
				int parentOfArg2  = _pkb.getParentOf(*it);
				if (parentOfArg2) {
					qr.insertArg2Result(to_string(*it)); 
					qr.setIsExist(1);
				}
			}
			return qr;
		}
	}
	else 
	{
		// arg1 is a synonym
		// assume validator already check arg1Type == CONTAINER
		qr.setArgToSynonymMapping(PARAM_ARG1, arg1);
		list<int> synonymStatementsArg1 = getList(arg1Type);
		if (synonymStatementsArg1.empty() == true) {
			return qr;
		}
		if (arg2Type == ARGTYPE_CONSTANT) {
			// arg2 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}
			// arg2 must have parent in synonymStatementsArg1
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				if (_pkb.isParentOf(*it, stoi(arg2))) {
					qr.insertArg1Result(to_string(*it)); 
					qr.setIsExist(1); 
					return qr;
				}
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// if there exists at least 1 parent in synonymStatementsArg1 that has a child, query evaluates to 1
			// for each synonymStatement that has a kid, add to resultList in qr
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++)
			{
				list<int> arg1Children = _pkb.getChildrenOf(stoi(arg1));
				if (!arg1Children.empty() == true) {
					qr.setIsExist(1);
					for (list<int>::iterator it1 = arg1Children.begin(); it1 != arg1Children.end(); it1++) {
						qr.insertArg1Result(to_string(*it1));
					}
				}
			}
			return qr;
		}
		else
		{
			// Arg2 is a synonym
			// Probably wrong implementation now... Gotta redesign QueryResult to handle 2 synonym cases
			
			// Corner case. P(x,x)
			if (arg1 == arg2) {
				return qr;
			}
			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty() == true) {
				return qr;
			}

			// Check for all combination of arg1 and arg2 if exist parental relationship
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				int outerIteratorIsValidResult = -1;
				for (list<int>::iterator it2 = synonymStatementsArg2.begin(); it2 != synonymStatementsArg2.end(); it2++) {
					if (_pkb.isParentOf(*it1, *it2)) {
						outerIteratorIsValidResult = 1; 
						qr.insertArg2Result(to_string(*it2)); 
						qr.setIsExist(1);
					}
				}
				if (outerIteratorIsValidResult == 1) {
					qr.insertArg1Result(arg1);
				}
			}
			return qr;
		}
	}
	return qr;
}

QueryResult QueryEvaluator::processParentT(Clause parentTClause) {
	string arg1 = parentTClause.getArg().at(0);
	string arg2 = parentTClause.getArg().at(1);
	string arg1Type = parentTClause.getArgType().at(0);
	string arg2Type = parentTClause.getArgType().at(1);

	QueryResult qr;

	if (arg1Type == ARGTYPE_CONSTANT) {
		// arg1 must be valid stmt
		if (_pkb.isValidStmt(stoi(arg1)) == false) {
			return qr;
		}
		list<int> arg1Children = _pkb.getChildrenOf(stoi(arg1));
		if (arg1Children.empty() == true) {
			return qr;
		}

		if (arg2Type == ARGTYPE_CONSTANT) {
			// arg2 nmust be valid stmt
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}
			// Check if arg2 is in arg1Children. If yes, query is 1
			if (find(arg1Children.begin(), arg1Children.end(), stoi(arg2)) != arg1Children.end()) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// Query is 1 as arg1 has children
			qr.setIsExist(1);
			return qr;
		}
		else {
			// arg2 is 
			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty() == true) {
				return qr;
			}

			// For each element in synonymStatementsArg2, check if arg2 is in arg1Children. If yes, add to results
			for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++) {
				if (_pkb.isParentStar(stoi(arg1), *it) != false) {
					qr.setIsExist(1);
					qr.insertArg2Result(to_string(*it));
				}
			}
			//// For each element in synonymStatementsArg2, check if arg2 is in arg1Children. If yes, add to results
			//for (list<int>::iterator it = synonymStatementsArg2.begin(); it != synonymStatementsArg2.end(); it++) {
			//	if (find(arg1Children.begin(), arg1Children.end(), *it) != arg1Children.end()) {
			//		qr.insertArg2Result(to_string(*it));
			//		qr.setIsExist(1);
			//	}
			//}
			return qr;
		}
	}
	else if (arg1Type == ARGTYPE_ANY) {
		if (arg2Type == ARGTYPE_CONSTANT) {
			// arg2 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}
			// Check if arg2 has non empty parentStar
			list<int> arg2ParentStar = _pkb.getParentStar(stoi(arg2));
			if (!arg2ParentStar.empty() == true) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// check if parent table non empty
			if (_pkb.isParentEmpty() == false) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else {
			// arg2 is 
			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty() == true) {
				return qr;
			}

			// For each child in synonymStatementsArg2, check the parentStar and add them in results accordingly
			for (list<int>::iterator it1 = synonymStatementsArg2.begin(); it1 != synonymStatementsArg2.end(); it1++) {
				list<int> arg2ParentStar = _pkb.getParentStar(*it1);
				if (arg2ParentStar.empty() == false ) {
					qr.setIsExist(1);
					for (list<int>::iterator it2 = arg2ParentStar.begin(); it2 != arg2ParentStar.end(); it2++) {
						qr.insertArg2Result(to_string(*it2));
					}
				}
			}
			return qr;
		}
	}
	else {
		// arg1 is synonym
		qr.setArgToSynonymMapping(PARAM_ARG1, arg1);
		list<int> synonymStatementsArg1 = getList(arg1Type);
		if (synonymStatementsArg1.empty() == true) {
			return qr;
		}

		if (arg2Type == ARGTYPE_CONSTANT) {
			//arg must be valid stmt
			if (_pkb.isValidStmt(stoi(arg2)) == false) {
				return qr;
			}

			// For each element in synonymStatementsArg1, check if they are the Parent* of arg2
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				if (_pkb.isParentStar(*it1, stoi(arg2))) {
					qr.insertArg1Result(to_string(*it1)); 
					qr.setIsExist(1);
				}
			}
			return qr;
		}
		else if (arg2Type == ARGTYPE_ANY) {
			// For each element in synonymStatementsArg1, check if arg1 has children. if have, add results accordingly
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				list<int> arg1Children = _pkb.getChildrenOf(stoi(arg1));
				if (arg1Children.empty() == false) {
					qr.setIsExist(1);
					qr.insertArg1Result(to_string(*it));
				}
			}
			return qr;
		}
		else {
			// arg2 is synonym

			// Corner case Parent*(s,s)
			if (arg1 == arg2) {
				return qr;
			}

			qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
			list<int> synonymStatementsArg2 = getList(arg2Type);
			if (synonymStatementsArg2.empty() == true) {
				return qr;
			}

			// For each element in synonymStatementsArg1, check if each element in synonymStatementsArg2 fulfil isFollowStar(arg1, arg2)
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				int outerIteratorIsValidResult = -1;
				for (list<int>::iterator it2 = synonymStatementsArg2.begin(); it2 != synonymStatementsArg2.end(); it2++) {
					if (_pkb.isParentStar(*it1, *it2)) {
						outerIteratorIsValidResult = 1; 
						qr.insertArg2Result(to_string(*it2));
					}
				}
				if (outerIteratorIsValidResult == 1) {
					qr.setIsExist(1);
					qr.insertArg1Result(to_string(*it1));
				}
			}
			return qr;
		}
	}
	return qr;
}

QueryResult QueryEvaluator::processModifies(Clause modifiesClause) {
	string arg1 = modifiesClause.getArg().at(0);
	string arg2 = modifiesClause.getArg().at(1);
	string arg1Type = modifiesClause.getArgType().at(0);
	string arg2Type = modifiesClause.getArgType().at(1);
	
	QueryResult qr;

	if (arg2Type == ARGTYPE_STRING) {
		// arg2 must be valid var
		if (_pkb.isValidVar(arg2) == false) {
			return qr;
		}
		list<int> arg2ModifiedBy = _pkb.getModifiedBy(arg2);
		if (arg2ModifiedBy.empty() == true) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg1)) == false) {
				return qr;
			}

			// Check if arg1 in arg2ModifiedBy list
			if (find(arg2ModifiedBy.begin(), arg2ModifiedBy.end(), stoi(arg1)) != arg2ModifiedBy.end()) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			// Actually, an illegal argument for final iteration because it will be ambiguous. Legal in iteration1
			// Query is 1 if arg2ModifiedBy is not emptyQ
			qr.setIsExist(1);
			return qr;
		}
		else {
			// arg1 is a synonym
			qr.setArgToSynonymMapping(PARAM_ARG1, arg1);
			list<int> synonymStatementsArg1 = getList(arg1Type); //For now, this should only be assign|while
			if (synonymStatementsArg1.empty() == true) {
				return qr;
			}

			// For each stmt in synonymStatementsArg1, check if they modify arg2 and add results accordingly
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				if (_pkb.isModified(*it, arg2)) {
					qr.setIsExist(1); 
					qr.insertArg1Result(to_string(*it));
				}
			}
			return qr;
		}
	}
	else if (arg2Type == ARGTYPE_VARIABLE) {
		// arg2 is a variable synonym
		qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
		list<string> allVarList = _pkb.getVarList();
		if (allVarList.empty() == true) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg1)) == false) {
				return qr;
			}
			
			// Check if arg1 modifies anything
			list<string> arg1ModifiedVar = _pkb.getModifiedBy(stoi(arg1));
			if (arg1ModifiedVar.empty() == true) {
				return qr;
			}

			// Query is 1 since arg1ModifiedVar is non empty
			qr.setIsExist(1);

			// Add everything in arg1ModifiedVar to results in arg2
			for (list<string>::iterator it = arg1ModifiedVar.begin(); it != arg1ModifiedVar.end(); it++) {
				qr.insertArg2Result(*it);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			// Since allVarList is non-empty, add everything into the reuslts for arg2
			for (list<string>::iterator it = allVarList.begin(); it != allVarList.end(); it++) {
				qr.insertArg2Result(*it);
				qr.setIsExist(1);
			}
			return qr;
		}
		else {
			// arg1 is a synonym
			qr.setArgToSynonymMapping(PARAM_ARG1, arg1);
			list<int> synonymStatementsArg1 = getList(arg1Type);
			if (synonymStatementsArg1.empty()) {
				return qr;
			}

			// For each statement in synonymStatementsArg1, check if they modify any of the var
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				int outerIteratorIsValidResult = -1;
				for (list<string>::iterator it2 = allVarList.begin(); it2 != allVarList.end(); it2++) {
					if (_pkb.isModified(*it1, *it2)) {
						outerIteratorIsValidResult = 1; 
						qr.insertArg2Result(*it2);
					}
				}
				if (outerIteratorIsValidResult == 1) {
					qr.setIsExist(1);
					qr.insertArg1Result(to_string(*it1));
				}
			}
			return qr;
		}
	}
	else {
		// arg2Type is any '_', explicitly is a variable
		list<string> allVarList = _pkb.getVarList();
		allVarList.unique();
		if (allVarList.empty() == true) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg1)) == false) {
				return qr;
			}

			// Check if arg1 modifies anything
			list<string> arg1Modifies = _pkb.getModifiedBy(stoi(arg1));
			if (arg1Modifies.empty() == false) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			// Since varList is non empty, something is modified
			qr.setIsExist(1);
			return qr;
		}
		else {
			// arg1 is a synonym. as of now, this should only be assign type statements
			qr.setArgToSynonymMapping(PARAM_ARG1, arg1);;
			list<int> synonymStatementsArg1 = getList(arg1Type);
			if (synonymStatementsArg1.empty()) {
				return qr;
			}

			// For each statement in synonymStatementsArg1, check if it modifies anything
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				list<string> arg1ModifiesVar = _pkb.getModifiedBy(*it);
				if (arg1ModifiesVar.empty() == false) {
					qr.insertArg1Result(to_string(*it));
					qr.setIsExist(1);
				}
			}
			return qr;
		}
	}
	return qr;
}

QueryResult QueryEvaluator::processUses(Clause usesClause) {
	string arg1 = usesClause.getArg().at(0);
	string arg2 = usesClause.getArg().at(1);
	string arg1Type = usesClause.getArgType().at(0);
	string arg2Type = usesClause.getArgType().at(1);

	QueryResult qr;

	if (arg2Type == ARGTYPE_STRING) {
		// arg2 must be valid var
		if (_pkb.isValidVar(arg2) == false) {
			return qr;
		}

		// Check if arg2 is being used by anything
		list<int> arg2UsedBy = _pkb.getUsedBy(arg2);
		if (arg2UsedBy.empty() == true) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg1)) == false) {
				return qr;
			}

			// Check if arg1 is in arg2UsedBy
			if (find(arg2UsedBy.begin(), arg2UsedBy.end(), stoi(arg1)) != arg2UsedBy.end()) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			// Since arg2UsedBy is non empty, this query is 1
			qr.setIsExist(1);
			return qr;
		}
		else {
			// arg1 is a synonym. at this point in time, should just be assign type statements
			qr.setArgToSynonymMapping(PARAM_ARG1, arg1);
			list<int> synonymStatementsArg1 = getList(arg1Type);
			if (synonymStatementsArg1.empty() == true) {
				return qr;
			}

			// For each statement in synonymStatementsArg1, if any statement is using arg2
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				if (_pkb.isUsed(*it, arg2)) {
					qr.setIsExist(1); 
					qr.insertArg1Result(to_string(*it));
				}
			}
			return qr;
		}
	}
	else if (arg2Type == ARGTYPE_VARIABLE) {
		// arg2 is a variable synonym
		qr.setArgToSynonymMapping(PARAM_ARG2, arg2);
		list<string> allVarList = _pkb.getVarList();
		allVarList.unique();
		if (allVarList.empty() == true) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg1)) == false) {
				return qr;
			}

			// Check if arg1 uses anything
			list<string> arg1UsesVar = _pkb.getUsedBy(stoi(arg1));
			if (arg1UsesVar.empty() == true) {
				return qr;
			}

			// if arg1UsesVar non empty, add everything in arg1UsesVar to results
			qr.setIsExist(1);
			for (list<string>::iterator it = arg1UsesVar.begin(); it != arg1UsesVar.end(); it++) {
				qr.insertArg2Result(*it);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			list<string> allUsedVar = _pkb.getAllUsedVar();
			if (allUsedVar.empty() == true) {
				return qr;
			}
			// Add all used var into results
			qr.setIsExist(1);
			for (list<string>::iterator it = allUsedVar.begin(); it != allUsedVar.end(); it++) {
				qr.insertArg2Result(*it);
			}
			return qr;
		}
		else {
			// arg1 is a synonym (can be assign or while)
			qr.setArgToSynonymMapping(PARAM_ARG1, arg1);
			list<int> synonymStatementsArg1 = getList(arg1Type);
			if (synonymStatementsArg1.empty() == true) {
				return qr;
			}

			// For each statement in synonymStatementsArg1, check if they use anything. if yes, add to result
			for (list<int>::iterator it1 = synonymStatementsArg1.begin(); it1 != synonymStatementsArg1.end(); it1++) {
				int outerIteratorIsValidResult = -1;
				for (list<string>::iterator it2 = allVarList.begin(); it2 != allVarList.end(); it2++) {
					if (_pkb.isUsed(*it1, *it2)) {
						outerIteratorIsValidResult = 1; 
						qr.insertArg2Result(*it2);
					}
				}
				if (outerIteratorIsValidResult == 1) {
					qr.setIsExist(1);
					qr.insertArg1Result(to_string(*it1));
				}
			}
			return qr;
		}
	}
	else {
		// arg2 is any, '_'
		list<string> allVarList = _pkb.getVarList();
		allVarList.unique();
		if (allVarList.empty() == true) {
			return qr;
		}

		if (arg1Type == ARGTYPE_CONSTANT) {
			// arg1 must be valid stmt
			if (_pkb.isValidStmt(stoi(arg1)) == false) {
				return qr;
			}

			// Check if arg1 use anything
			list<string> arg1UsesVar = _pkb.getUsedBy(stoi(arg1));
			if (arg1UsesVar.empty() == false) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else if (arg1Type == ARGTYPE_ANY) {
			list<string> allUsedVar = _pkb.getAllUsedVar();
			if (allUsedVar.empty() == false) {
				qr.setIsExist(1);
			}
			return qr;
		}
		else {
			// arg1 is a synonym
			qr.setArgToSynonymMapping(PARAM_ARG1, arg1);
			list<int> synonymStatementsArg1 = getList(arg1Type);
			if (synonymStatementsArg1.empty()==true) {
				return qr;
			}

			// For each statement in synonymStatementsArg1, check if each of them uses something
			for (list<int>::iterator it = synonymStatementsArg1.begin(); it != synonymStatementsArg1.end(); it++) {
				list<string> arg1UsesVar = _pkb.getUsedBy(*it);
				if (arg1UsesVar.empty() == false) {
					qr.insertArg1Result(to_string(*it));
					qr.setIsExist(1);
				}
			}
			return qr;
		}
	}
	return qr;
}


int QueryEvaluator::isInList(list<int> inList, int item) {
	if (find(inList.begin(), inList.end(), item) != inList.end()) {
		return 1;
	}
	return -1;
}

int QueryEvaluator::isListEmpty(list<int> inList) {
	if (inList.empty() == true) {
		return -1;
	}

	return 1;
}

list<int> QueryEvaluator::getList(string argType) {
	list<int> wantedList;
	if (argType == ARGTYPE_ASSIGN) {
		wantedList = _pkb.getAssignList();
	}
	else if (argType == ARGTYPE_WHILE) {
		wantedList = _pkb.getWhileList();
	}
	else if (argType == ARGTYPE_STMT || argType == ARGTYPE_PROG_LINE) {
		wantedList = _pkb.getStmtList();
	}
	return wantedList;
}