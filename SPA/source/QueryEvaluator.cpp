#include "QueryEvaluator.h"

const string REL_FOLLOWS = "follows";
const string REL_FOLLOWS_STAR = "follows*";
const string REL_PARENT = "parent";
const string REL_PARENT_STAR = "parent*";
const string REL_MODIFIES = "modifies";
const string REL_USES = "uses";
const string REL_NEXT = "next";
const string REL_NEXT_STAR = "next*";
const string REL_CALLS = "calls";
const string REL_CALLS_STAR = "calls*";
const string REL_PATTERN = "pattern";

const string ARGTYPE_CONSTANT = "constant";
const string ARGTYPE_ANY = "any";
const string ARGTYPE_intEAN = "intean";
const string ARGTYPE_VARIABLE = "variable";
const string ARGTYPE_ASSIGN = "assign";
const string ARGTYPE_WHILE = "while";
const string ARGTYPE_IF = "if";
const string ARGTYPE_STRING = "string";
const string ARGTYPE_PROCEDURE = "procedure";
const string ARGTYPE_STMT = "stmt";
const string ARGTYPE_PROG_LINE = "prog_line";
const string ARGTYPE_CALLS = "calls";
const string ARGTYPE_EXPR = "expr";
const string ARGTYPE_SUB_EXPR = "sub_expr";

const string PARAM_ARG1 = "ARG1";
const string PARAM_ARG2 = "ARG2";
const string PARAM_PATTERN = "PATTERN";
const string PARAM_intEAN = "intEAN";

QueryEvaluator::QueryEvaluator() {
}

QueryEvaluator::~QueryEvaluator() {
}


QueryEvaluator::QueryEvaluator(QueryTable qt, PKB pkb) {
	_qt = qt;
	_pkb = pkb;
}

ResultTable QueryEvaluator::evaluate() {
	// Check if the QueryTable given by validator is NULL. Return NULL if 1
	if (_qt.isNullQuery() == 1) {
		intermediate_result_.SetIsQueryTrue(false);
		return intermediate_result_;
	}

	// Iteration 2
	if (ProcessNoSynGroup() == false) {
		intermediate_result_.SetIsQueryTrue(false);
		return intermediate_result_;
	}

	if (ProcessNonConnectedGroup() == false) {
		intermediate_result_.SetIsQueryTrue(false);
		return intermediate_result_;
	}

	ProcessConnectedGroup();


	// Below obsolete for iteration 1. Upgrade for iteration 2 above
	// Process all such that and pattern clause, and finally select clause
	if (_qt.getSuchThatClause().isClauseNull() == -1) {
		_qt.setSuchThatResult(processSuchThat(_qt.getSuchThatClause()));
	}
	if (_qt.getPatternClause().isClauseNull() == -1) {
		_qt.setPatternResult(ProcessPattern(_qt.getPatternClause()));
	}
	if (_qt.getSelectClause().isClauseNull() == -1) {
		_qt.setSelectResult(processSelect(_qt.getSelectClause()));
	}
	return intermediate_result_;
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
		//suchThatResult = ProcessFollows(suchThatClause);
	}
	else if (relation == REL_FOLLOWS_STAR) {
		//suchThatResult = ProcessFollowsT(suchThatClause);
	}
	else if (relation == REL_PARENT) {
		suchThatResult = ProcessParent(suchThatClause);
	}
	else if (relation == REL_PARENT_STAR) {
		suchThatResult = ProcessParentT(suchThatClause);
	}
	else if (relation == REL_MODIFIES) {
		suchThatResult = ProcessModifies(suchThatClause);
	}
	else if (relation == REL_USES) {
		suchThatResult = ProcessUses(suchThatClause);
	}
	return suchThatResult;
}

