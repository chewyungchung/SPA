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
	"procedure", // added for iteration 2
	"if", // added for iteration 2 
	"call" // added for iteration 2
});

const vector<string> ATTRIBUTE_TYPES({
	"stmt#",
	"varName",
	"constant",
	"procName"
});

class QueryValidator{
public:
	QueryValidator();
	~QueryValidator();
	QueryValidator(string entity);
	QueryTable parse();
	
	QueryToken getToken();
	void match(string token);
	void match(int tokenType);
	void matchDeclaration();
	void matchDeclarationVar(string token); 
	void matchSelect();
	void matchSelectResult();
	void matchClause();
	void matchSuchThat();
	void matchPattern();
	void matchAnd();
	void matchWith();
	void matchWithClause();
	void matchPatternAssign();
	pair<int,string> matchFactor();
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
	void restrainCommonSynonym();

	pair<int,string> matchStmtRef();
	pair<int,string> matchEntRef();
	vector<pair<int, string>> matchRef();

	bool isAttrTypeValid(string attrType);
	bool synTypeAndAttrNameMatches(string synType, string attrName);

	string getWithType(string attrName, string synType);
	string getWithType(int argType);
	
private:
	QueryTable _qt;
	QueryToken _nextToken;
	QueryTokenizer _tokenizer;
	RelationTable _relTable;
	unordered_map<string, string> _synToEntityMap;
	map<string, int> _synToUseCountMap;
	string _queryString;

};