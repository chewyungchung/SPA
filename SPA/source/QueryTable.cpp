#include "QueryTable.h"

// Default
QueryTable::QueryTable()
{
	is_null_ = 1;
}

QueryTable::QueryTable(int isNull) {
	is_null_ = isNull;
}

// Overloaded Constructor
QueryTable::QueryTable(Clause selectClause, vector<Clause> suchThatClauses, vector<Clause> withClauses, vector<Clause> patternClauses) {
	select_clause_ = selectClause;
	such_that_clauses_ = suchThatClauses;
	with_clauses_ = withClauses;
	pattern_clauses_ = patternClauses;
}

void QueryTable::Optimize() {
	InitialiseWeightTable();
	ReplaceWithClauses(); 
	GroupClauses();
	SortClausesInGroup();
	SortGroups();
}

void QueryTable::InitialiseWeightTable() {
	//_weightTable = WeightTable();
}

void QueryTable::ReplaceWithClauses() {
	// three possible scenarios
	// 1. withClause: one arg is a synonym while the other is a string or int (constant)
	// 2. withClause: both arg are synonyms e.g. with s.stmtNum = c.value (also an exception)
	// 3. what about weird with clauses that QV accepts? e.g. 2 = 2; meaningless, will evaluate to T/F
	// (exceptions include: call.procName =  v.varName)
	// we will process case 1 & 3 first, case 2 will have exceptions to be taken care of.
	string syn, toBeReplaced;
	bool isReplaced = false; 
	bool terminateFlag = true;
	for (auto i : with_clauses_) {
		if (GetNumOfSynInClause(i.getArg(), i.getArgType()) == 0) {
			// add into the nonSyn group for T/F evaluation in QE
			no_syn_group_.push_back(i);
		}
		else if (GetNumOfSynInClause(i.getArg(), i.getArgType()) == 1) {
			// check if syn is found in suchThatClause and withClause
			// if yes, replace syn in suchThat clause with constant/integer
			if (IsSynonym(i.getArg().at(0))) {
				syn = i.getArg().at(0);
				toBeReplaced = i.getArg().at(1);
			}
			else {
				syn = i.getArg().at(1);
				toBeReplaced = i.getArg().at(0);
			}
			for (auto i2 : such_that_clauses_) {
				if (GetNumOfSynInClause(i2.getArg(), i2.getArgType()) != 0 && i2.getArg().at(0) == syn) {
					//common synonym is found, replace it with the constant/string in withClause
					i2.getArg().at(0) = toBeReplaced;
					isReplaced = true;
				}
				else if (GetNumOfSynInClause(i2.getArg(), i2.getArgType()) != 0 && i2.getArg().at(1) == syn) {
					i2.getArg().at(1) = toBeReplaced;
					isReplaced = true;
				}
			}
			if (isReplaced) {
				with_clauses_.erase(with_clauses_.begin());
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
				for (auto i2 : such_that_clauses_) {
					if (GetNumOfSynInClause(i2.getArg(), i2.getArgType()) != 0 && i2.getArg().at(0) == syn) {
						//common synonym is found, replace it with the constant/string in withClause
						i2.getArg().at(0) = toBeReplaced;
						isReplaced = true;
					}
					else if (GetNumOfSynInClause(i2.getArg(), i2.getArgType()) != 0 && i2.getArg().at(1) == syn) {
						i2.getArg().at(1) = toBeReplaced;
						isReplaced = true;
					}
				}
				if (isReplaced) {
					with_clauses_.erase(with_clauses_.begin());
				}
			}
		}
	}
}

int QueryTable::GetNumOfSynInClause(vector<string> arg, vector<string> argType) {
	if (argType.at(0) == "string" || argType.at(0) == "constant" || argType.at(0) == "any" &&
		argType.at(1) == "string" || argType.at(1) == "constant" || argType.at(1) == "any") {
		return 0;
	}
	else if (IsSynonym(arg.at(0)) && IsSynonym(arg.at(1))) {
		return 2;
	}
	else {
		return 1;
	}
}

void QueryTable::GroupClauses() {
	PopulateNonSynClauses();
	PopulateSynClauses();
	if (with_syn_clauses_.size() > 0) {
		GroupSynClauses();
	}
}

void QueryTable::SortClausesInGroup() {
	// focus on connected & nonConnected groups 
	for (auto i : non_connected_group_) {
		// set order based on weight
		// first set weight based on arguments within clause, then add on the weight of the table
		// sort by total weight
		// but this might affect the order in the connectedGroup such that the subsequent clauses might not have common synonyms
	}
	// vector<vector<Clause>>
	for (auto i : connected_groups_) {
		for (auto i1 : i) {

		}
	}
}

