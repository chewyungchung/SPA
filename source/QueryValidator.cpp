#include "QueryValidator.h"
#include "QueryException.h"

QueryValidator::QueryValidator() {
	_qt = QueryTable(false);
}

QueryValidator::~QueryValidator() {

}

QueryValidator::QueryValidator(string query) {
	_queryString = query;
	_tokenizer = QueryTokenizer(query);
	
}

QueryTable QueryValidator::parse() {
	try {
		matchDeclaration();
		matchSelect();
	}
	catch (QueryException& e) { // Catch exception by reference. Internet recommendation
		// Output error message onto console. Purely debug for now
		cout << e.what() << endl;
		// Return a null pointer to indicate that an invalid query has been sent in
		return QueryTable(true);
	}
	return _qt;
}

QueryToken QueryValidator::getToken() {
	return _tokenizer.tokenize();
}

void QueryValidator::match(string token) {
	if (_nextToken.getTokenName() == token) {
		_nextToken = getToken();
	}
	else {
		throw(QueryException("Invalid query: Unexpected token '" + _nextToken.getTokenName() + "; Expected token '" + token));
	}
}

void QueryValidator::match(int tokenType) {
	if (_nextToken.getTokenType() == tokenType) {
		_nextToken = getToken();
	}
	else {
		throw(QueryException("Invalid query: Unexpected token type'" + to_string(_nextToken.getTokenType()) + "; Expected tokenType '" + to_string(tokenType)));
	}
}
	
void QueryValidator::matchDeclaration() {
	_nextToken = getToken();
	// If the first token is not "Select", that means there are Declarations
	if (_nextToken.getTokenName() != "Select") {
		// Check if the next token are any of the accepted design entities
		// Iteration 1: 'stmt' | 'assign' | 'while' | 'variable' | 'constant' | 'prog_line'
		for (vector<string>::const_iterator it = DESIGN_ENTITIES.begin(); it != DESIGN_ENTITIES.end();) {
			if (_nextToken.getTokenName() == *it) {
				match(*it);
				matchDeclarationVar(*it);
				// Reset the iterator to the start of the design entities list
				it = DESIGN_ENTITIES.begin();
			}
			else {
				// Forward the iterator until end of list
				it++;
			}
		}
	}

}

void QueryValidator::matchDeclarationVar(string entity) {
	string synonym = _nextToken.getTokenName();

	// Make sure synonym is IDENT, else invalid synonym name
	if (_nextToken.getTokenType() != IDENT) {
		throw(QueryException("Invalid Query : Unexpected synonym that begins with '" + _nextToken.getTokenName() + "'"));
	}

	// Check if synonym has already been declared
	if (_synToEntityMap[synonym] != "") {
		throw(QueryException("Invalid Query : Synonym '" + synonym + "' has been redeclared"));
	}

	// Update syn to entity map
	_synToEntityMap[synonym] = entity;
	 
	match(synonym);
	// Check if next token is end of declaration for this entity type
	// If not, get more
	if (_nextToken.getTokenName() != ";") {
		match(",");
		matchDeclarationVar(entity);
	}
	else {
		// End declaration for this entity type
		match(";");
	}
}

void QueryValidator::matchSelect() {
	match("Select");
	matchSelectResult();
	// Match 'Such That' | 'Pattern'
	while (_nextToken.getTokenName() == "such" || _nextToken.getTokenName() == "pattern") {
		matchClause();
		restrainCommonSynonym();
	}
	if (_nextToken.getTokenName() != "") {
		throw (QueryException("Invalid Query!"));
	}
}

void QueryValidator::matchSelectResult() {
	if (_nextToken.getTokenName() == "BOOLEAN") {
		// Could declare SelectClause here and init it in QueryTable to return
		vector<string> selectArg1({ "BOOLEAN" });
		Clause selectClause("SELECT", selectArg1, selectArg1);
		_qt.setSelectClause(selectClause);
		match("BOOLEAN");
	}
	else {
		// If not BOOLEAN, should be a synonym. Match it
		string syn = _nextToken.getTokenName();
		if (_synToEntityMap[syn] == "") {
			throw(QueryException("Invalid Query : Unexpected synonym '" + syn + "' in Select clause"));
		}
		else {
			// If syn not found in map, JIBABOM. Else match it
			vector<string> selectArg1({ syn });
			vector<string> selectArgType({ _synToEntityMap[syn] });
			Clause selectClause("select", selectArg1, selectArgType);
			_qt.setSelectClause(selectClause);
		}
		match(syn);
	}
}

