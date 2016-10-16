#include "QueryTable.h"

// Default
QueryTable::QueryTable()
{
	is_null_ = true;
}

QueryTable::QueryTable(bool is_null) {
	is_null_ = is_null;
}

// Overloaded Constructor
QueryTable::QueryTable(Clause selectClause, vector<Clause> suchThatClauses, vector<Clause> withClauses, vector<Clause> patternClauses) {
	select_clause_ = selectClause;
	such_that_clauses_ = suchThatClauses;
	with_clauses_ = withClauses;
	pattern_clauses_ = patternClauses;
}

void QueryTable::Optimize() {
	CategorizeClauses();
	EvaluateWithConstantClauses();
	GroupClauses();
}

void QueryTable::CategorizeClauses()
{
	if (IsNullQuery() == true) {
		return;
	}

	BuildAllClause();
	PopulateWithConstantClauses();
	PopulateNoSynClauses();
	PopulateOneSynClauses();
	PopulateTwoSynClauses();
}

void QueryTable::PopulateWithConstantClauses()
{
	for (auto &clause : all_clauses_) {
		if (IsConstantWithClause(clause) == true) {
			with_constant_clauses_.push_back(clause);
		}
	}
}

void QueryTable::PopulateNoSynClauses()
{
	for (auto &such_that_clause : such_that_clauses_) {
		if (GetNumOfSynInClause(such_that_clause) == 0) {
			no_syn_group_.push_back(such_that_clause);
		}
	}

	for (auto &pattern_clause : pattern_clauses_) {
		if (GetNumOfSynInClause(pattern_clause) == 0) {
			no_syn_group_.push_back(pattern_clause);
		}
	}
}

void QueryTable::PopulateOneSynClauses()
{
	for (auto &clause : all_clauses_) {
		if (GetNumOfSynInClause(clause) == 1) {
			one_syn_clauses_.push_back(clause);
		}
	}
}

void QueryTable::PopulateTwoSynClauses()
{
	for (auto &clause : all_clauses_) {
		if (GetNumOfSynInClause(clause) == 2) {
			// Corner. Discard with clauses that are e.g. s1.stmt# = s1.stmt#
			if (IsImplicitTrueWithClause(clause) == true) {
				continue;
			}
			two_syn_clauses_.push_back(clause);
		}
	}
}

void QueryTable::PopulateNonConnectedGroup()
{
	priority_queue<Clause, vector<Clause>, ClauseComparator> one_syn_queue;
	priority_queue<Clause, vector<Clause>, ClauseComparator> two_syn_queue;
	
	if (one_syn_clauses_.empty() == false) {
		for (auto &one_syn_clause : one_syn_clauses_) {
			one_syn_queue.push(one_syn_clause);
		}
	}

	if (two_syn_clauses_.empty() == false) {
		for (auto &two_syn_clause : two_syn_clauses_) {
			two_syn_queue.push(two_syn_clause);
		}
	}

	while (one_syn_queue.empty() == false) {
		non_connected_group_.push_back(one_syn_queue.top());
		one_syn_queue.pop();
	}

	while (two_syn_queue.empty() == false) {
		non_connected_group_.push_back(two_syn_queue.top());
		two_syn_queue.pop();
	}
}

void QueryTable::EvaluateWithConstantClauses()
{
	if (is_null_ == true) {
		return;
	}

	if (with_constant_clauses_.empty() == false) {
		for (auto &with_const_clause : with_constant_clauses_) {
			string with_left_arg = with_const_clause.GetArg().at(0);
			string with_right_arg = with_const_clause.GetArg().at(1);
			if (with_left_arg != with_right_arg) {
				is_null_ = true;
				break;
			}
		}
	}
}

