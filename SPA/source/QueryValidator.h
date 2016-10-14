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

const vector<string> ATTRIBUTE_TYPES({
	"stmt#",
	"varName",
	"value",
	"procName"
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
	QueryTable parse();
	
	QueryToken getToken();
	void match(string token);
	void match(int tokenType);
	void matchDeclaration();
	void matchDeclarationVar(string token); 
	void matchSelect();
	void matchResultClause();
	void matchTupleResult();
	void matchResultClauseElement(bool is_tuple);
	void matchClause();
	void matchSuchThat();
	void matchPatternClause();
	void matchPattern();
	void matchWith();
	void matchAttrCompare();
	void matchPatternAssign();
	void matchPatternWhile();
	void matchPatternIf();
	void matchRelation();
	void matchFollow();
	void matchFollowStar();
	void matchParent();
	void matchParentStar();
	void matchModifies();
	void matchUses();
	void matchCalls();
	void matchCallsStar();
	void matchNext();
	void matchNextStar();

	// Expression
	void matchExpr();
	void matchTerm();
	void matchFactor();


	pair<int,string> matchStmtRef();
	pair<int,string> matchEntRef();
	pair<int, string> matchVarRef();
	QueryValidator::Ref matchRef();

	bool synTypeAndAttrNameMatches(string synType, string attrName);
	bool IsRefCompatible(Ref left_ref, Ref right_ref);
 
	string getWithTypeByAttrName(string attrName);
	
private:
	QueryTable query_table_;
	QueryToken next_token_;
	QueryTokenizer tokenizer_;
	RelationTable rel_table_;
	unordered_map<string, string> syn_to_entity_map_;
	string query_string_;
	string expression_string_ = "";

	vector<Clause> _suchThatClauses;
	vector<Clause> _withClauses;
	vector<Clause> _patternClauses;
};