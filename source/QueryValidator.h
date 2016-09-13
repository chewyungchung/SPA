#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "QueryTable.h"
#include "QueryToken.h"
#include "QueryTokenizer.h"
#include "RelationTable.h"

const vector<string> DESIGN_ENTITIES({
	"stmt",
	"assign",
	"while",
	"variable",
	"constant",
	"prog_line"
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
	void matchPatternAssign();
	pair<int,string> matchFactor();
	void matchRelation();
	void matchFollow();
	void matchFollowStar();
	void matchParent();
	void matchParentStar();
	void matchModifies();
	void matchUses();
	void restrainCommonSynonym();

	pair<int,string> matchStmtRef();
	pair<int,string> matchEntRef();

private:
	QueryTable _qt;
	QueryToken _nextToken;
	QueryTokenizer _tokenizer;
	RelationTable _relTable;
	unordered_map<string, string> _synToEntityMap;
	map<string, int> _synToUseCountMap;
	string _queryString;

};