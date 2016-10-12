#include "QueryValidator.h"

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
		// TODO: Check QueryTable implementation for marking as null query
		return QueryTable(true);
	}
	_qt.optimize();
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
		//std::this_thread::sleep_for(std::chrono::milliseconds(30));
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
		// Iteration 2: 'calls' | 'if' | 'procedure'
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

// TODO: Check if synonym are KEYWORDS. Reject when they are.
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
	_qt.setSynEntityMap(_synToEntityMap);
}

void QueryValidator::matchSelect() {
	// does not work for such that ( match such but does not match that!!!)
	string previousClause = "none";
	match("Select");
	matchSelectResult();
	// Match 'Such That' | 'Pattern' | 'And' | 'With' |
	while (_nextToken.getTokenName() == "such" || _nextToken.getTokenName() == "pattern" || _nextToken.getTokenName() == "with" ||
		_nextToken.getTokenName() == "and") {
		if (_nextToken.getTokenName != "and") {
			previousClause = _nextToken.getTokenName();
			matchClause(previousClause);
			restrainCommonSynonym(); // TODO: Remove this because this was to make sure that there are no more than 1 common syn. Iteration 2 dont care.
		}
		else {
			if (previousClause != "none") { // TODO: Previous clause might be "such". Might not have catered for --> Select a s.t. Follows(s1,s2) and Parent(s1,s2). You are matching 
				matchClause(previousClause); // Select a s.t. Follows(s1,s2) and s.t. Parent(s1,s2)
			}
			else {
				throw (QueryException("Invalid Query : Query begins with an AND clause"));
			}
		}
	}
	if (_nextToken.getTokenName() != "") {
		throw (QueryException("Invalid Query!"));
	}
}

void QueryValidator::matchSelectResult() {
	// "Select" result-cl (suchthat-cl | with-cl | pattern-cl 
	// result-cl : tuple | "BOOLEAN" // Iteration 2: result-cl : synonym | "BOOLEAN"
	// tuple : elem | "<" elem ("," elem)* ">"
	// elem : synonym | attrRef
	vector<string> selectArg, selectArgType;
	string syn, synType, prevToken;
	if (_nextToken.getTokenName() == "BOOLEAN") {
		vector<string> selectArg1({ "BOOLEAN" });
		Clause selectClause("SELECT", selectArg1, selectArg1);
		_qt.setSelectClause(selectClause);
		match("BOOLEAN");
	}
	else if (_nextToken.getTokenType == IDENT) {
		syn = _nextToken.getTokenName();
		synType = _synToEntityMap[syn];
		if (_synToEntityMap.count(syn) == 0) {
			throw(QueryException("Invalid Query : Unexpected synonym '" + syn + "' in Select clause"));
		}
		// result-cl is a synonym or attrRef 
		// attrRef: synonym.attrName
		else {
			selectArg.push_back(syn);
			selectArgType.push_back(_synToEntityMap[syn]);
			match(syn);
			if (_nextToken.getTokenType != DOT) {
				Clause selectClause("select", selectArg, selectArgType);
				_qt.setSelectClause(selectClause);
				match(_nextToken.getTokenName);
			}
			else {
				string attrName = _nextToken.getTokenName();
				if (isAttrNameValid(attrName) && synTypeAndAttrNameMatches(synType, attrName)) {
					selectArg.push_back(attrName); // TODO: Check with cher lin about this
					Clause selectClause("select", selectArg, selectArgType);
					_qt.setSelectClause(selectClause);
					match(attrName);
				}
			}
		}
	}
	else {
		if (_nextToken.getTokenName() == "<") {
			match("<");
			while (_nextToken.getTokenName() != ">") {
				if (_nextToken.getTokenType() == IDENT) {
					syn = _nextToken.getTokenName();
					synType = _synToEntityMap[syn]; //TODO: Should check if the synonym has been declared before using the map.count() just in case
					selectArg.push_back(syn);
					selectArgType.push_back(synType);
					match(syn);
				}
				else if (_nextToken.getTokenType() == COMMA) {
					match(COMMA); // TODO: Currently, this query might pass --> Select <s1,,,,s2> s.t. ...
					if (_nextToken.getTokenName() == ">") {
						throw(QueryException("Invalid Query: Incorrect tuple format"));
					}
				}
			}
			match(">");
		}
	}
}