// TODO: with clauses with "string" == " string" and number == number
void QueryTable::ReplaceWithClauses() {
	// three possible scenarios
	// 1. what about weird with clauses that QV accepts? e.g. 2 = 2; meaningless, will evaluate to T/F
	// 2. withClause: one arg is a synonym while the other is a string or int (constant)
	// 3. withClause: both arg are synonyms e.g. with s.stmtNum = c.value (also an exception)
	// (exceptions include: call.procName =  v.varName)
	vector<Clause> unoptimized_with_clauses;
	for (auto with_clause : with_clauses_) {
		if (HasSelectedSynonym(with_clause) == true) {
			unoptimized_with_clauses.push_back(with_clause);
			continue;
		}
		string syn = "", toBeReplaced = "", toBeReplacedArgType = "";
		bool isReplaced = false;
		bool terminateFlag = true;
		string with_clause_arg1 = with_clause.GetArg().at(0);
		string with_clause_arg2 = with_clause.GetArg().at(1);
		string with_clause_arg1_type = with_clause.GetArgType().at(0);
		string with_clause_arg2_type = with_clause.GetArgType().at(1);

		if (IsConstantWithClause(with_clause) == true) {
			// add into the list of with clauses with only constants and evaluate them group for T/F evaluation in QE
			with_constant_clauses_.push_back(with_clause);
		}
		else if (GetNumOfSynInClause(with_clause) == 1) {
			// check if syn is found in suchThatClause and withClause
			// if yes, replace syn in suchThat clause with constant/integer
			if (IsSynonym(with_clause_arg1) == true) {
				syn = with_clause_arg1;
				toBeReplaced = with_clause_arg2;
				if (with_clause_arg2_type == "number") {
					toBeReplacedArgType = "constant";
				}
				else {
					toBeReplacedArgType = "string";
				}
			}
			else if (IsSynonym(with_clause_arg2)) {
				syn = with_clause_arg2;
				toBeReplaced = with_clause_arg1;
				if (with_clause_arg1_type == "number") {
					toBeReplacedArgType = "constant";
				}
				else {
					toBeReplacedArgType = "string";
				}
			}

			// Modify by reference so that changes take place
			for (auto &such_that_clause : such_that_clauses_) {
				if (GetNumOfSynInClause(such_that_clause) != 0 && such_that_clause.GetArg().at(0) == syn) {
					//common synonym is found, replace it with the constant/string in withClause
					//such_that_clause.GetArg().at(0) = toBeReplaced;
					such_that_clause.SetArg(0, toBeReplaced);
					such_that_clause.SetArgType(0, toBeReplacedArgType);
					isReplaced = true;
				}
				else if (GetNumOfSynInClause(such_that_clause) != 0 && such_that_clause.GetArg().at(1) == syn) {
					//such_that_clause.GetArg().at(1) = toBeReplaced;
					such_that_clause.SetArg(1, toBeReplaced);
					such_that_clause.SetArgType(1, toBeReplacedArgType);
					isReplaced = true;
				}
			}
			if (isReplaced == false) {
				// Keep the unoptimized for grouping later
				unoptimized_with_clauses.push_back(with_clause);
			}
			//if (isReplaced) {
			//	// Will there be problems? The clause that got erased might not be the one that was replaced
			//	with_clauses_.erase(with_clauses_.begin());
			//}
		}
		// for clauses with two synonyms, we will process cases & replace when the stmtType of
		// one synonym is "stmt" while the other is one those under the vector of strings <stmtTypes>
		else {
			vector<string> stmtTypes = { "assign", "call_number", "while", "if" };

			if (with_clause_arg1_type == "stmt" &&
				find(stmtTypes.begin(), stmtTypes.end(), with_clause_arg2_type) != stmtTypes.end()) {
				syn = with_clause_arg1;
				toBeReplaced = with_clause_arg2;
				toBeReplacedArgType = with_clause_arg2_type;
				terminateFlag = false;
			}
			else if (with_clause_arg2_type == "stmt" &&
				find(stmtTypes.begin(), stmtTypes.end(), with_clause_arg1_type) != stmtTypes.end()) {
				syn = with_clause_arg2;
				toBeReplaced = with_clause_arg1;
				toBeReplacedArgType = with_clause_arg1_type;
				terminateFlag = false;
			}
			else if (with_clause_arg1_type == with_clause_arg2_type) {
				syn = with_clause_arg1;
				toBeReplaced = with_clause_arg2;
				toBeReplacedArgType = with_clause_arg2_type;
				terminateFlag = false;
			}

			if (terminateFlag == false) {
				// Modify by reference so that changes take place
				for (auto &such_that_clause : such_that_clauses_) {
					if (GetNumOfSynInClause(such_that_clause) != 0 && such_that_clause.GetArg().at(0) == syn) {
						//common synonym is found, replace it with the constant/string in withClause
						//such_that_clause.GetArg().at(0) = toBeReplaced;
						such_that_clause.SetArg(0, toBeReplaced);
						such_that_clause.SetArgType(0, toBeReplacedArgType);
						isReplaced = true;
					}
					else if (GetNumOfSynInClause(such_that_clause) != 0 && such_that_clause.GetArg().at(1) == syn) {
						//such_that_clause.GetArg().at(1) = toBeReplaced;
						such_that_clause.SetArg(1, toBeReplaced);
						such_that_clause.SetArgType(1, toBeReplacedArgType);
						isReplaced = true;
					}
				}
			}

			if (isReplaced == false) {
				unoptimized_with_clauses.push_back(with_clause);
			}
		}
	}
	with_clauses_ = unoptimized_with_clauses;
}

