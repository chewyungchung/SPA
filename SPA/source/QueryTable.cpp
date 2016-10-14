#include "QueryTable.h"

// Default
QueryTable::QueryTable()
{
	_isNull = 1;
}

QueryTable::QueryTable(int isNull) {
	_isNull = isNull;
}

// Overloaded Constructor
QueryTable::QueryTable(Clause selectClause, vector<Clause> suchThatClauses, vector<Clause> withClauses, vector<Clause> patternClauses) {
	_selectClause = selectClause;
	_suchThatClauses = suchThatClauses;
	_withClauses = withClauses;
	_patternClauses = patternClauses;
}

void QueryTable::optimize() {
	initialiseWeightTable();
	replaceWithClauses(); 
	groupClauses();
	sortClausesInGroup();
	sortGroups();
}

void QueryTable::initialiseWeightTable() {
	//_weightTable = WeightTable();
}

void QueryTable::replaceWithClauses() {
	// three possible scenarios
	// 1. withClause: one arg is a synonym while the other is a string or int (constant)
	// 2. withClause: both arg are synonyms e.g. with s.stmtNum = c.value (also an exception)
	// 3. what about weird with clauses that QV accepts? e.g. 2 = 2; meaningless, will evaluate to T/F
	// (exceptions include: call.procName =  v.varName)
	// we will process case 1 & 3 first, case 2 will have exceptions to be taken care of.
	string syn, toBeReplaced;
	bool isReplaced = false; 
	bool terminateFlag = true;
	for (auto i : _withClauses) {
		if (getNumOfSynInClause(i.getArg(), i.getArgType()) == 0) {
			// add into the nonSyn group for T/F evaluation in QE
			_nonSynGroup.push_back(i);
		}
		else if (getNumOfSynInClause(i.getArg(), i.getArgType()) == 1) {
			// check if syn is found in suchThatClause and withClause
			// if yes, replace syn in suchThat clause with constant/integer
			if (isSynonym(i.getArg().at(0))) {
				syn = i.getArg().at(0);
				toBeReplaced = i.getArg().at(1);
			}
			else {
				syn = i.getArg().at(1);
				toBeReplaced = i.getArg().at(0);
			}
			for (auto i2 : _suchThatClauses) {
				if (getNumOfSynInClause(i2.getArg(), i2.getArgType()) != 0 && i2.getArg().at(0) == syn) {
					//common synonym is found, replace it with the constant/string in withClause
					i2.getArg().at(0) = toBeReplaced;
					isReplaced = true;
				}
				else if (getNumOfSynInClause(i2.getArg(), i2.getArgType()) != 0 && i2.getArg().at(1) == syn) {
					i2.getArg().at(1) = toBeReplaced;
					isReplaced = true;
				}
			}
			if (isReplaced) {
				_withClauses.erase(_withClauses.begin());
			}
		}
		// for clauses with two synonyms, we will process cases & replace when the stmtType of 
		// one synonym is "stmt" while the other is one those under the vector of strings <stmtTypes>
		else {
			vector<string> stmtTypes = { "assign", "call", "while", "if" };
			if (i.getArgType().at(0) == "stmt" &&
				find(stmtTypes.begin(), stmtTypes.end(), i.getArgType().at(1)) != stmtTypes.end()) {
				if (i.getRelation() == "withStmt") {
					toBeReplaced = i.getArgType().at(1);
					terminateFlag = false;
				}
			}
			else if (i.getArgType().at(1) == "stmt" &&
				find(stmtTypes.begin(), stmtTypes.end(), i.getArgType().at(0)) != stmtTypes.end()) {
				if (i.getRelation() == "withStmt") {
					toBeReplaced = i.getArgType().at(0);
					terminateFlag = false;
				}
			}
			if (!terminateFlag) {
				for (auto i2 : _suchThatClauses) {
					if (getNumOfSynInClause(i2.getArg(), i2.getArgType()) != 0 && i2.getArg().at(0) == syn) {
						//common synonym is found, replace it with the constant/string in withClause
						i2.getArg().at(0) = toBeReplaced;
						isReplaced = true;
					}
					else if (getNumOfSynInClause(i2.getArg(), i2.getArgType()) != 0 && i2.getArg().at(1) == syn) {
						i2.getArg().at(1) = toBeReplaced;
						isReplaced = true;
					}
				}
				if (isReplaced) {
					_withClauses.erase(_withClauses.begin());
				}
			}
		}
	}
}

int QueryTable::getNumOfSynInClause(vector<string> arg, vector<string> argType) {
	if (argType.at(0) == "string" || argType.at(0) == "constant" || argType.at(0) == "any" &&
		argType.at(1) == "string" || argType.at(1) == "constant" || argType.at(1) == "any") {
		return 0;
	}
	else if (isSynonym(arg.at(0)) && isSynonym(arg.at(1))) {
		return 2;
	}
	else {
		return 1;
	}
}