ResultTable QueryEvaluator::ProcessPattern(Clause pattern_clause) {
	ResultTable temp_result;
	string pattern_syn_type = pattern_clause.getArgType().at(0);
	if (pattern_syn_type == ARGTYPE_ASSIGN) {
		temp_result = ProcessPatternAssign(pattern_clause);
	}
	else if (pattern_syn_type == ARGTYPE_WHILE) {
		temp_result = ProcessPatternWhile(pattern_clause);
	}
	else if (pattern_syn_type == ARGTYPE_IF) {
		temp_result = ProcessPatternIf(pattern_clause);
	}
	else {
		return temp_result;
	}
}

ResultTable QueryEvaluator::ProcessFollows(Clause follows_clause) {
	string arg1 = follows_clause.getArg().at(0);
	string arg2 = follows_clause.getArg().at(1);
	string arg1_type = follows_clause.getArgType().at(0);
	string arg2_type = follows_clause.getArgType().at(1);
	
	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT) {
		int arg1_stmt_num = stoi(arg1);
		if (_pkb.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}

		if (_pkb.getFollower(arg1_stmt_num) == -1) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);

			if (arg2_stmt_num <= arg1_stmt_num) {
				return temp_result;
			}

			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (_pkb.isValidFollows(arg1_stmt_num, arg2_stmt_num) == true) {
				temp_result.SetIsQueryTrue(true);
			}
			
			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			// At this point, arg1 determined to have follower from the first getFollower() check for. Hence, return QueryResult set to 1
			temp_result.SetIsQueryTrue(true);
			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (_pkb.isValidFollows(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
					break;
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (_pkb.getFollowedFrom(arg2_stmt_num) != -1) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (_pkb.isFollowEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (_pkb.getFollowedFrom(arg2_stmt_num) != -1) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
					break;
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym
		list<int> synonym_stmt_list_arg1 = getList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (_pkb.isValidFollows(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
					break;
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (_pkb.getFollower(arg1_stmt_num) != -1) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			// Corner case: Follows(s,s)
			if (arg2 == arg1) {
				return temp_result;
			}
			
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
					if (_pkb.isValidFollows(arg1_stmt_num, arg2_stmt_num) == true) {
						temp_result.SetIsQueryTrue(true);
						temp_row_data.push_back(to_string(arg1_stmt_num));
						temp_row_data.push_back(to_string(arg2_stmt_num));
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
						break;
					}
				}
			}

			return temp_result;
		}
	}
	return temp_result;
}