int QueryTable::GetNumOfSynInClause(Clause clause) {
	vector<string> clause_arg = clause.GetArg();
	vector<string> clause_arg_type = clause.GetArgType();
	if ((clause_arg_type.at(0) == "string" || clause_arg_type.at(0) == "constant" || clause_arg_type.at(0) == "any" || clause_arg_type.at(0) == "number") &&
		(clause_arg_type.at(1) == "string" || clause_arg_type.at(1) == "constant" || clause_arg_type.at(1) == "any" || clause_arg_type.at(1) == "number")) {
		return 0;
	}
	else if (IsSynonym(clause_arg.at(0)) && IsSynonym(clause_arg.at(1))) {
		return 2;
	}
	else {
		return 1;
	}
}

void QueryTable::GroupClauses() {
	if (IsNullQuery() == true) {
		return;
	}
	GroupConnectedClauses();
	GroupNonConnectedClauses();
}

void QueryTable::GroupConnectedClauses()
{
	queue<string> processing_synonyms;
	unordered_map<string, bool> is_syn_processed_map;

	if (IsSynSelected(select_clause_) == false) {
		PopulateNonConnectedGroup();
		return;
	}

	for (auto &selected_synonym : select_clause_.GetArg()) {
		processing_synonyms.push(selected_synonym);
	}

	priority_queue<Clause, vector<Clause>, ClauseComparator> one_syn_queue;
	priority_queue<Clause, vector<Clause>, ClauseComparator> two_syn_queue;
	vector<Clause> unprocessed_one_syn_clauses;
	vector<Clause> unprocessed_two_syn_clauses;
	vector<Clause> current_group;

	while (processing_synonyms.empty() == false) {
		unprocessed_one_syn_clauses.clear();
		unprocessed_two_syn_clauses.clear();

		string current_processing_syn = processing_synonyms.front();
		if (is_syn_processed_map[current_processing_syn] == true) {
			processing_synonyms.pop();
			continue;
		}

		for (auto &one_syn_clause : one_syn_clauses_) {
			if (IsSynFound(one_syn_clause, current_processing_syn) == true) {
				one_syn_queue.push(one_syn_clause);
			}
			else {
				unprocessed_one_syn_clauses.push_back(one_syn_clause);
			}
		}

		for (auto &two_syn_clause : two_syn_clauses_) {
			if (IsSynFound(two_syn_clause, current_processing_syn)) {
				two_syn_queue.push(two_syn_clause);
				string new_synonym_in_clause = GetSecondSynonym(two_syn_clause, current_processing_syn);
				processing_synonyms.push(new_synonym_in_clause);
			}
			else {
				unprocessed_two_syn_clauses.push_back(two_syn_clause);
			}
		}

		while (one_syn_queue.empty() == false) {
			current_group.push_back(one_syn_queue.top());
			one_syn_queue.pop();
		}

		while (two_syn_queue.empty() == false) {
			current_group.push_back(two_syn_queue.top());
			two_syn_queue.pop();
		}

		one_syn_clauses_ = unprocessed_one_syn_clauses;
		two_syn_clauses_ = unprocessed_two_syn_clauses;
		is_syn_processed_map[current_processing_syn] = true;
		processing_synonyms.pop();
	}

	// All the non-connected clauses
	one_syn_clauses_ = unprocessed_one_syn_clauses;
	two_syn_clauses_ = unprocessed_two_syn_clauses;

	/*if (unprocessed_one_syn_clauses.empty() == false) {
		for (auto &unprocessed_one_syn_clause : unprocessed_one_syn_clauses) {
			non_connected_group_.push_back(unprocessed_one_syn_clause);
		}
	}

	if (unprocessed_two_syn_clauses.empty() == false) {
		for (auto &unprocessed_two_syn_clause : unprocessed_two_syn_clauses) {
			non_connected_group_.push_back(unprocessed_two_syn_clause);
		}
	}*/

	// Support for iteration 2. Need to update for iteration 3.
	connected_groups_.push_back(current_group);
}