void QueryTable::groupClauses() {
	populateNonSynClauses();
	populateSynClauses();
	if (_withSynClauses.size() > 0) {
		groupSynClauses();
	}
}

void QueryTable::sortClausesInGroup() {
	// focus on connected & nonConnected groups 
	for (auto i : _nonConnectedGroup) {
		// set order based on weight
		// first set weight based on arguments within clause, then add on the weight of the table
		// sort by total weight
		// but this might affect the order in the connectedGroup such that the subsequent clauses might not have common synonyms
	}
	// vector<vector<Clause>>
	for (auto i : _connectedGroups) {
		for (auto i1 : i) {

		}
	}
}

void QueryTable::sortGroups() {

}

void QueryTable::populateNonSynClauses() {
	//find clauses that does not contain any synonym
	string arg1Type, arg2Type;
	for (auto i : _suchThatClauses) {
		if (getNumOfSynInClause(i.getArg(), i.getArgType()) == 0) {
			_nonSynGroup.push_back(i);
		}
	}
}

void QueryTable::populateSynClauses() {
	//find clauses that contain one or more synonyms
	string arg1Type, arg2Type;
	// remove such that clauses that does not contain any synonym
	for (auto i : _suchThatClauses) {
		for (auto j : _nonSynGroup) {
			if (i.getRelation() != j.getRelation() && i.getArgType() != j.getArgType() && i.getArg() != j.getArg()) {
				_withSynClauses.push_back(i);
			}
		}
	}
	// is it possible for a pattern clause to have no synonym?
	for (auto i : _patternClauses) {
		_withSynClauses.push_back(i);
	}
}

void QueryTable::groupSynClauses() {
	//group clauses into groups with : 
	//(1) connected clauses 
	//(2) non-connected clauses (no common synonym with other clauses)
	//clauses with connected synonyms should be in the same group 
	//clauses with common synonyms are placed in the same group
	bool found = false;
	vector<string> synList;
	vector<Clause> currentGroup;

	for (auto i1 : _withSynClauses) {
		if (isSynonym(i1.getArg().at(0))) {
			synList.push_back(i1.getArg().at(0));
		}
		if (isSynonym(i1.getArg().at(1))) {
			synList.push_back(i1.getArg().at(1));
		}
		currentGroup.push_back(i1);
		_withSynClauses.erase(_withSynClauses.begin());
		for (auto i2 : _withSynClauses) {
			for (int i3 = 0; i3 < i2.getArg().size(); i3++) {
				if (isSynonym(i2.getArg().at(i3))) {
					found = find(synList.begin(), synList.end(), i2.getArg().at(i3)) != synList.end();
					if (found) {
						synList.push_back(i2.getArg().at(i3));
						currentGroup.push_back(i2);
						_withSynClauses.erase(_withSynClauses.begin());
					}
				}
			}
		}
		if (found) {
			_connectedGroups.push_back(currentGroup);
		}
		else {
			for (auto i3 : currentGroup) {
				_nonConnectedGroup.push_back(i3);
			}
		}
		currentGroup.clear();
		synList.clear();
	}
}

bool QueryTable::isSynonym(string key) {
	if (_synEntityMap.count(key) == 0) {
		return true;
	}
	else {
		return false;
	}
}

Clause QueryTable::getSelectClause() {
	return _selectClause;
}

vector<Clause> QueryTable::getSuchThatClauses() {
	return _suchThatClauses;
}

vector<Clause> QueryTable::getWithClauses() {
	return _withClauses;
}

vector<Clause> QueryTable::getPatternClauses() {
	return _patternClauses;
}


string QueryTable::getSynType(string syn) {
	if (_synEntityMap.find(syn) != _synEntityMap.end()) {
		return _synEntityMap.at(syn);
	}

	// Debug
	return "NO HAVE";
}

int QueryTable::isNullQuery() {
	return _isNull;
}

void QueryTable::setSelectClause(Clause selectClause)
{
	_selectClause = selectClause;
	_isNull = -1;
}

void QueryTable::setSynEntityMap(unordered_map<string, string> synEntityMap) {
	_synEntityMap = synEntityMap;
}

void QueryTable::AddWithClause(Clause with_clause)
{
	_withClauses.push_back(with_clause);
}

void QueryTable::AddSuchThatClause(Clause such_that_clause)
{
	_suchThatClauses.push_back(such_that_clause);
}

void QueryTable::AddPatternClause(Clause pattern_clause)
{
	_patternClauses.push_back(pattern_clause);
}