void QueryValidator::matchClause(string previousClause) {
	if (previousClause == "such") {
		matchSuchThat();
	}
	else if (previousClause == "pattern") {
		matchPattern();
	}
	else if (previousClause == "with") {
		matchWith();
	}
	else {
		// No SUCHTHAT, PATTERN, or WITH clause, query should be invalid
		throw(QueryException("Invalid Query: Unexpected token '" + _nextToken.getTokenName() + "'; Expecting 'SUCH THAT', 'PATTERN', or 'WITH'"));
	}
}

void QueryValidator::matchSuchThat() {
	match("such");
	match("that");
	matchRelation();
}

void QueryValidator::matchPattern() { // TODO: Update match for pattern-if and pattern-while
	match("pattern");
	matchPatternAssign();
}

void QueryValidator::matchWith() {
	match("with");
	matchWithClause();
}

void QueryValidator::matchWithClause() {
	bool arg1Valid = false;
	bool arg2Valid = false;
	string withTypeLHS, withTypeRHS;
	int type;
	string synonym, synType, attrName, argName, argType;
	vector<string> withArg, withArgType;

	vector<pair<int, string>> arg1 = matchRef();
	match("=");
	vector<pair<int, string>> arg2 = matchRef();

	// check number of pairs to determine ref type 
	if (arg1.size() == 2) {
		// attrRef 
		for (vector<pair<int, string>>::const_iterator it = arg1.begin(); it != arg1.end(); it++) {
			synonym = it->second;
			synType = _synToEntityMap[synonym];
			//firstTokenType = it->first;
			it++;
			attrName = it->second;
			if (isAttrNameValid(attrName) &&
				synTypeAndAttrNameMatches(synType, attrName)) {
				arg1Valid = true;
				withTypeLHS = getWithType(attrName, synType);
			}
		}
		withArg.push_back(synonym);
		withArgType.push_back(synType);
	}
	// all other ref options i.e.IDENT | INTEGER | PROG_LINE | SYNONYM
	else {
		for (vector<pair<int, string>>::const_iterator it = arg1.begin(); it != arg1.end(); it++) {
			type = it->first;
			argName = it->second;
			withTypeLHS = getWithType(type);
		}
		// only synonym of prog_line type is allowed
		if (type == IDENT) {
			if (_synToEntityMap[argName] != "" && _synToEntityMap[argName] == "prog_line") {
				_synToUseCountMap[argName] += 1;
				arg1Valid = true;
				argType = _synToEntityMap[argName];
			}
		}
		else if (type == STRING) {
			arg1Valid = _relTable.isArg1Valid(withTypeLHS, "string");
			argType = "string";
		}
		else if (type == INTEGER) {
			arg1Valid = _relTable.isArg1Valid(withTypeLHS, "constant");
			argType = "constant";
		}
		withArg.push_back(argName);
		withArgType.push_back(argType);
	}

	if (arg2.size() == 2) {
		// attrRef 
		for (vector<pair<int, string>>::const_iterator it = arg2.begin(); it != arg2.end(); it++) {
			synonym = it->second;
			synType = _synToEntityMap[synonym];
			//int secondTokenType = it->first;
			it++;
			attrName = it->second;
			if (isAttrNameValid(attrName) &&
				synTypeAndAttrNameMatches(synType, attrName)) {
				arg2Valid = true;
				withTypeRHS = getWithType(attrName, synType);
			}
		}
		withArg.push_back(synonym);
		withArgType.push_back(synType);
	}
	// all other ref options i.e.IDENT | INTEGER | SYNONYM (prog_line)
	else {
		for (vector<pair<int, string>>::const_iterator it = arg2.begin(); it != arg2.end(); it++) {
			type = it->first;
			argName = it->second;
			withTypeRHS = getWithType(type);
		}
		// only synonym of prog_line type is allowed
		if (type == IDENT) {
			if (_synToEntityMap[argName] != "" && _synToEntityMap[argName] == "prog_line") {
				_synToUseCountMap[argName] += 1;
				arg2Valid = true;
				argType = _synToEntityMap[argName];
			}
		}
		else if (type == STRING) {
			arg2Valid = _relTable.isArg1Valid(withTypeRHS, "string");
			argType = "string";
		}
		else if (type == INTEGER) {
			arg2Valid = _relTable.isArg1Valid(withTypeRHS, "constant");
			argType = "constant";
		}
		withArg.push_back(argName);
		withArgType.push_back(argType);
	}
	if (arg1Valid && arg2Valid) {
		if (withTypeLHS == withTypeRHS) {
			Clause withClause(withTypeLHS, withArg, withArgType);
			_withClauses.push_back(withClause);
			_qt.setWithClause(_withClauses);
		}
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for with"));
	}
}