void QueryTable::GroupNonConnectedClauses()
{
	// If no non-connected clauses (most of the time have)
	if (one_syn_clauses_.empty() == true && two_syn_clauses_.empty() == true) {
		return;
	}

	// All the non-connected clauses are in one_syn_clauses_ and two_syn_clauses_. Need to get all the synonym referenced.
	unordered_map<string, bool> is_syn_processed_map;
	set<string> referenced_synonym = ExtractSynonymsFromSet();
	queue<string> unconnected_clause_synonyms;
	queue<string> processing_synonyms;

	for (auto &element : referenced_synonym) {
		unconnected_clause_synonyms.push(element);
	}

	priority_queue<Clause, vector<Clause>, ClauseComparator> one_syn_queue;
	priority_queue<Clause, vector<Clause>, ClauseComparator> two_syn_queue;
	vector<Clause> unprocessed_one_syn_clauses;
	vector<Clause> unprocessed_two_syn_clauses;
	vector<Clause> current_non_connected_group;

	while (unconnected_clause_synonyms.empty() == false) {
		string next_syn_to_consider = unconnected_clause_synonyms.front();
		unconnected_clause_synonyms.pop();
		processing_synonyms.push(next_syn_to_consider);
		current_non_connected_group.clear();
		while (processing_synonyms.empty() == false) {
			unprocessed_one_syn_clauses.clear();
			unprocessed_two_syn_clauses.clear();

			string processing_syn = processing_synonyms.front();
			if (is_syn_processed_map[processing_syn] == true) {
				processing_synonyms.pop();
				continue;
			}

			for (auto &one_syn_clause : one_syn_clauses_) {
				if (IsSynFound(one_syn_clause, processing_syn)) {
					one_syn_queue.push(one_syn_clause);
				}
				else {
					unprocessed_one_syn_clauses.push_back(one_syn_clause);
				}
			}

			for (auto &two_syn_clause : two_syn_clauses_) {
				if (IsSynFound(two_syn_clause, processing_syn)) {
					two_syn_queue.push(two_syn_clause);
					string new_synonym_in_clause = GetSecondSynonym(two_syn_clause, processing_syn);
					processing_synonyms.push(new_synonym_in_clause);
				}
				else {
					unprocessed_two_syn_clauses.push_back(two_syn_clause);
				}
			}

			while (one_syn_queue.empty() == false) {
				current_non_connected_group.push_back(one_syn_queue.top());
				one_syn_queue.pop();
			}

			while (two_syn_queue.empty() == false) {
				current_non_connected_group.push_back(two_syn_queue.top());
				two_syn_queue.pop();
			}

			one_syn_clauses_ = unprocessed_one_syn_clauses;
			two_syn_clauses_ = unprocessed_two_syn_clauses;
			is_syn_processed_map[processing_syn] = true;
			processing_synonyms.pop();
		}
		
		if (current_non_connected_group.empty() == false) {
			non_connected_groups_.push_back(current_non_connected_group);
			one_syn_clauses_ = unprocessed_one_syn_clauses;
			two_syn_clauses_ = unprocessed_two_syn_clauses;
		}
	}

}

set<string> QueryTable::ExtractSynonymsFromSet()
{
	set<string> synonym_set;
	for (auto &one_syn_clause : one_syn_clauses_) {
		string one_syn_clause_arg1 = one_syn_clause.GetArg().at(0);
		string one_syn_clause_arg2 = one_syn_clause.GetArg().at(1);
		if (IsSynonym(one_syn_clause_arg1) == true) {
			synonym_set.insert(one_syn_clause_arg1);
		}
		if (IsSynonym(one_syn_clause_arg2) == true) {
			synonym_set.insert(one_syn_clause_arg2);
		}
	}

	for (auto &two_syn_clause : two_syn_clauses_) {
		string two_syn_clause_arg1 = two_syn_clause.GetArg().at(0);
		string two_syn_clause_arg2 = two_syn_clause.GetArg().at(1);
		if (IsSynonym(two_syn_clause_arg1) == true) {
			synonym_set.insert(two_syn_clause_arg1);
		}
		if (IsSynonym(two_syn_clause_arg2) == true) {
			synonym_set.insert(two_syn_clause_arg2);
		}
	}
	return synonym_set;
}