ResultTable QueryEvaluator::ProcessFollowsT(Clause follow_star_clause) {
	string arg1 = follow_star_clause.getArg().at(0);
	string arg2 = follow_star_clause.getArg().at(1);
	string arg1_type = follow_star_clause.getArgType().at(0);
	string arg2_type = follow_star_clause.getArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT) {
		int arg1_stmt_num = stoi(arg1);
		if (_pkb.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}

		list<int> arg1_follower_star = _pkb.getFollowerStar(arg1_stmt_num);
		if (arg1_follower_star.empty() == true) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (find(arg1_follower_star.begin(), arg1_follower_star.end(), arg2_stmt_num) != arg1_follower_star.end()) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			temp_result.SetIsQueryTrue(true);
			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (_pkb.isFollowsStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			list<int> arg2_followed_from_star = _pkb.getFollowedFromStar(arg2_stmt_num);
			if (arg2_followed_from_star.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (_pkb.isFollowEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				list<int> arg2_followed_from_star = _pkb.getFollowedFromStar(arg2_stmt_num);
				if (arg2_followed_from_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym
		list<int> synonym_stmt_list_arg1 = getList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if(arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (_pkb.isFollowsStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<int> arg1_follower_star = _pkb.getFollowerStar(arg1_stmt_num);
				if (arg1_follower_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			// Corner case FollowStar*(s,s)
			if (arg1 == arg2) {
				return temp_result;
			}

			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
					if (_pkb.isFollowsStar(arg1_stmt_num, arg2_stmt_num)) {
						temp_result.SetIsQueryTrue(true);
						temp_row_data.push_back(to_string(arg1_stmt_num));
						temp_row_data.push_back(to_string(arg2_stmt_num));
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessParent(Clause parent_clause) {
	string arg1 = parent_clause.getArg().at(0);
	string arg2 = parent_clause.getArg().at(1);
	string arg1_type = parent_clause.getArgType().at(0);
	string arg2_type = parent_clause.getArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT)
	{
		int arg1_stmt_num = stoi(arg1);
		if (_pkb.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}

		list<int> arg1_children = _pkb.getChildrenOf(arg1_stmt_num);
		if (arg1_children.empty() == true) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (_pkb.isParentOf(arg1_stmt_num, arg2_stmt_num)) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			// Query is true since arg1 has children
			temp_result.SetIsQueryTrue(true);
			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (_pkb.isParentOf(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (_pkb.getParentOf(arg2_stmt_num) != -1) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			// If ParentTable is non empty (with relationships), existential query
			if (_pkb.isParentEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (_pkb.getParentOf(arg2_stmt_num) != -1) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else 
	{
		// Argument 1 is a synonym
		list<int> synonym_stmt_list_arg1 = getList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (_pkb.isParentOf(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
					break;
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<int> arg1_children = _pkb.getChildrenOf(arg1_stmt_num);
				if (arg1_children.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else
		{
			// Argument 2 is a synonym			
			// Corner case. Parent(s,s)
			if (arg1 == arg2) {
				return temp_result;
			}

			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
					if (_pkb.isParentOf(arg1_stmt_num, arg2_stmt_num) == true) {
						temp_result.SetIsQueryTrue(true);
						temp_row_data.push_back(to_string(arg1_stmt_num));
						temp_row_data.push_back(to_string(arg2_stmt_num));
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessParentT(Clause parent_star_clause) {
	string arg1 = parent_star_clause.getArg().at(0);
	string arg2 = parent_star_clause.getArg().at(1);
	string arg1_type = parent_star_clause.getArgType().at(0);
	string arg2_type = parent_star_clause.getArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT) {
		int arg1_stmt_num = stoi(arg1);
		if (_pkb.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}

		list<int> arg1_children_star = _pkb.getChildStarOf(arg1_stmt_num);
		if (arg1_children_star.empty() == true) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (find(arg1_children_star.begin(), arg1_children_star.end(), arg2_stmt_num) != arg1_children_star.end()) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			temp_result.SetIsQueryTrue(true);
			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (_pkb.isParentStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			list<int> arg2_parent_star = _pkb.getParentStar(arg2_stmt_num);
			if (arg2_parent_star.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (_pkb.isParentEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				list<int> arg2_parent_star = _pkb.getParentStar(arg2_stmt_num);
				if (arg2_parent_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym
		list<int> synonym_stmt_list_arg1 = getList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (_pkb.isParentStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<int> arg1_children_star = _pkb.getChildStarOf(arg1_stmt_num);
				if (arg1_children_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			// Corner case Parent*(s,s)
			if (arg1 == arg2) {
				return temp_result;
			}

			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
					if (_pkb.isParentStar(arg1_stmt_num, arg2_stmt_num) == true) {
						temp_result.SetIsQueryTrue(true);
						temp_row_data.push_back(to_string(arg1_stmt_num));
						temp_row_data.push_back(to_string(arg2_stmt_num));
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}
	return temp_result;
}

ResultTable QueryEvaluator::ProcessModifies(Clause modifies_clause) {
	string arg1 = modifies_clause.getArg().at(0);
	string arg2 = modifies_clause.getArg().at(1);
	string arg1_type = modifies_clause.getArgType().at(0);
	string arg2_type = modifies_clause.getArgType().at(1);
	
	ResultTable temp_result;

	if (arg2_type == ARGTYPE_STRING) {
		if (_pkb.isValidVar(arg2) == false) {
			return temp_result;
		}
		list<int> arg2_modified_by = _pkb.getModifiedBy(arg2);
		if (arg2_modified_by.empty() == true) {
			return temp_result;
		}

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (_pkb.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}


			if (find(arg2_modified_by.begin(), arg2_modified_by.end(), arg1_stmt_num) != arg2_modified_by.end()) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (_pkb.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			if (_pkb.isModifiedBy(arg1, arg2) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure type
			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;
			list<string> procedure_list = _pkb.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				if (_pkb.isModifiedBy(arg1_procedure, arg2) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}
			
			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synonym_stmt_list_arg1 = getList(arg1_type);
			if (synonym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (_pkb.isModified(arg1_stmt_num, arg2) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg2_type == ARGTYPE_VARIABLE) {
		// Argument 2 is a variable synonym
		list<string> all_variable_list = _pkb.getVarList();
		all_variable_list.unique();
		if (all_variable_list.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg2);
		vector<string> temp_row_data;

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (_pkb.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}
			
			// Check if arg1 modifies anything
			list<string> arg1_modified_variables = _pkb.getModifiedBy(arg1_stmt_num);
			if (arg1_modified_variables.empty() == true) {
				return ;
			}

			// Query is 1 since arg1_modified_variables is non empty
			temp_result.SetIsQueryTrue(true);

			for (auto &arg2_variable : arg1_modified_variables) {
				temp_row_data.push_back(arg2_variable);
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (_pkb.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			list<string> arg1_procedure_modifies_variable = _pkb.getModifiedByProc(arg1);
			if (arg1_procedure_modifies_variable.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &arg2_variable : arg1_procedure_modifies_variable) {
					temp_row_data.push_back(arg2_variable);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure type
			temp_result = ResultTable(arg1, arg2);
			list<string> procedure_list = _pkb.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_procedure_modifies_variable = _pkb.getModifiedByProc(arg1_procedure);
				if (arg1_procedure_modifies_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg2_variable : arg1_procedure_modifies_variable) {
						temp_row_data.push_back(arg1_procedure);
						temp_row_data.push_back(arg2_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synonym_stmt_list_arg1 = getList(arg1_type);
			if (synonym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<string> arg1_modifies_variable = _pkb.getModifiedBy(arg1_stmt_num);
				if (arg1_modifies_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg2_variable : arg1_modifies_variable) {
						temp_row_data.push_back(to_string(arg1_stmt_num));
						temp_row_data.push_back(arg2_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}
	else {
		// arg2_type is any ('_'), explicitly is a variable
		list<string> all_variable_list = _pkb.getVarList();
		all_variable_list.unique();
		if (all_variable_list.empty() == true) {
			return temp_result;
		}

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (_pkb.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}

			list<string> arg1_modifies = _pkb.getModifiedBy(stoi(arg1));
			if (arg1_modifies.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (_pkb.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			list<string> arg1_procedure_uses_variable = _pkb.getModifiedByProc(arg1);
			if (arg1_procedure_uses_variable.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure type
			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;
			list<string> procedure_list = _pkb.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_procedure_modifies_variable = _pkb.getModifiedByProc(arg1_procedure);
				if (arg1_procedure_modifies_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synonym_stmt_list_arg1 = getList(arg1_type);
			if (synonym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<string> arg1_modifies = _pkb.getModifiedBy(arg1_stmt_num);
				if (arg1_modifies.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	return temp_result;
}

ResultTable QueryEvaluator::ProcessNext(Clause next_clause)
{
	string arg1 = next_clause.getArg().at(0);
	string arg2 = next_clause.getArg().at(1);
	string arg1_type = next_clause.getArgType().at(0);
	string arg2_type = next_clause.getArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT) {
		int arg1_stmt_num = stoi(arg1);
		if (_pkb.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (!_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}
			if (_pkb.isNext(arg1_stmt_num, arg2_stmt_num) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			// Check if there is a statement that can be executed after arg1_stmt_num
			if (_pkb.getExecutedAfter(arg1_stmt_num) != -1) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			// Init the table with arg2 synonym
			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			// For each statemet of arg2_type, check if arg1_stmt_num is executed before it
			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (_pkb.isNext(arg1_stmt_num, arg2_stmt_num) != -1) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	} 
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}
			
			if (_pkb.getExecutedBefore(arg2_stmt_num) != -1) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (_pkb.isNextEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (_pkb.getExecutedBefore(arg2_stmt_num) != -1) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym
		list<int>synonym_stmt_list_arg1 = getList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (_pkb.isNext(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (_pkb.getExecutedAfter(arg1_stmt_num) != -1) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			// Corner case: Next(n,n)
			if (arg1 == arg2) {
				return temp_result;
			}

			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
					if (_pkb.isNext(arg1_stmt_num, arg2_stmt_num) != -1) {
						temp_result.SetIsQueryTrue(true);
						temp_row_data.push_back(to_string(arg1_stmt_num));
						temp_row_data.push_back(to_string(arg2_stmt_num));
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessNextT(Clause next_star_clause)
{
	string arg1 = next_star_clause.getArg().at(0);
	string arg2 = next_star_clause.getArg().at(1);
	string arg1_type = next_star_clause.getArgType().at(0);
	string arg2_type = next_star_clause.getArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT) {
		int arg1_stmt_num = stoi(arg1);
		if (_pkb.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (_pkb.isNextStar(arg1_stmt_num, arg2_stmt_num) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			list<int> executed_after_arg1_list = _pkb.getExecutedAfterStar(arg1_stmt_num);
			if (executed_after_arg1_list.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (_pkb.isNextStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			list<int> executed_before_arg2_list = _pkb.getExecutedBeforeStar(arg2_stmt_num);
			if (executed_before_arg2_list.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (_pkb.isNextEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}
			
			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				list<int> executed_before_star_arg2 = _pkb.getExecutedBeforeStar(arg2_stmt_num);
				if (executed_before_star_arg2.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg2_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym
		list<int> synonym_stmt_list_arg1 = getList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (_pkb.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (_pkb.isNextStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<int> executed_after_star_arg1 = _pkb.getExecutedAfterStar(arg1_stmt_num);
				if (executed_after_star_arg1.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = getList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);
			
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
					if (_pkb.isNextStar(arg1_stmt_num, arg2_stmt_num)) {
						temp_result.SetIsQueryTrue(true);
						temp_row_data.push_back(to_string(arg1_stmt_num));
						temp_row_data.push_back(to_string(arg2_stmt_num));
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessCalls(Clause calls_clause)
{
	string arg1 = calls_clause.getArg().at(0);
	string arg2 = calls_clause.getArg().at(1);
	string arg1_type = calls_clause.getArgType().at(0);
	string arg2_type = calls_clause.getArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_STRING) {
		if (_pkb.isProcedureExist(arg1) == false) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_STRING) {
			if (_pkb.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			if (_pkb.isCall(arg1, arg2) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			list<string> arg1_callee = _pkb.getCallee(arg1);
			if (arg1_callee.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type. Should be non-empty
			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;
			
			list<string> arg1_callee_list = _pkb.getCallee(arg1);
			if (arg1_callee_list.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &arg1_callee : arg1_callee_list) {
					temp_row_data.push_back(arg1_callee);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_STRING) {
			if (_pkb.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			list<string> arg2_caller = _pkb.getCaller(arg2);
			if (arg2_caller.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (_pkb.isCallExist() == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type. Should not be empt
			list<string> procedure_list = _pkb.getProcedureList();
			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_procedure : procedure_list) {
				list<string> arg2_caller = _pkb.getCaller(arg2);
				if (arg2_caller.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg2_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym of procedure type. Should not be empty
		list<string> procedure_list = _pkb.getProcedureList();
		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_STRING) {
			if (_pkb.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			list<string> arg2_caller_list = _pkb.getCaller(arg2);
			if (arg2_caller_list.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &arg2_caller : arg2_caller_list) {
					temp_row_data.push_back(arg2_caller);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_callee = _pkb.getCallee(arg1_procedure);
				if (arg1_callee.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type. Should not be empty
			if (procedure_list.size() < 2) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_callee_list = _pkb.getCallee(arg1);
				if (arg1_callee_list.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg1_callee : arg1_callee_list) {
						temp_row_data.push_back(arg1_procedure);
						temp_row_data.push_back(arg1_callee);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}
			
			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessCallsStar(Clause calls_star_clause)
{
	string arg1 = calls_star_clause.getArg().at(0);
	string arg2 = calls_star_clause.getArg().at(1);
	string arg1_type = calls_star_clause.getArgType().at(0);
	string arg2_type = calls_star_clause.getArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_STRING) {
		if (_pkb.isProcedureExist(arg1) == false) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_STRING) {
			if (_pkb.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			if (_pkb.isCallStar(arg1, arg2) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			list<string> arg1_callee_star = _pkb.getCalleeStar(arg1);
			if (arg1_callee_star.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type
			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			list<string> arg1_callee_star_list = _pkb.getCalleeStar(arg1);
			if (arg1_callee_star_list.empty() == false) {
				for (auto &arg1_callee_star : arg1_callee_star_list) {
					temp_row_data.push_back(arg1_callee_star);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_STRING) {
			if (_pkb.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			list<string> arg2_caller_star = _pkb.getCallerStar(arg2);
			if (arg2_caller_star.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (_pkb.isCallExist() == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type
			list<string> procedure_list = _pkb.getProcedureList();
			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_procedure : procedure_list) {
				list<string> arg2_caller_star = _pkb.getCallerStar(arg2);
				if (arg2_caller_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg2_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym of procedure type. Should not be empty
		list<string> procedure_list = _pkb.getProcedureList();
		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_STRING) {
			if (_pkb.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			list<string> arg2_caller_star_list = _pkb.getCallerStar(arg2);
			if (arg2_caller_star_list.empty() == false) {
				for (auto &arg2_caller_star : arg2_caller_star_list) {
					temp_row_data.push_back(arg2_caller_star);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_callee_star = _pkb.getCalleeStar(arg1);
				if (arg1_callee_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type
			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_caller_star_list = _pkb.getCallerStar(arg1);
				if (arg1_caller_star_list.empty() == false) {
					for (auto &arg1_caller_star : arg1_caller_star_list) {
						temp_row_data.push_back(arg1_procedure);
						temp_row_data.push_back(arg1_caller_star);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessPatternAssign(Clause pattern_assign_clause)
{
	string pattern_assign_syn = pattern_assign_clause.getArg().at(0);
	string arg1 = pattern_assign_clause.getArg().at(1);
	string arg2 = pattern_assign_clause.getArg().at(2);
	string arg1_type = pattern_assign_clause.getArgType().at(1);
	string arg2_type = pattern_assign_clause.getArgType().at(2);

	list<int> all_assign_statements = getList(ARGTYPE_ASSIGN);

	ResultTable temp_result;

	if (all_assign_statements.empty() == true) {
		return temp_result;
	}

	temp_result = ResultTable(pattern_assign_syn);
	vector<string> temp_row_data;

	if (arg1_type == ARGTYPE_STRING) {
		if (_pkb.isValidVar(arg1) == false) {
			return temp_result;
		}
		if (arg2_type == ARGTYPE_ANY) {
			list<int> arg1_modified_by = _pkb.getModifiedBy(arg1);
			if (arg1_modified_by.empty() == true) {
				return temp_result;
			}
			// If arg1ModifiedBy (made up of assign stmts in iteration1) is non empty, add them all to pattern results
			temp_result.SetIsQueryTrue(true);

			for (auto &pattern_assign_syn_stmt_num : arg1_modified_by) {
				temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_EXPR) {
			list<int> assign_with_expression = _pkb.getAssignWithExpression(arg2);
			if (assign_with_expression.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &pattern_assign_syn_stmt_num : assign_with_expression) {
					temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_SUB_EXPR) {
			list<int> assign_with_sub_expression = _pkb.getAssignWithSubExpr(arg2);
			if (assign_with_sub_expression.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &pattern_assign_syn_stmt_num : assign_with_sub_expression) {
					temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Should never be here. Consider removing this else.
			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_ANY) {
			list<int> all_assign_list = _pkb.getAssignList();
			if (all_assign_list.empty() == true) {
				return temp_result;
			}

			temp_result.SetIsQueryTrue(true);

			for (auto &pattern_assign_syn_stmt_num : all_assign_list) {
				temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_EXPR) {
			list<int> assign_with_expression = _pkb.getAssignWithExpression(arg2);
			if (assign_with_expression.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &pattern_assign_syn_stmt_num : assign_with_expression) {
					temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_SUB_EXPR) {
			list<int> assign_with_sub_expression = _pkb.getAssignWithSubExpr(arg2);
			if (assign_with_sub_expression.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &pattern_assign_syn_stmt_num : assign_with_sub_expression) {
					temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a variable synonym
		list<string> all_variable_list = _pkb.getVarList();
		if (all_variable_list.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(pattern_assign_syn, arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_ANY) {
			for (auto &pattern_assign_syn_stmt_num : all_assign_statements) {
				list<string> pattern_assign_syn_modifies_variables = _pkb.getModifiedBy(pattern_assign_syn_stmt_num);
				if (pattern_assign_syn_modifies_variables.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg1_variable : pattern_assign_syn_modifies_variables) {
						temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
						temp_row_data.push_back(arg1_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}

				return temp_result;
			}
			return temp_result;
		}
		else if (arg2_type == ARGTYPE_EXPR) {
			list<int> assign_with_expression = _pkb.getAssignWithExpression(arg2);
			if (assign_with_expression.empty() == true) {
				return temp_result;
			}

			for (auto &pattern_assign_syn_stmt_num : assign_with_expression) {
				list<string> pattern_assign_syn_modifies_variables = _pkb.getModifiedBy(pattern_assign_syn_stmt_num);
				if (pattern_assign_syn_modifies_variables.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg1_variable : pattern_assign_syn_modifies_variables) {
						temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
						temp_row_data.push_back(arg1_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_SUB_EXPR) {
			list<int> assign_with_sub_expression = _pkb.getAssignWithSubExpr(arg2);
			if (assign_with_sub_expression.empty() == true) {
				return temp_result;
			}

			for (auto &pattern_assign_syn_stmt_num : assign_with_sub_expression) {
				list<string> pattern_assign_syn_modifies_variables = _pkb.getModifiedBy(pattern_assign_syn_stmt_num);
				if (pattern_assign_syn_modifies_variables.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg1_variable : pattern_assign_syn_modifies_variables) {
						temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
						temp_row_data.push_back(arg1_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessPatternWhile(Clause pattern_while_clause)
{
	return ResultTable();
}

ResultTable QueryEvaluator::ProcessPatternIf(Clause pattern_if_clause)
{
	return ResultTable();
}

bool QueryEvaluator::ProcessNoSynGroup()
{

	return false;
}

bool QueryEvaluator::ProcessNonConnectedGroup()
{
	return false;
}

void QueryEvaluator::ProcessConnectedGroup()
{
}

ResultTable QueryEvaluator::ProcessUses(Clause uses_clause) {
	string arg1 = uses_clause.getArg().at(0);
	string arg2 = uses_clause.getArg().at(1);
	string arg1_type = uses_clause.getArgType().at(0);
	string arg2_type = uses_clause.getArgType().at(1);

	ResultTable temp_result;

	if (arg2_type == ARGTYPE_STRING) {
		if (_pkb.isValidVar(arg2) == false) {
			return temp_result;
		}

		list<int> arg2_used_by = _pkb.getUsedBy(arg2);
		if (arg2_used_by.empty() == true) {
			return temp_result;
		}

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (_pkb.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}

			if (find(arg2_used_by.begin(), arg2_used_by.end(), arg1_stmt_num) != arg2_used_by.end()) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (_pkb.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			if (_pkb.isUsedBy(arg1, arg2) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure. Guaranteed to have at least 1 procedure
			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;
			list<string> procedure_list = _pkb.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				if (_pkb.isUsedBy(arg1_procedure, arg2) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synomym_stmt_list_arg1 = getList(arg1_type);
			if (synomym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;

			for (auto &arg1_stmt_num : synomym_stmt_list_arg1) {
				if (_pkb.isUsed(arg1_stmt_num, arg2) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg2_type == ARGTYPE_VARIABLE) {
		// Argument 2 is a variable synonym
		list<string> all_variable_list = _pkb.getVarList();
		all_variable_list.unique();
		if (all_variable_list.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg2);
		vector<string> temp_row_data;

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (_pkb.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}

			list<string> arg1_uses_variable = _pkb.getUsedBy(arg1_stmt_num);
			if (arg1_uses_variable.empty() == true) {
				return temp_result;
			}

			// If arg1_uses_variable non empty, add everything in arg1UsesVar to results
			temp_result.SetIsQueryTrue(true);
			for (auto &arg2_variable : arg1_uses_variable) {
				temp_row_data.push_back(arg2_variable);
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (_pkb.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			list<string> arg1_procedure_uses_variable = _pkb.getUsedByProc(arg1);
			if (arg1_procedure_uses_variable.empty() == true) {
				return temp_result;
			}

			temp_result.SetIsQueryTrue(true);
			for (auto &arg2_variable : arg1_procedure_uses_variable) {
				temp_row_data.push_back(arg2_variable);
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure type. Guaranteed to have at least 1 procedure
			temp_result = ResultTable(arg1, arg2);
			list<string> procedure_list = _pkb.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_procedure_uses_variable = _pkb.getUsedByProc(arg1_procedure);
				if (arg1_procedure_uses_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg2_variable : arg1_procedure_uses_variable) {
						temp_row_data.push_back(arg1_procedure);
						temp_row_data.push_back(arg2_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synonym_stmt_list_arg1 = getList(arg1_type);
			if (synonym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<string> arg1_uses_variable = _pkb.getUsedBy(arg1_stmt_num);
				if (arg1_uses_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg2_variable : arg1_uses_variable) {
						temp_row_data.push_back(to_string(arg1_stmt_num));
						temp_row_data.push_back(arg2_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 2 is any ('_')
		list<string> all_variable_list = _pkb.getVarList();
;		all_variable_list.unique();
		if (all_variable_list.empty() == true) {
			return temp_result;
		}

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (_pkb.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}

			// Check if arg1 use anything
			list<string> arg1_uses_var = _pkb.getUsedBy(arg1_stmt_num);
			if (arg1_uses_var.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (_pkb.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			list<string> arg1_procedure_uses_variable = _pkb.getUsedByProc(arg1);
			if (arg1_procedure_uses_variable.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure type
			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;
			list<string> procedure_list = _pkb.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_procedure_uses_variable = _pkb.getUsedByProc(arg1_procedure);
				if (arg1_procedure_uses_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synonym_stmt_list_arg1 = getList(arg1_type);
			if (synonym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<string> arg1_uses_variable = _pkb.getUsedBy(arg1_stmt_num);
				if (arg1_uses_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}

	return temp_result;
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
	else if (argType == ARGTYPE_IF) {
		wantedList = _pkb.getIfList();
	} 
	else if (argType == ARGTYPE_CALLS) {
		wantedList = _pkb.getCallList();
	}
	else if (argType == ARGTYPE_STMT || argType == ARGTYPE_PROG_LINE) {
		wantedList = _pkb.getStmtList();
	}
	return wantedList;
}