vector<pair<int, string>> QueryValidator::matchRef() {
	// ref : attrRef | synonyn | IDENT | INTEGER
	// attrRef : synonym.attrName
	string synonym, argument;
	int tokenType;
	_nextToken = getToken();
	if (_nextToken.getTokenType() == IDENT) {
		synonym = _nextToken.getTokenName();
		tokenType = _nextToken.getTokenType();
		match(synonym);
		if (_nextToken.getTokenType() != DOT) {
			return vector<pair<int, string>>{{ tokenType, synonym }}; // TODO: This case means the synonym must be prog_line.
		}
		match(".");
		if (_nextToken.getTokenType() == IDENT) {
			string attrName = _nextToken.getTokenName();
			int attrType = _nextToken.getTokenType();
			return vector<pair<int, string>>{ {tokenType, synonym}, {attrType, attrName} };
		}
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
	else if (_nextToken.getTokenType() == INTEGER) {
		argument = _nextToken.getTokenName();
		tokenType = _nextToken.getTokenType();
		match(argument);
		return vector<pair<int, string>>{ { tokenType, argument }};
	}
	else {
		// Invalid Query
		throw(QueryException("Invalid Query : Unexpected token '" + _nextToken.getTokenName() + "'; Expected Ref token"));
	}
}

bool QueryValidator::isAttrNameValid(string attrName) {
	for (vector<string>::const_iterator it = ATTRIBUTE_TYPES.begin(); it != ATTRIBUTE_TYPES.end(); it++) {
		if (attrName != *it) { // TODO: Might always return false.
			return false;
		}
	}
	return true;
}

// TODO: Conditions can be shorten. Not urgent tho.
bool QueryValidator::synTypeAndAttrNameMatches(string synType, string attrName) {
	if (synType == "procedure") {
		if (attrName == "procName") {
			return true;
		}
	}
	else if (synType == "variable") {
		if (attrName == "varName") {
			return true;
		}
	}
	else if (synType == "constant") {
		if (attrName == "value") {
			return true;
		}
	}
	else if (synType == "stmt") {
		if (attrName == "stmt#") {
			return true;
		}
	}
	else if (synType == "while") {
		if (attrName == "stmt#") {
			return true;
		}
	}
	else if (synType == "assign") {
		if (attrName == "stmt#") {
			return true;
		}
	}
	else if (synType == "call") {
		if (attrName == "stmt#" || attrName == "procName") {
			return true;
		}
	}
	else if (synType == "if") {
		if (attrName == "stmt#") {
			return true;
		}
	}
	else {
		return false;
	}
}

string QueryValidator::getWithType(string attrName, string synType) {
	string withType;
	if (synType == "procedure" || synType == "call") {
		withType = "name";
	}
	else if (synType == "while" || synType == "if" || synType == "assign " || synType == "stmt" ||
		synType == "constant") {
		withType = "integer";
	}
	else if (synType == "call") {
		if (attrName == "procName") {
			withType = "name";
		}
		else if (attrName == "stmt#") {
			withType = "integer";
		}
	}
	return withType;
}

string QueryValidator::getWithType(int argType) {
	string withType; 
	if (argType == IDENT) {
		withType = "withName";
	}
	else if (argType == INTEGER) {
		withType = "withInt";
	}
	return withType;
}

void QueryValidator::matchPatternAssign() {
	// pattern-cl : ‘pattern’ syn-assign (entRef, expression-spec)
	// expression-spec : ‘_’ ‘"’ factor ‘"’ ‘_’ | ‘_’
	// factor : var_name | const_value
	string synAssign = _nextToken.getTokenName();
	string synAssignType;
	int arg2MatchFactor = -1;
	pair<int, string> arg1, arg2;
	// Check the syn to entity map and verify if it is "assign" or not. If NOT, ERROR!!!!!!!!!!!!!!!!
	if (_synToEntityMap[synAssign] == "assign") {
		synAssignType = _synToEntityMap[synAssign];
		_synToUseCountMap[synAssign] += 1; // TODO: Can remove
		match(synAssign);
		match("(");
		arg1 = matchEntRef();
		match(",");
		if (_nextToken.getTokenName() == "_") {
			match("_");
		}
		if (_nextToken.getTokenName() == "\"") {
			match("\"");
			arg2 = matchFactor();
			match("\"");
			arg2MatchFactor = 1;
		}
		if (arg2MatchFactor == -1) {
			arg2 = pair<int, string>(UNDERSCORE, "_");
		}

		match(")");

		// Validate arg1 & arg2
		int isArg1Valid = -1;
		int isArg2Valid = -1;
		string arg1Type, arg2Type;
		if (arg1.first == IDENT) {
			if (_synToEntityMap[arg1.second] != "") {
				_synToUseCountMap[arg1.second] += 1; // Can remove
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
			vector<string> patternArg({ synAssign, arg1.second, arg2.second });
			vector<string> patternArgType({ synAssignType, arg1Type,arg2Type });
			Clause patternClause("pattern", patternArg, patternArgType);
			_patternClauses.push_back(patternClause);
			_qt.setPatternClause(_patternClauses);
		}
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + _nextToken.getTokenName() + "'; Expected valid synAssign"));
	}
}