bool QueryTable::IsSynonym(string key) {
	if (syn_entity_map_.count(key) == 0) {
		return false;
	}
	else {
		return true;
	}
}

bool QueryTable::IsSynFound(Clause clause, string synonym) {
	string clause_arg1 = clause.GetArg().at(0);
	string clause_arg2 = clause.GetArg().at(1);
	string clause_arg1_type = clause.GetArgType().at(0);
	string clause_arg2_type = clause.GetArgType().at(1);
	string select_arg_type = syn_entity_map_[synonym];

	if ((clause_arg1 == synonym && clause_arg1_type == select_arg_type)
		|| (clause_arg2 == synonym && clause_arg2_type == select_arg_type)) {
		return true;
	}
	return false;
}

bool QueryTable::IsSynSelected(Clause select_clause)
{
	string select_arg_type = select_clause.GetArgType().at(0);
	if (select_arg_type != "BOOLEAN") {
		return true;
	}
	return false;
}

bool QueryTable::IsImplicitTrueWithClause(Clause clause)
{
	if (clause.GetRelation() == "with") {
		if (clause.GetArg().at(0) == clause.GetArg().at(1)) {
			return true;
		}
	}
	return false;
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

vector<vector<Clause>> QueryTable::GetConnectedGroups()
{
	return connected_groups_;
}

vector<vector<Clause>> QueryTable::GetNonConnectedGroups()
{
	return non_connected_groups_;
}

string QueryTable::GetSynType(string syn) {
	if (syn_entity_map_.find(syn) != syn_entity_map_.end()) {
		return syn_entity_map_.at(syn);
	}

	// Debug
	return "NO HAVE";
}

bool QueryTable::IsNullQuery() {
	return is_null_;
}

void QueryTable::SetSelectClause(Clause selectClause)
{
	select_clause_ = selectClause;
	is_null_ = false;
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

bool QueryTable::IsConstantWithClause(Clause with_clause)
{
	string clause_relation = with_clause.GetRelation();
	string left_ref_arg_type = with_clause.GetArgType().at(0);
	string right_ref_arg_type = with_clause.GetArgType().at(1);
	if (clause_relation == "with"
		&& (left_ref_arg_type == "string" && right_ref_arg_type == "string")
		|| (left_ref_arg_type == "number" && right_ref_arg_type == "number")) {
		return true;
	}
	return false;
}

bool QueryTable::HasSelectedSynonym(Clause clause)
{
	// Consider the case where: Select s1 with "s1"="s1"
	string select_arg, select_arg_type;
	string clause_arg1 = clause.GetArg().at(0);
	string clause_arg2 = clause.GetArg().at(1);
	string clause_arg1_type = clause.GetArgType().at(0);
	string clause_arg2_type = clause.GetArgType().at(1);

	for (unsigned i = 0; i < select_clause_.GetArg().size(); ++i) {
		select_arg = select_clause_.GetArg().at(i);
		select_arg_type = select_clause_.GetArgType().at(i);
		if ((select_arg == clause_arg1 && select_arg_type == clause_arg1_type) ||
			(select_arg == clause_arg2 && select_arg_type == clause_arg2_type)) {
			return true;
		}
	}
	return false;
}

string QueryTable::GetSecondSynonym(Clause clause, string select_syn)
{
	string clause_arg1 = clause.GetArg().at(0);
	string clause_arg2 = clause.GetArg().at(1);

	if (clause_arg1 == select_syn) {
		return clause_arg2;
	}
	else {
		return clause_arg1;
	}
}

void QueryTable::BuildAllClause()
{
	for (auto &with_clause : with_clauses_) {
		all_clauses_.push_back(with_clause);
	}

	for (auto &pattern_clause : pattern_clauses_) {
		all_clauses_.push_back(pattern_clause);
	}

	for (auto &such_that_clause : such_that_clauses_) {
		all_clauses_.push_back(such_that_clause);
	}
}