void QueryValidator::matchClause() {
	if (_nextToken.getTokenName() == "such") {
		matchSuchThat();
	}
	else if (_nextToken.getTokenName() == "pattern") {
		matchPattern();
	}
	else {
		// No SUCHTHAT or PATTERN so should be invalid query
		throw(QueryException("Invalid Query: Unexpected token '" + _nextToken.getTokenName() + "'; Expecting 'SUCH THAT' or 'PATTERN'"));
	}
}

void QueryValidator::matchSuchThat() {
	match("such");
	match("that");
	matchRelation();
}

void QueryValidator::matchPattern() {
	match("pattern");
	matchPatternAssign();
}

void QueryValidator::matchPatternAssign() {
	// pattern-cl : ‘pattern’ syn-assign (entRef, expression-spec)
	// expression-spec : ‘_’ ‘"’ factor ‘"’ ‘_’ | ‘_’
	// factor : var_name | const_value
	string synAssign = _nextToken.getTokenName();
	string synAssignType;
	bool arg2MatchFactor = false;
	pair<int, string> arg1, arg2;
	// Check the syn to entity map and verify if it is "assign" or not. If NOT, ERROR!!!!!!!!!!!!!!!!
	if (_synToEntityMap[synAssign] == "assign") {
		synAssignType = _synToEntityMap[synAssign];
		_synToUseCountMap[synAssign] += 1;
		match(synAssign);
		match("(");
		pair<int,string> arg1 = matchEntRef();
		match(",");
		if (_nextToken.getTokenName() == "_") {
			match("_");
		}
		if (_nextToken.getTokenName() == "\"") {
			match("\"");
			arg2 = matchFactor();
			match("\"");
			match("_");
			arg2MatchFactor = true;
		}
		if (!arg2MatchFactor) {
			arg2 = pair<int, string>(UNDERSCORE, "_");
		}

		match(")");

		// Validate arg1 & arg2
		bool isArg1Valid = false;
		bool isArg2Valid = false;
		string arg1Type, arg2Type;
		if (arg1.first == IDENT) {
			if (_synToEntityMap[arg1.second] != "") {
				_synToUseCountMap[arg1.second] += 1;
				isArg1Valid = _relTable.isArg1Valid("patternAssign", _synToEntityMap[arg1.second]);
				arg1Type = _synToEntityMap[arg1.second];
			}
		}
		else if (arg1.first == UNDERSCORE) {
			isArg1Valid = _relTable.isArg1Valid("patternAssign", "_");
			arg1Type = "any";
		}
		else if (arg1.first == STRING) {
			isArg1Valid = _relTable.isArg1Valid("patternAssign", "string");
			arg1Type = "string";
		}
		if (arg2.first == IDENT) {;
			isArg2Valid = _relTable.isArg2Valid("patternAssign", "string");
			arg2Type = "string";
		}
		else if (arg2.first == INTEGER) {
			isArg2Valid = _relTable.isArg2Valid("patternAssign", "constant");
			arg2Type = "constant";
		}
		else if (arg2.first == UNDERSCORE) {
			isArg2Valid = _relTable.isArg2Valid("patternAssign", "_");
			arg2Type = "any";
		}

		if (isArg1Valid && isArg2Valid) {
			vector<string> patternArg({ arg1.second, arg2.second, synAssign });
			vector<string> patternArgType({ arg1Type,arg2Type, synAssignType });
			Clause patternClause("patternAssign", patternArg, patternArgType);
			_qt.setPatternClause(patternClause);
		}
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + _nextToken.getTokenName() + "'; Expected valid synAssign"));
	}
}

pair<int,string> QueryValidator::matchFactor() {
	if (_nextToken.getTokenType() == IDENT || _nextToken.getTokenType() == INTEGER || _nextToken.getTokenType() == UNDERSCORE) {
		string factor = _nextToken.getTokenName();
		int tokenType = _nextToken.getTokenType();
		match(factor);
		return pair<int,string>(tokenType,factor);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + _nextToken.getTokenName() + "'; Expected a factor"));
	}
}