void QueryValidator::matchPatternWhile() {
	// while : synonym (varRef ,"_")
	// varRef : synonym | "_" | "IDENT"
	string syn = _nextToken.getTokenName();
	string synType;
	pair<int, string> arg1, arg2;
	// Check the syn to entity map and verify if it is "assign" or not. If NOT, ERROR!!!!!!!!!!!!!!!!
	if (_synToEntityMap[syn] == "while") {
		synType = _synToEntityMap[syn];
		_synToUseCountMap[syn] += 1;
		match(syn);
		match("(");
		arg1 = matchEntRef();
		match(",");
		if (_nextToken.getTokenName() == "_") {
			match("_");
			arg2 = pair<int, string>(UNDERSCORE, "_");
		}
		match(")");
	}
	// Validate arg1 & arg2
	int isArg1Valid = -1;
	int isArg2Valid = -1;
	string arg1Type, arg2Type;
	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			isArg1Valid = _relTable.isArg1Valid("patternWhile", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		isArg1Valid = _relTable.isArg1Valid("patternWhile", "_");
		arg1Type = "any";
	}
	else if (arg1.first == STRING) {
		isArg1Valid = _relTable.isArg1Valid("patternWhile", "string");
		arg1Type = "string";
	}
	else if (arg2.first == UNDERSCORE) {
		isArg2Valid = _relTable.isArg2Valid("patternWhile", "_");
		arg2Type = "any";
	}

	if (isArg1Valid && isArg2Valid) {
		vector<string> patternArg({ syn, arg1.second, arg2.second });
		vector<string> patternArgType({ synType, arg1Type,arg2Type });
		Clause patternClause("pattern", patternArg, patternArgType);
		_patternClauses.push_back(patternClause);
		_qt.setPatternClause(_patternClauses);
	}
}

