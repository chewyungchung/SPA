#pragma once
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <chrono>
#include <thread>

#include "PKB.h"
#include "QueryValidator.h"
#include "QueryResultProjector.h"
#include "ResultTable.h"

using namespace std;

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
const string REL_AFFECTS = "affects";
const string REL_AFFECTS_STAR = "affects*";
const string REL_PATTERN = "pattern";
const string REL_WITH = "with";
const string REL_SUBSTITUTE = "substitute";

const string ARGTYPE_NUMBER = "number";
const string ARGTYPE_CONSTANT = "constant";
const string ARGTYPE_CONSTANT_VALUE = "value";
const string ARGTYPE_ANY = "any";
const string ARGTYPE_VARIABLE = "variable";
const string ARGTYPE_ASSIGN = "assign";
const string ARGTYPE_WHILE = "while";
const string ARGTYPE_IF = "if";
const string ARGTYPE_STRING = "string";
const string ARGTYPE_PROCEDURE = "procedure";
const string ARGTYPE_STMT = "stmt";
const string ARGTYPE_PROG_LINE = "prog_line";
const string ARGTYPE_CALLS = "call";
const string ARGTYPE_CALLS_NAME = "call_name";
const string ARGTYPE_CALLS_NUMBER = "call_number";
const string ARGTYPE_EXPR = "expr";
const string ARGTYPE_SUB_EXPR = "sub_expr";
const string ARGTYPE_STMTLST = "stmtLst";

const string PARAM_ARG1 = "ARG1";
const string PARAM_ARG2 = "ARG2";
const string PARAM_PATTERN = "PATTERN";

class QueryEvaluator {

public:
	QueryEvaluator();
	QueryEvaluator(QueryTable, PKB);
	list<string> Evaluate();

private:
	vector<ResultTable> connected_groups_results_;
	vector<ResultTable> non_connected_groups_results_;

	QueryTable input_query_;
	PKB pkb_;

	ResultTable ProcessSubstitute(Clause& substitute_clause);
	ResultTable ProcessClause(Clause& input_clause);
	ResultTable ProcessClauseOptimized(Clause& input_clause, ResultTable& intermediate_result);
	ResultTable ProcessSuchThat(Clause& such_that_clause);
	ResultTable ProcessSuchThatOptimized(Clause& such_that_clause, ResultTable& intermediate_result);
	ResultTable ProcessFollows(Clause& follow_clause);
	ResultTable ProcessFollowsT(Clause& follow_star_clause);
	ResultTable ProcessFollowsOptimized(ResultTable& current_result_set, Clause& follow_clause, bool is_star);
	ResultTable ProcessParent(Clause& parent_clause);
	ResultTable ProcessParentT(Clause& parent_star_clause);
	ResultTable ProcessParentOptimized(ResultTable& current_result_set, Clause& parent_clause, bool is_star);
	ResultTable ProcessUses(Clause& uses_clause);
	ResultTable ProcessUsesOptimized(ResultTable& current_result_set, Clause& uses_clause);
	ResultTable ProcessModifies(Clause& modifies_clause);
	ResultTable ProcessModifiesOptimized(ResultTable& current_result_set, Clause& modifies_clause);
	ResultTable ProcessNext(Clause& next_clause);
	ResultTable ProcessNextT(Clause& next_star_clause);
	ResultTable ProcessNextOptimized(ResultTable& current_result_set, Clause& next_clause, bool is_star);
	ResultTable ProcessCalls(Clause& calls_clause);
	ResultTable ProcessCallsStar(Clause& calls_star_clause);
	ResultTable ProcessCallsOptimized(ResultTable& current_result_set, Clause& calls_clause, bool is_star);
	ResultTable ProcessAffects(Clause& affects_clause);
	ResultTable ProcessAffectsStar(Clause& affects_star_clause);
	ResultTable ProcessAffectsOptimized(ResultTable& current_result_set, Clause& affects_clause, bool is_star);
	ResultTable ProcessPattern(Clause& pattern_clause);
	ResultTable ProcessPatternOptimized(ResultTable& intermediate_result, Clause& pattern_clause);
	ResultTable ProcessPatternAssign(Clause& pattern_assign_clause);
	ResultTable ProcessPatternAssignOptimized(ResultTable& current_result_set, Clause& pattern_assign_clause);
	ResultTable ProcessPatternWhile(Clause& pattern_while_clause);
	ResultTable ProcessPatternWhileOptimized(ResultTable& current_result_set, Clause& pattern_while_clause);
	ResultTable ProcessPatternIf(Clause& pattern_if_clause);
	ResultTable ProcessPatternIfOptimized(ResultTable& current_result_set, Clause& pattern_if_clause);
	ResultTable ProcessWith(Clause& with_clause);
	ResultTable ProcessWithOptimized(ResultTable& current_result_set, Clause& with_clause);
	ResultTable ProcessWithName(Clause& with_name_clause);
	ResultTable ProcessWithNameOptimized(ResultTable& current_result_set, Clause& with_name_clause);
	ResultTable ProcessWithNumber(Clause& with_number_clause);
	ResultTable ProcessWithNumberOptimized(ResultTable& current_result_set, Clause& with_number_clause);

	int GetNumOfCommonColumn(vector<string>& table_columns, vector<string>& clause_columns);
	string GetCommonColumn(vector<string>& table_columns, vector<string>& clause_columns);
	bool IsLeftArg(Clause& clause, string& common_column);
	bool IsCompatibleStmtType(string& stmt_type, int stmt_num);
	bool ProcessNoSynGroup();
	bool ProcessConnectedGroupsOptimized();
	bool ProcessNonConnectedGroupsOptimized();
	bool IsBooleanSelected();
	bool IsNumber(string& s);

	list<int> GetList(string arg_type);
};