void QueryTable::SortGroups() {

}

void QueryTable::PopulateNonSynClauses() {
	//find clauses that does not contain any synonym
	string arg1Type, arg2Type;
	for (auto i : such_that_clauses_) {
		if (GetNumOfSynInClause(i.getArg(), i.getArgType()) == 0) {
			no_syn_group_.push_back(i);
		}
	}
}

void QueryTable::PopulateSynClauses() {
	//find clauses that contain one or more synonyms
	string arg1Type, arg2Type;
	// remove such that clauses that does not contain any synonym
	for (auto i : such_that_clauses_) {
		for (auto j : no_syn_group_) {
			if (i.getRelation() != j.getRelation() && i.getArgType() != j.getArgType() && i.getArg() != j.getArg()) {
				with_syn_clauses_.push_back(i);
			}
		}
	}
	// is it possible for a pattern clause to have no synonym?
	for (auto i : pattern_clauses_) {
		with_syn_clauses_.push_back(i);
	}
}

void QueryTable::GroupSynClauses() {
	//group clauses into groups with : 
	//(1) connected clauses 
	//(2) non-connected clauses (no common synonym with other clauses)
	//clauses with connected synonyms should be in the same group 
	//clauses with common synonyms are placed in the same group
	bool found = false;
	vector<string> synList;
	vector<Clause> currentGroup;

	for (auto i1 : with_syn_clauses_) {
		if (IsSynonym(i1.getArg().at(0))) {
			synList.push_back(i1.getArg().at(0));
		}
		if (IsSynonym(i1.getArg().at(1))) {
			synList.push_back(i1.getArg().at(1));
		}
		currentGroup.push_back(i1);
		with_syn_clauses_.erase(with_syn_clauses_.begin());
		for (auto i2 : with_syn_clauses_) {
			for (int i3 = 0; i3 < i2.getArg().size(); i3++) {
				if (IsSynonym(i2.getArg().at(i3))) {
					found = find(synList.begin(), synList.end(), i2.getArg().at(i3)) != synList.end();
					if (found) {
						synList.push_back(i2.getArg().at(i3));
						currentGroup.push_back(i2);
						with_syn_clauses_.erase(with_syn_clauses_.begin());
					}
				}
			}
		}
		if (found) {
			connected_groups_.push_back(currentGroup);
		}
		else {
			for (auto i3 : currentGroup) {
				non_connected_group_.push_back(i3);
			}
		}
		currentGroup.clear();
		synList.clear();
	}
}

bool QueryTable::IsSynonym(string key) {
	if (syn_entity_map_.count(key) == 0) {
		return true;
	}
	else {
		return false;
	}
}

Clause QueryTable::GetSelectClause() {
	return select_clause_;
}

vector<Clause> QueryTable::GetSuchThatClauses() {
	return such_that_clauses_;
}

vector<Clause> QueryTable::GetWithClauses() {
	return with_clauses_;
}

vector<Clause> QueryTable::GetPatternClauses() {
	return pattern_clauses_;
}

vector<Clause> QueryTable::GetNoSynGroup()
{
	return vector<Clause>();
}

vector<Clause> QueryTable::GetNonConnectedGroup()
{
	return vector<Clause>();
}

vector<Clause> QueryTable::GetConnectedGroup()
{
	return vector<Clause>();
}


string QueryTable::GetSynType(string syn) {
	if (syn_entity_map_.find(syn) != syn_entity_map_.end()) {
		return syn_entity_map_.at(syn);
	}

	// Debug
	return "NO HAVE";
}

int QueryTable::IsNullQuery() {
	return is_null_;
}

void QueryTable::SetSelectClause(Clause selectClause)
{
	select_clause_ = selectClause;
	is_null_ = -1;
}

void QueryTable::SetSynEntityMap(unordered_map<string, string> synEntityMap) {
	syn_entity_map_ = synEntityMap;
}

void QueryTable::AddWithClause(Clause with_clause)
{
	with_clauses_.push_back(with_clause);
}

void QueryTable::AddSuchThatClause(Clause such_that_clause)
{
	such_that_clauses_.push_back(such_that_clause);
}

void QueryTable::AddPatternClause(Clause pattern_clause)
{
	pattern_clauses_.push_back(pattern_clause);
}