void QueryValidator::matchPatternIf() {
	// if : synonym (varRef, "_", "_")
	// varRef : synonym | "_" | "IDENT"
	string syn = _nextToken.getTokenName();
	string synType;
	pair<int, string> arg1, arg2;
	// Check the syn to entity map and verify if it is "assign" or not. If NOT, ERROR!!!!!!!!!!!!!!!!
	if (_synToEntityMap[syn] == "if") {
		synType = _synToEntityMap[syn];
		_synToUseCountMap[syn] += 1;
		match(syn);
		match("(");
		arg1 = matchEntRef();
		match(",");
		if (_nextToken.getTokenName() == "_") {
			match("_");
			arg2 = pair<int, string>(UNDERSCORE, "_");
		}
		match(")");
	}
	// Validate arg1 & arg2
	int isArg1Valid = -1;
	int isArg2Valid = -1;
	string arg1Type, arg2Type;
	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			isArg1Valid = _relTable.isArg1Valid("patternIf", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		isArg1Valid = _relTable.isArg1Valid("patternIf", "_");
		arg1Type = "any";
	}
	else if (arg1.first == STRING) {
		isArg1Valid = _relTable.isArg1Valid("patternIf", "string");
		arg1Type = "string";
	}
	else if (arg2.first == UNDERSCORE) {
		isArg2Valid = _relTable.isArg2Valid("patternIf", "_");
		arg2Type = "any";
	}

	if (isArg1Valid && isArg2Valid) {
		vector<string> patternArg({ syn, arg1.second, arg2.second });
		vector<string> patternArgType({ synType, arg1Type,arg2Type });
		Clause patternClause("pattern", patternArg, patternArgType);
		_patternClauses.push_back(patternClause);
		_qt.setPatternClause(_patternClauses);
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
	else if (relation == "Call") {
		matchCalls();
	}
	else if (relation == "Call*") {
		matchCallsStar();
	}
	else if (relation == "Next") {
		matchNext();
	}
	else if (relation == "Next*") {
		matchNextStar();
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
	int arg1Valid = -1;
	int arg2Valid = -1;
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
		Clause followsRelation("follows", followsArg, followsArgType);
		_suchThatClauses.push_back(followsRelation);
		_qt.setSuchThatClause(_suchThatClauses);
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
	
	int arg1Valid = -1;
	int arg2Valid = -1;
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
		Clause followsStarRel("follows*", followsStarArg, followsStarArgType);
		_suchThatClauses.push_back(followsStarRel);
		_qt.setSuchThatClause(_suchThatClauses);
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
	
	int arg1Valid = -1;
	int arg2Valid = -1;
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
		Clause parentRel("parent", parentArg, parentArgType);
		_suchThatClauses.push_back(parentRel);
		_qt.setSuchThatClause(_suchThatClauses);
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
	
	int arg1Valid = -1;
	int arg2Valid = -1;
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
		Clause parentStarRel("parent*", parentStarArg, parentStarArgType);
		_suchThatClauses.push_back(parentStarRel);
		_qt.setSuchThatClause(_suchThatClauses);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for parent*"));
	}
}

void QueryValidator::matchModifies() {
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int,string> arg1 = matchStmtRef(); // TODO: Might need to change and cater for entRef also. Since now can be procedure/"string"
	match(",");
	pair<int, string> arg2 = matchEntRef();
	match(")");
	
	int arg1Valid = -1;
	int arg2Valid = -1;
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
	// arg 1 is a procedure e.g. Modifies("First", "x")
	else if (arg1.first == STRING) {
		arg1Valid = _relTable.isArg1Valid("modifies", "string");
		arg1Type = "string";
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
		Clause modifiesRel("modifies", modifiesArg, modifiesArgType);
		_suchThatClauses.push_back(modifiesRel);
		_qt.setSuchThatClause(_suchThatClauses);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for modifies"));
	}
}

void QueryValidator::matchUses() {
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int,string> arg1 = matchStmtRef(); // TODO: Same as modifies. Need cater for ModifiesP
	match(",");
	pair<int,string> arg2 = matchEntRef();
	match(")");

	// Validate arg1 and arg2
	int arg1Valid = -1;
	int arg2Valid = -1;
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
		Clause usesRel("uses", usesArg, usesArgType);
		_suchThatClauses.push_back(usesRel);
		_qt.setSuchThatClause(_suchThatClauses);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for uses"));
	}
}