void QueryValidator::matchRelation() {
	string relation = _nextToken.getTokenName();
	_nextToken = getToken();
	if (_nextToken.getTokenType() == STAR) {
		relation += "*";
		_nextToken = getToken();
	}
	if (relation == "Follows") {
		matchFollow();
	}
	else if (relation == "Follows*") {
		matchFollowStar();
	}
	else if (relation == "Parent") {
		matchParent();
	}
	else if (relation == "Parent*") {
		matchParentStar();
	}
	else if (relation == "Modifies") {
		matchModifies();
	}
	else if (relation == "Uses") {
		matchUses();
	}
	else {
		// If its none of the above relation, ERROR!!!!!!!!!!!!!
		throw(QueryException("Invalid Query : Unexpected relation '" + _nextToken.getTokenName() + "'; Expecting 'Follow'|'Follow*'|'Parent'|'Parent*'|'Modifies'|'Uses'"));
	}
}

void QueryValidator::matchFollow() {
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int, string> arg1 = matchStmtRef();
	match(",");
	pair<int, string> arg2 = matchStmtRef();
	match(")");
	// Validate arg1 and arg2
	bool arg1Valid = false;
	bool arg2Valid = false;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = _relTable.isArg1Valid("follows", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = _relTable.isArg1Valid("follows", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = _relTable.isArg1Valid("follows", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (_synToEntityMap[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = _relTable.isArg2Valid("follows", _synToEntityMap[arg2.second]);
			arg2Type = _synToEntityMap[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = _relTable.isArg2Valid("follows", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = _relTable.isArg2Valid("follows", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> followsArg({ arg1.second,arg2.second });
		vector<string> followsArgType ({ arg1Type,arg2Type });
		Clause followClause("follows", followsArg, followsArgType);
		_qt.setSuchThatClause(followClause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for follows"));
	}

}

void QueryValidator::matchFollowStar() {
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int, string> arg1 = matchStmtRef();
	match(",");
	pair<int, string> arg2 = matchStmtRef();
	match(")");
	
	bool arg1Valid = false;
	bool arg2Valid = false;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = _relTable.isArg1Valid("follows*", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = _relTable.isArg1Valid("follows*", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = _relTable.isArg1Valid("follows*", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (_synToEntityMap[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = _relTable.isArg2Valid("follows*", _synToEntityMap[arg2.second]);
			arg2Type = _synToEntityMap[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = _relTable.isArg2Valid("follows*", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = _relTable.isArg2Valid("follows*", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> followsStarArg({ arg1.second,arg2.second });
		vector<string> followsStarArgType({ arg1Type,arg2Type });
		Clause followStarClause("follows*", followsStarArg, followsStarArgType);
		_qt.setSuchThatClause(followStarClause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for follows*"));
	}
}

void QueryValidator::matchParent() {
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int, string> arg1 = matchStmtRef();
	match(",");
	pair<int, string> arg2 = matchStmtRef();
	match(")");
	
	bool arg1Valid = false;
	bool arg2Valid = false;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = _relTable.isArg1Valid("parent", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = _relTable.isArg1Valid("parent", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = _relTable.isArg1Valid("parent", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (_synToEntityMap[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = _relTable.isArg2Valid("parent", _synToEntityMap[arg2.second]);
			arg2Type = _synToEntityMap[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = _relTable.isArg2Valid("parent", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = _relTable.isArg2Valid("parent", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> parentArg({ arg1.second,arg2.second });
		vector<string> parentArgType({ arg1Type,arg2Type });
		Clause parentClause("parent", parentArg, parentArgType);
		_qt.setSuchThatClause(parentClause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for parent"));
	}
}

void QueryValidator::matchParentStar() {
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int, string> arg1 = matchStmtRef();
	match(",");
	pair<int, string> arg2 = matchStmtRef();
	match(")");
	

	bool arg1Valid = false;
	bool arg2Valid = false;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = _relTable.isArg1Valid("parent*", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = _relTable.isArg1Valid("parent*", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = _relTable.isArg1Valid("parent*", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (_synToEntityMap[arg2.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg2Valid = _relTable.isArg2Valid("parent*", _synToEntityMap[arg2.second]);
			arg2Type = _synToEntityMap[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = _relTable.isArg2Valid("parent*", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = _relTable.isArg2Valid("parent*", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> parentStarArg({ arg1.second,arg2.second });
		vector<string> parentStarArgType({ arg1Type,arg2Type });
		Clause parentStarClause("parent*", parentStarArg, parentStarArgType);
		_qt.setSuchThatClause(parentStarClause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for parent*"));
	}
}

void QueryValidator::matchModifies() {
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int,string> arg1 = matchStmtRef();
	match(",");
	pair<int, string> arg2 = matchEntRef();
	match(")");
	
	bool arg1Valid = false;
	bool arg2Valid = false;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = _relTable.isArg1Valid("modifies", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = _relTable.isArg1Valid("modifies", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = _relTable.isArg1Valid("modifies", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (_synToEntityMap[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = _relTable.isArg2Valid("modifies", _synToEntityMap[arg2.second]);
			arg2Type = _synToEntityMap[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = _relTable.isArg2Valid("modifies", "_");
		arg2Type = "any";
	}
	else if (arg2.first == STRING) {
		arg2Valid = _relTable.isArg2Valid("modifies", "string");
		arg2Type = "string";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> modifiesArg({ arg1.second,arg2.second });
		vector<string> modifiesArgType({ arg1Type,arg2Type });
		Clause modifiesClause("modifies", modifiesArg, modifiesArgType);
		_qt.setSuchThatClause(modifiesClause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for modifies"));
	}
}

void QueryValidator::matchUses() {
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int,string> arg1 = matchStmtRef();
	match(",");
	pair<int,string> arg2 = matchEntRef();
	match(")");

	// Validate arg1 and arg2
	bool arg1Valid = false;
	bool arg2Valid = false;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = _relTable.isArg1Valid("uses", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = _relTable.isArg1Valid("uses", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = _relTable.isArg1Valid("uses", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (_synToEntityMap[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = _relTable.isArg2Valid("uses", _synToEntityMap[arg2.second]);
			arg2Type = _synToEntityMap[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = _relTable.isArg2Valid("uses", "_");
		arg2Type = "any";
	}
	else if (arg2.first == STRING) {
		arg2Valid = _relTable.isArg2Valid("uses", "string");
		arg2Type = "string";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> usesArg({ arg1.second,arg2.second });
		vector<string> usesArgType({ arg1Type,arg2Type });
		Clause usesClause("uses", usesArg, usesArgType);
		_qt.setSuchThatClause(usesClause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for uses"));
	}
}

pair<int,string> QueryValidator::matchStmtRef() {
	// stmtRef : synonym | ‘_’ | INTEGER
	int tokenType;
	string argument;
	if (_nextToken.getTokenType() == IDENT) {
		argument = _nextToken.getTokenName();
		tokenType = _nextToken.getTokenType();
		match(argument);
	}
	else if (_nextToken.getTokenName() == "_") {
		argument = "_";
		tokenType = _nextToken.getTokenType();
		match(UNDERSCORE);
	}
	else if (_nextToken.getTokenType() == INTEGER) {
		argument = _nextToken.getTokenName();
		tokenType = _nextToken.getTokenType();
		match(argument);
	}
	else {
		// Invalid Query
		throw(QueryException("Invalid Query : Unexpected token '" + _nextToken.getTokenName() + "'; Expected stmtRef token"));
	}
	return pair<int, string>(tokenType, argument);
}

pair<int,string> QueryValidator::matchEntRef() {
	// entRef : synonym | ‘_’ | ‘"’ IDENT ‘"’
	string argument;
	int tokenType;
	if (_nextToken.getTokenType() == IDENT) {
		// synonym
		argument = _nextToken.getTokenName();
		tokenType = _nextToken.getTokenType();
		match(argument);
	}
	else if (_nextToken.getTokenName() == "_") {
		// '_'
		argument = "_";
		tokenType = _nextToken.getTokenType();
		match(argument);
	}
	else if (_nextToken.getTokenName() == "\"") {
		// '"' IDENT '"'
		match("\"");
		argument = _nextToken.getTokenName();
		tokenType = STRING;
		match(argument);
		// Some funny thing here
		match("\"");
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + _nextToken.getTokenName() + "'; Expected entRef token"));
	}
	return pair<int,string>(tokenType, argument);
}

void QueryValidator::restrainCommonSynonym() {
	int commonSynCount = 0;
	for (map<string, int>::iterator it = _synToUseCountMap.begin(); it != _synToUseCountMap.end(); it++) {
		if (it->second >= 2) {
			commonSynCount++;
		}
	}

	if (commonSynCount > 1) {
		throw(QueryException("Invalid Query : Too many common synonym"));
	}
}