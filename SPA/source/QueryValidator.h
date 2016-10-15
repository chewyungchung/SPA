#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <unordered_map>

#include "QueryTable.h"
#include "QueryTokenizer.h"
#include "RelationTable.h"
#include "QueryException.h"

using namespace std;
 
const vector<string> DESIGN_ENTITIES({
	"stmt",
	"assign",
	"while",
	"variable",
	"constant",
	"prog_line",
	"procedure",
	"if", 
	"call"
});

class QueryValidator{
public:
	struct Ref {
		string value = "";
		string arg_type = "";
		string with_type = "";
	};

	QueryValidator();
	QueryValidator(string query_string);
	QueryTable Parse();
	
	QueryToken GetToken();
	void Match(string token);
	void Match(int token_type);
	void MatchDeclaration();
	void MatchDeclarationVar(string token); 
	void MatchSelect();
	void MatchResultClause();
	void MatchTupleResult();
	void MatchResultClauseElement(bool is_tuple);
	void MatchClause();
	void MatchSuchThat();
	void MatchPatternClause();
	void MatchPattern();
	void MatchWith();
	void MatchAttrCompare();
	void MatchPatternAssign();
	void MatchPatternWhile();
	void MatchPatternIf();
	void MatchRelation();
	void MatchFollow();
	void MatchFollowStar();
	void MatchParent();
	void MatchParentStar();
	void MatchModifies();
	void MatchUses();
	void MatchCalls();
	void MatchCallsStar();
	void MatchNext();
	void MatchNextStar();

	// Expression
	void MatchExpr();
	void MatchTerm();
	void MatchFactor();

	pair<int,string> MatchStmtRef();
	pair<int,string> MatchEntRef();
	pair<int, string> MatchVarRef();
	QueryValidator::Ref MatchRef();
	bool IsAttributeMatchSynType(string synType, string attrName);
	bool IsRefCompatible(Ref left_ref, Ref right_ref);
	string GetWithTypeByAttrName(string attrName);
	int GetClausePriority(string relation);
	
private:
	QueryTable query_table_;
	QueryToken next_token_;
	QueryTokenizer tokenizer_;
	RelationTable rel_table_;
	unordered_map<string, string> syn_to_entity_map_;
	string query_string_;
	string expression_string_ = "";

	vector<Clause> such_that_clauses_;
	vector<Clause> with_clauses_;
	vector<Clause> pattern_clauses_;
};