void QueryValidator::matchCalls() {
	// Calls:"Calls""("entRef", "entRef")" 
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int, string> arg1 = matchEntRef();
	match(",");
	pair<int, string> arg2 = matchEntRef();
	match(")");

	int arg1Valid = -1;
	int arg2Valid = -1;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = _relTable.isArg1Valid("calls", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = _relTable.isArg1Valid("calls", "_");
		arg1Type = "any";
	}
	else if (arg1.first == STRING) {
		arg1Valid = _relTable.isArg1Valid("calls", "string");
		arg1Type = "string";
	}
	/* call rel does not accept int as parameter
	else if (arg1.first == INTEGER) {
		arg1Valid = _relTable.isArg1Valid("calls", "constant");
		arg1Type = "constant";
	}
	*/
	if (arg2.first == IDENT) {
		if (_synToEntityMap[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = _relTable.isArg2Valid("calls", _synToEntityMap[arg2.second]);
			arg2Type = _synToEntityMap[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = _relTable.isArg2Valid("calls", "_");
		arg2Type = "any";
	}
	else if (arg2.first == STRING) {
		arg2Valid = _relTable.isArg2Valid("calls", "string");
		arg2Type = "string";
	}
	/* call rel does not accept int as parameter
	else if (arg2.first == INTEGER) {
		arg2Valid = _relTable.isArg2Valid("calls", "constant");
		arg2Type = "constant";
	}
	*/
	if (arg1Valid && arg2Valid) {
		vector<string> callsArg({ arg1.second,arg2.second });
		vector<string> callsArgType({ arg1Type,arg2Type });
		Clause callsRel("calls", callsArg, callsArgType);
		_suchThatClauses.push_back(callsRel);
		_qt.setSuchThatClause(_suchThatClauses);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for calls"));
	}
}

void QueryValidator::matchCallsStar() {
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int, string> arg1 = matchEntRef();
	match(",");
	pair<int, string> arg2 = matchEntRef();
	match(")");

	int arg1Valid = -1;
	int arg2Valid = -1;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = _relTable.isArg1Valid("calls*", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = _relTable.isArg1Valid("calls*", "_");
		arg1Type = "any";
	}
	else if (arg1.first == STRING) {
		arg1Valid = _relTable.isArg1Valid("calls*", "string");
		arg1Type = "string";
	}
	if (arg2.first == IDENT) {
		if (_synToEntityMap[arg2.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg2Valid = _relTable.isArg2Valid("calls*", _synToEntityMap[arg2.second]);
			arg2Type = _synToEntityMap[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = _relTable.isArg2Valid("calls*", "_");
		arg2Type = "any";
	}
	else if (arg2.first == STRING) {
		arg2Valid = _relTable.isArg2Valid("calls*", "string");
		arg2Type = "string";
	}
	if (arg1Valid && arg2Valid) {
		vector<string> callsStarArg({ arg1.second,arg2.second });
		vector<string> callsStarArgType({ arg1Type,arg2Type });
		Clause callsStarRel("calls*", callsStarArg, callsStarArgType);
		_suchThatClauses.push_back(callsStarRel);
		_qt.setSuchThatClause(_suchThatClauses);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for calls*"));
	}
}

void QueryValidator::matchNext() {
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int, string> arg1 = matchStmtRef();
	match(",");
	pair<int, string> arg2 = matchStmtRef();
	match(")");

	int arg1Valid = -1;
	int arg2Valid = -1;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = _relTable.isArg1Valid("next", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = _relTable.isArg1Valid("next", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = _relTable.isArg1Valid("next", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (_synToEntityMap[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = _relTable.isArg2Valid("next", _synToEntityMap[arg2.second]);
			arg2Type = _synToEntityMap[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = _relTable.isArg2Valid("next", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = _relTable.isArg2Valid("next", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> nextArg({ arg1.second,arg2.second });
		vector<string> nextArgType({ arg1Type,arg2Type });
		Clause nextRel("next", nextArg, nextArgType);
		_suchThatClauses.push_back(nextRel);
		_qt.setSuchThatClause(_suchThatClauses);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for next"));
	}
}

void QueryValidator::matchNextStar() {
	// Check if arguments, num of arguments are valid
	match("(");
	pair<int, string> arg1 = matchStmtRef();
	match(",");
	pair<int, string> arg2 = matchStmtRef();
	match(")");

	int arg1Valid = -1;
	int arg2Valid = -1;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (_synToEntityMap[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = _relTable.isArg1Valid("next*", _synToEntityMap[arg1.second]);
			arg1Type = _synToEntityMap[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = _relTable.isArg1Valid("next*", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = _relTable.isArg1Valid("next*", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (_synToEntityMap[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = _relTable.isArg2Valid("next*", _synToEntityMap[arg2.second]);
			arg2Type = _synToEntityMap[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = _relTable.isArg2Valid("next*", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = _relTable.isArg2Valid("next*", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> nextStarArg({ arg1.second,arg2.second });
		vector<string> nextStarArgType({ arg1Type,arg2Type });
		Clause nextStarRel("next*", nextStarArg, nextStarArgType);
		_suchThatClauses.push_back(nextStarRel);
		_qt.setSuchThatClause(_suchThatClauses);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for nextStar"));
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
	// entRef : synonym | ‘_’ | ‘"’ IDENT ‘"’ | INTEGER
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
	else if (_nextToken.getTokenType() == INTEGER) {
		argument = _nextToken.getTokenName();
		tokenType = _nextToken.getTokenType();
		match(argument);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + _nextToken.getTokenName() + "'; Expected entRef token"));
	}
	
	return pair<int,string>(tokenType, argument);
}

// TODO: Remove this cause it was for iteration 1
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