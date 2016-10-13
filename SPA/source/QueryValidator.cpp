#include "QueryValidator.h"

QueryValidator::QueryValidator() {
	query_table_ = QueryTable(false);
}

QueryValidator::~QueryValidator() {

}

QueryValidator::QueryValidator(string query) {
	query_string_ = query;
	tokenizer_ = QueryTokenizer(query);
	
}

QueryTable QueryValidator::parse() {
	try {
		matchDeclaration();
		matchSelect();
	}
	catch (QueryException& e) { 
		// Catch exception by reference. Internet recommendation
		// Output error message onto console. Purely debug for now
		cout << e.what() << endl;
		// Return a null pointer to indicate that an invalid query has been sent in
		// TODO: Check QueryTable implementation for marking as null query
		return QueryTable(true);
	}

	query_table_.optimize();
	return query_table_;
}

QueryToken QueryValidator::getToken() {
	return tokenizer_.tokenize();
}

void QueryValidator::match(string token) {
	if (next_token_.getTokenName() == token) {
		next_token_ = getToken();
	}
	else {
		throw(QueryException("Invalid query: Unexpected token '" + next_token_.getTokenName() + "; Expected token '" + token));
	}
}

void QueryValidator::match(int tokenType) {
	if (next_token_.getTokenType() == tokenType) {
		next_token_ = getToken();
	}
	else {
		throw(QueryException("Invalid query: Unexpected token type'" + to_string(next_token_.getTokenType()) + "; Expected tokenType '" + to_string(tokenType)));
	}
}
	
void QueryValidator::matchDeclaration() {
	next_token_ = getToken();
	// If the first token is not "Select", that means there are Declarations
	if (next_token_.getTokenName() != "Select") {
		// Check if the next token are any of the accepted design entities
		// Iteration 1: 'stmt' | 'assign' | 'while' | 'variable' | 'constant' | 'prog_line'
		// Iteration 2: 'calls' | 'if' | 'procedure'
		for (vector<string>::const_iterator it = DESIGN_ENTITIES.begin(); it != DESIGN_ENTITIES.end();) {
			if (next_token_.getTokenName() == *it) {
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
	string synonym = next_token_.getTokenName();

	// Make sure synonym is IDENT, else invalid synonym name
	if (next_token_.getTokenType() != IDENT) {
		throw(QueryException("Invalid Query : Unexpected synonym that begins with '" + next_token_.getTokenName() + "'"));
	}

	// Check if synonym has already been declared
	if (syn_to_entity_map_[synonym] != "") {
		throw(QueryException("Invalid Query : Synonym '" + synonym + "' has been redeclared"));
	}

	// Update syn to entity map
	syn_to_entity_map_[synonym] = entity;
	match(synonym);

	// Check if next token is end of declaration for this entity type
	// If not, get more
	if (next_token_.getTokenName() != ";") {
		match(",");
		matchDeclarationVar(entity);
	}
	else {
		// End declaration for this entity type
		match(";");
	}
	query_table_.setSynEntityMap(syn_to_entity_map_);
}

void QueryValidator::matchSelect() {
	string previousClause = "none";
	match("Select");
	// matchResultClauseElement();
	// Match 'Such That' | 'Pattern' | 'And' | 'With' |

	while (next_token_.getTokenName() == "such" || next_token_.getTokenName() == "pattern" || next_token_.getTokenName() == "with") {
		matchClause(next_token_.getTokenName());
	}
	if (next_token_.getTokenName() != "") {
		throw (QueryException("Invalid Query! : Not in PQL Grammar: " + next_token_.getTokenName()));
	}
}

void QueryValidator::matchResultClause() {
	// "Select" result-cl (suchthat-cl | with-cl | pattern-cl 
	// result-cl : tuple | "BOOLEAN" // Iteration 2: result-cl : synonym | "BOOLEAN"
	// tuple : elem | "<" elem ("," elem)* ">"
	// elem : synonym | attrRef
	vector<string> selectArg, selectArgType;
	string syn, synType, prevToken;

	if (next_token_.getTokenName() == "BOOLEAN") {
		vector<string> selectArg1({ "BOOLEAN" });
		Clause selectClause("SELECT", selectArg1, selectArg1);
		query_table_.setSelectClause(selectClause);
		match("BOOLEAN");
	}
	else {
		matchTupleResult();
	}
	/*
	else if (next_token_.getTokenType == IDENT) {
		syn = next_token_.getTokenName();
		synType = syn_to_entity_map_[syn];
		if (syn_to_entity_map_.count(syn) == 0) {
			throw(QueryException("Invalid Query : Unexpected synonym '" + syn + "' in Select clause"));
		}
		// result-cl is a synonym or attrRef 
		// attrRef: synonym.attrName
		else {
			selectArg.push_back(syn);
			selectArgType.push_back(syn_to_entity_map_[syn]);
			match(syn);
			if (next_token_.getTokenType != DOT) {
				Clause selectClause("select", selectArg, selectArgType);
				query_table_.setSelectClause(selectClause);
				match(next_token_.getTokenName);
			}
			else {
				string attrName = next_token_.getTokenName();
				if (isAttrNameValid(attrName) && synTypeAndAttrNameMatches(synType, attrName)) {
					selectArg.push_back(attrName); // TODO: Check with cher lin about this
					Clause selectClause("select", selectArg, selectArgType);
					query_table_.setSelectClause(selectClause);
					match(attrName);
				}
			}
		}
	}
	else {
		if (next_token_.getTokenName() == "<") {
			match("<");
			while (next_token_.getTokenName() != ">") {
				if (next_token_.getTokenType() == IDENT) {
					syn = next_token_.getTokenName();
					synType = syn_to_entity_map_[syn]; //TODO: Should check if the synonym has been declared before using the map.count() just in case
					selectArg.push_back(syn);
					selectArgType.push_back(synType);
					match(syn);
				}
				else if (next_token_.getTokenType() == COMMA) {
					match(COMMA); // TODO: Currently, this query might pass --> Select <s1,,,,s2> s.t. ...
					if (next_token_.getTokenName() == ">") {
						throw(QueryException("Invalid Query: Incorrect tuple format"));
					}
				}
			}
			match(">");
		}
	}
	*/
}

void QueryValidator::matchTupleResult()
{
	if (next_token_.getTokenName() == "<") {
		match("<");
		matchResultClauseElement(true);
		match(">");
	}
	else {
		matchResultClauseElement(false);
	}
}

void QueryValidator::matchResultClauseElement(bool is_tuple)
{
	vector<string> selectArg, selectArgType;
	string result_syn, result_syn_type;

	if (next_token_.getTokenType == IDENT) {
		result_syn = next_token_.getTokenName();
		result_syn_type = syn_to_entity_map_[result_syn];
		if (syn_to_entity_map_.count(result_syn) == 0) {
			throw(QueryException("Invalid Query : Unexpected synonym '" + result_syn + "' in Select clause"));
		}

		selectArg.push_back(result_syn);
		selectArgType.push_back(result_syn_type);
		match(result_syn);
	}

	if (next_token_.getTokenName() == ".") {
		string attribute_type = next_token_.getTokenName();
		if ((isAttrNameValid(attribute_type) == true) && (synTypeAndAttrNameMatches(result_syn_type, attribute_type) == true)) {
			// To be completed in iteration 3
		}
		else {
			throw(QueryException("Invalid Query : Unexpected attribute '" + result_syn + "." + attribute_type + "' in Select clause"));
		}
	}

	if ((is_tuple == true) && (next_token_.getTokenName() == ",")) {
		match(",");
		matchResultClauseElement(true);
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
		throw(QueryException("Invalid Query: Unexpected token '" + next_token_.getTokenName() + "'; Expecting 'SUCH THAT', 'PATTERN', or 'WITH'"));
	}
}

void QueryValidator::matchSuchThat() {
	match("such");
	match("that");
	matchRelation();
	while (next_token_.getTokenName() == "and") {
		match("and");
		matchRelation();
	}
}

void QueryValidator::matchPatternClause()
{
	match("pattern");
	matchPattern();
	while (next_token_.getTokenName() == "and") {
		match("and");
		matchPattern();
	}
}

void QueryValidator::matchPattern() { // TODO: Update match for pattern-if and pattern-while
	string pattern_syn = next_token_.getTokenName();
	string pattern_syn_type = syn_to_entity_map_[pattern_syn];
	if (syn_to_entity_map_.count(pattern_syn) == 0) {
		throw(QueryException("Invalid Query : Unexpected synonym '" + pattern_syn + "' in Pattern clause"));

	}

	if (pattern_syn_type == "assign") {
		matchPatternAssign();
	}
	else if (pattern_syn_type == "while") {
		matchPatternWhile();
	}
	else if (pattern_syn_type == "if") {
		matchPatternIf();
	}
	else {
		throw(QueryException("Invalid Query : Unexpected synonym type '" + pattern_syn_type + "' in Pattern clause"));
	}
}

void QueryValidator::matchWith() {
	match("with");
	matchAttrCond();
	while (next_token_.getTokenName() == "and") {
		match("and");
		matchAttrCond();
	}
}

void QueryValidator::matchAttrCond()
{
	// attrCompare: ref '=' ref
	// Get left and right. Then check validity and then create the with clause
	
}

void QueryValidator::matchAttrCompare()
{
	vector<string> with_arg, with_arg_type;
	pair<int, string> with_left = matchRefNew(true);
	match("=");
	pair<int, string> with_right = matchRefNew(false);

	if ((with_left.first == STRING && with_right.first == INTEGER) || (with_left.first == INTEGER && with_right.first == INTEGER)) {
		throw(QueryException("Invalid Query : Mismatch attributes, '" + with_left.second + "' and '" + with_right.second + "'"));
	}

	if (with_left.first == IDENT) {
		if (with_right.first == IDENT) {
			if (isLegalWith(with_left.second, with_right.second) == true) {
				with_arg.push_back(with_left.second);
				with_arg.push_back(with_right.second);
				if (with_type_left == "name") {
					if (syn_to_entity_map_[with_left.second] == "call") {
						with_arg_type.push_back("call_name");
					}
					else {
						with_arg_type.push_back(syn_to_entity_map_[with_left.second]);
					}

					if (syn_to_entity_map_[with_right.second] == "call") {
						with_arg_type.push_back("call_name");
					}
				}
				else {
					if (syn_to_entity_map_[with_right.second] == "call") {
						with_arg_type.push_back("call_number");
					}
					else {
						with_arg_type.push_back(syn_to_entity_map_[with_left.second]);
					}
				}
				Clause with_clause("with", with_arg, with_arg_type);
				query_table_.AddWithClause(with_clause);
			}
			else {
				throw(QueryException("Invalid Query : Mismatch attributes, '" + with_left.second + "' and '" + with_right.second + "'"));
			}
		}
		else if (with_right.first == STRING) {
			string with_left_syn_type = syn_to_entity_map_[with_left.second];
			with_arg.push_back(with_left.second);
			with_arg.push_back(with_right.second);
			if ((with_left_syn_type == "call") && (with_type_left == "name")) {
				with_arg_type.push_back("call_name");
				with_arg_type.push_back("string");
				Clause with_clause("with", with_arg, with_arg_type);
				query_table_.AddWithClause(with_clause);
			}
			else if (IsNameType(with_left_syn_type) == true) {
				with_arg.push_back(with_left.second);
				with_arg.push_back(with_right.second);
				with_arg_type.push_back(syn_to_entity_map_[with_left.second]);
				with_arg_type.push_back("string");
				Clause with_clause("with", with_arg, with_arg_type);
				query_table_.AddWithClause(with_clause);
			}
			else {
				throw(QueryException("Invalid Query : Mismatch attributes, '" + with_left.second + "' and '" + with_right.second + "'"));
			}
		}
		else if (with_right.first == INTEGER) {

		}
		else {
			// ERROR
		}
	}
	else if (with_left.first == STRING) {
		if (with_right.first == IDENT) {

		}
		else if (with_right.first == STRING) {

		}
		else if (with_right.first == INTEGER) {

		}
		else {
			// ERROR
		}
	}
	else if (with_left.first == INTEGER) {
		if (with_right.first == IDENT) {

		}
		else if (with_right.first == STRING) {

		}
		else if (with_right.first == INTEGER) {

		}
		else {
			// ERROR
		}
	}
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
			synType = syn_to_entity_map_[synonym];
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
			if (syn_to_entity_map_[argName] != "" && syn_to_entity_map_[argName] == "prog_line") {
				_synToUseCountMap[argName] += 1;
				arg1Valid = true;
				argType = syn_to_entity_map_[argName];
			}
		}
		else if (type == STRING) {
			arg1Valid = rel_table_.isArg1Valid(withTypeLHS, "string");
			argType = "string";
		}
		else if (type == INTEGER) {
			arg1Valid = rel_table_.isArg1Valid(withTypeLHS, "constant");
			argType = "constant";
		}
		withArg.push_back(argName);
		withArgType.push_back(argType);
	}

	if (arg2.size() == 2) {
		// attrRef 
		for (vector<pair<int, string>>::const_iterator it = arg2.begin(); it != arg2.end(); it++) {
			synonym = it->second;
			synType = syn_to_entity_map_[synonym];
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
			if (syn_to_entity_map_[argName] != "" && syn_to_entity_map_[argName] == "prog_line") {
				_synToUseCountMap[argName] += 1;
				arg2Valid = true;
				argType = syn_to_entity_map_[argName];
			}
		}
		else if (type == STRING) {
			arg2Valid = rel_table_.isArg1Valid(withTypeRHS, "string");
			argType = "string";
		}
		else if (type == INTEGER) {
			arg2Valid = rel_table_.isArg1Valid(withTypeRHS, "constant");
			argType = "constant";
		}
		withArg.push_back(argName);
		withArgType.push_back(argType);
	}
	if (arg1Valid && arg2Valid) {
		if (withTypeLHS == withTypeRHS) {
			Clause withClause(withTypeLHS, withArg, withArgType);
			_withClauses.push_back(withClause);
			query_table_.setWithClause(_withClauses);
		}
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for with"));
	}
}

vector<pair<int, string>> QueryValidator::matchRef() {
	// ref : attrRef | synonyn | IDENT | INTEGER
	// attrRef : synonym.attrName
	string with_syn, with_syn_type, argument;
	int tokenType;

	if (next_token_.getTokenType() == IDENT) {
		with_syn = next_token_.getTokenName();
		with_syn_type = syn_to_entity_map_[with_syn];
		if (syn_to_entity_map_.count(with_syn) == 0) {
			throw(QueryException("Invalid Query : Unexpected synonym '" + with_syn + "' in with clause"));
		}
		match(with_syn);
		tokenType = next_token_.getTokenType();
		if (next_token_.getTokenType() != DOT) {
			if (with_syn_type != "prog_line") {
				throw(QueryException("Invalid Query : Unexpected synonym '" + with_syn + "' in with clause"));
			}
			return vector<pair<int, string>>{{ tokenType, with_syn }}; // TODO: This case means the synonym must be prog_line.
		}
		match(".");
		if (next_token_.getTokenType() == IDENT) {
			string attrName = next_token_.getTokenName();
			int attrType = next_token_.getTokenType();
			match(attrName);
			return vector<pair<int, string>>{ {tokenType, with_syn}, {attrType, attrName} };
		}
		else {
			throw(QueryException("Invalid Query : Unexpected attibute '" + next_token_.getTokenName() + "' in with clause"));
		}
	}
	else if (next_token_.getTokenName() == "\"") {
		// '"' IDENT '"'
		match("\"");
		argument = next_token_.getTokenName();
		tokenType = STRING;
		match(argument);
		match("\"");
		return vector<pair<int, string>>{ { tokenType, argument }};
	}
	else if (next_token_.getTokenType() == INTEGER) {
		argument = next_token_.getTokenName();
		tokenType = next_token_.getTokenType();
		match(argument);
		return vector<pair<int, string>>{ { tokenType, argument }};
	}
	else {
		// Invalid Query
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.getTokenName() + "'; Expected Ref token"));
	}
}

pair<int, string> QueryValidator::matchRefNew(bool is_left)
{
	// ref : attrRef | synonyn | IDENT | INTEGER
	// attrRef : synonym.attrName
	string with_syn, with_syn_type, argument;
	int tokenType;

	if (next_token_.getTokenType() == IDENT) {
		with_syn = next_token_.getTokenName();
		with_syn_type = syn_to_entity_map_[with_syn];
		if (syn_to_entity_map_.count(with_syn) == 0) {
			throw(QueryException("Invalid Query : Unexpected synonym '" + with_syn + "' in with clause"));
		}
		match(with_syn);
		tokenType = next_token_.getTokenType();
		if (next_token_.getTokenType() != DOT) {
			if (with_syn_type != "prog_line") {
				throw(QueryException("Invalid Query : Unexpected synonym '" + with_syn + "' in with clause"));
			}
			else {
				if (is_left == true) with_type_left = "number";
				if (is_left == false) with_type_right = "number";
				return pair<int, string>(IDENT, with_syn);
			}
		}
		match(".");
		if (next_token_.getTokenType() == IDENT) {
			string attrName = next_token_.getTokenName();
			int attrType = next_token_.getTokenType();
			if (synTypeAndAttrNameMatches(with_syn_type, attrName) == false) {
				throw(QueryException("Invalid Query : Unexpected attribute for '" + with_syn + "' in with clause"));
			}
			match(attrName);
			if (attrName == "procName" || attrName == "varName") {
				if (is_left == true) with_type_left = "name";
				if (is_left == false) with_type_right = "name";
			}
			else {
				if (is_left == true) with_type_left = "number";
				if (is_left == false) with_type_right = "number";
			}
			return pair<int, string>(IDENT, with_syn);
		}
		else {
			throw(QueryException("Invalid Query : Unexpected attibute '" + next_token_.getTokenName() + "' in with clause"));
		}
	}
	else if (next_token_.getTokenName() == "\"") {
		// '"' IDENT '"'
		match("\"");
		argument = next_token_.getTokenName();
		tokenType = STRING;
		match(argument);
		match("\"");
		if (is_left == true) with_type_left = "name";
		if (is_left == false) with_type_right = "name";
		return pair<int, string>(STRING, argument);
	}
	else if (next_token_.getTokenType() == INTEGER) {
		argument = next_token_.getTokenName();
		tokenType = next_token_.getTokenType();
		match(argument);
		if (is_left == true) with_type_left = "number";
		if (is_left == false) with_type_right = "number";
		return pair<int, string>(INTEGER, argument);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.getTokenName() + "'; Expected Ref token"));
	}
}

bool QueryValidator::isAttrNameValid(string attrName) {
	vector<string>::const_iterator it;
	it = find(ATTRIBUTE_TYPES.begin(), ATTRIBUTE_TYPES.end(), attrName);
	if (it != ATTRIBUTE_TYPES.end()) {
		return true;
	}
	else {
		return false;
	}
}

bool QueryValidator::synTypeAndAttrNameMatches(string synType, string attrName) {
	if (((synType == "procedure") || (synType == "call")) && (attrName == "procName")) {
		return true;
	}
	else if ((synType == "variable") && (attrName == "varName")) {
		return true;
	}
	else if ((synType == "constant") && (attrName == "value")) {
		return true;
	}
	else if ((synType == "stmt") || (synType == "assign") || (synType == "while") || (synType == "if") || (synType == "call")){
		if (attrName == "stmt#") {
			return true;
		}
	}
	else {
		return false;
	}
}

bool QueryValidator::isLegalWith(string syn_one, string syn_two)
{
	return false;
}

bool QueryValidator::IsNameType(string syn_type)
{
	if ((syn_type == "procedure") || (syn_type == "variable")) {
		return true;
	}
	return false;
}

bool QueryValidator::IsStatementType(string syn_type)
{
	// Dunno should add constant or not
	if ((syn_type == "statement") || (syn_type == "assign") || (syn_type == "while") || (syn_type == "if") || (syn_type == "prog_line")) {
		return true;
	}
	return false;
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

string QueryValidator::getWithType(string syn_type)
{
	return string();
}

void QueryValidator::matchPatternAssign() {
	// pattern-cl : ‘pattern’ syn-assign (entRef, expression-spec)
	// expression-spec : ‘_’ ‘"’ factor ‘"’ ‘_’ | ‘_’
	// factor : var_name | const_value
	string synAssign = next_token_.getTokenName();
	string synAssignType;
	int arg2MatchFactor = -1;
	pair<int, string> arg1, arg2;
	// Check the syn to entity map and verify if it is "assign" or not. If NOT, ERROR!!!!!!!!!!!!!!!!
	if (syn_to_entity_map_[synAssign] == "assign") {
		synAssignType = syn_to_entity_map_[synAssign];
		_synToUseCountMap[synAssign] += 1; // TODO: Can remove
		match(synAssign);
		match("(");
		arg1 = matchEntRef();
		match(",");
		if (next_token_.getTokenName() == "_") {
			match("_");
		}
		if (next_token_.getTokenName() == "\"") {
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
			if (syn_to_entity_map_[arg1.second] != "") {
				_synToUseCountMap[arg1.second] += 1; // Can remove
				isArg1Valid = rel_table_.isArg1Valid("patternAssign", syn_to_entity_map_[arg1.second]);
				arg1Type = syn_to_entity_map_[arg1.second];
			}
		}
		else if (arg1.first == UNDERSCORE) {
			isArg1Valid = rel_table_.isArg1Valid("patternAssign", "_");
			arg1Type = "any";
		}
		else if (arg1.first == STRING) {
			isArg1Valid = rel_table_.isArg1Valid("patternAssign", "string");
			arg1Type = "string";
		}
		if (arg2.first == IDENT) {;
			isArg2Valid = rel_table_.isArg2Valid("patternAssign", "string");
			arg2Type = "string";
		}
		else if (arg2.first == INTEGER) {
			isArg2Valid = rel_table_.isArg2Valid("patternAssign", "constant");
			arg2Type = "constant";
		}
		else if (arg2.first == UNDERSCORE) {
			isArg2Valid = rel_table_.isArg2Valid("patternAssign", "_");
			arg2Type = "any";
		}

		if (isArg1Valid && isArg2Valid) {
			vector<string> patternArg({ synAssign, arg1.second, arg2.second });
			vector<string> patternArgType({ synAssignType, arg1Type,arg2Type });
			Clause patternClause("pattern", patternArg, patternArgType);
			_patternClauses.push_back(patternClause);
			query_table_.setPatternClause(_patternClauses);
		}
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.getTokenName() + "'; Expected valid synAssign"));
	}
}

void QueryValidator::matchPatternWhile() {
	// while : synonym (varRef ,"_")
	// varRef : synonym | "_" | "IDENT"
	string syn = next_token_.getTokenName();
	string synType;
	pair<int, string> arg1, arg2;
	// Check the syn to entity map and verify if it is "assign" or not. If NOT, ERROR!!!!!!!!!!!!!!!!
	if (syn_to_entity_map_[syn] == "while") {
		synType = syn_to_entity_map_[syn];
		_synToUseCountMap[syn] += 1;
		match(syn);
		match("(");
		arg1 = matchEntRef();
		match(",");
		if (next_token_.getTokenName() == "_") {
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			isArg1Valid = rel_table_.isArg1Valid("patternWhile", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		isArg1Valid = rel_table_.isArg1Valid("patternWhile", "_");
		arg1Type = "any";
	}
	else if (arg1.first == STRING) {
		isArg1Valid = rel_table_.isArg1Valid("patternWhile", "string");
		arg1Type = "string";
	}
	else if (arg2.first == UNDERSCORE) {
		isArg2Valid = rel_table_.isArg2Valid("patternWhile", "_");
		arg2Type = "any";
	}

	if (isArg1Valid && isArg2Valid) {
		vector<string> patternArg({ syn, arg1.second, arg2.second });
		vector<string> patternArgType({ synType, arg1Type,arg2Type });
		Clause patternClause("pattern", patternArg, patternArgType);
		_patternClauses.push_back(patternClause);
		query_table_.setPatternClause(_patternClauses);
	}
}

void QueryValidator::matchPatternIf() {
	// if : synonym (varRef, "_", "_")
	// varRef : synonym | "_" | "IDENT"
	string syn = next_token_.getTokenName();
	string synType;
	pair<int, string> arg1, arg2;
	// Check the syn to entity map and verify if it is "assign" or not. If NOT, ERROR!!!!!!!!!!!!!!!!
	if (syn_to_entity_map_[syn] == "if") {
		synType = syn_to_entity_map_[syn];
		_synToUseCountMap[syn] += 1;
		match(syn);
		match("(");
		arg1 = matchEntRef();
		match(",");
		if (next_token_.getTokenName() == "_") {
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			isArg1Valid = rel_table_.isArg1Valid("patternIf", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		isArg1Valid = rel_table_.isArg1Valid("patternIf", "_");
		arg1Type = "any";
	}
	else if (arg1.first == STRING) {
		isArg1Valid = rel_table_.isArg1Valid("patternIf", "string");
		arg1Type = "string";
	}
	else if (arg2.first == UNDERSCORE) {
		isArg2Valid = rel_table_.isArg2Valid("patternIf", "_");
		arg2Type = "any";
	}

	if (isArg1Valid && isArg2Valid) {
		vector<string> patternArg({ syn, arg1.second, arg2.second });
		vector<string> patternArgType({ synType, arg1Type,arg2Type });
		Clause patternClause("pattern", patternArg, patternArgType);
		_patternClauses.push_back(patternClause);
		query_table_.setPatternClause(_patternClauses);
	}
}

pair<int,string> QueryValidator::matchFactor() {
	if (next_token_.getTokenType() == IDENT || next_token_.getTokenType() == INTEGER || next_token_.getTokenType() == UNDERSCORE) {
		string factor = next_token_.getTokenName();
		int tokenType = next_token_.getTokenType();
		match(factor);
		return pair<int,string>(tokenType,factor);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.getTokenName() + "'; Expected a factor"));
	}
}

void QueryValidator::matchRelation() {
	string relation = next_token_.getTokenName();
	next_token_ = getToken();
	if (next_token_.getTokenType() == STAR) {
		relation += "*";
		next_token_ = getToken();
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
		throw(QueryException("Invalid Query : Unexpected relation '" + next_token_.getTokenName() + "'; Expecting 'Follow'|'Follow*'|'Parent'|'Parent*'|'Modifies'|'Uses'"));
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = rel_table_.isArg1Valid("follows", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = rel_table_.isArg1Valid("follows", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = rel_table_.isArg1Valid("follows", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = rel_table_.isArg2Valid("follows", syn_to_entity_map_[arg2.second]);
			arg2Type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = rel_table_.isArg2Valid("follows", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = rel_table_.isArg2Valid("follows", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> followsArg({ arg1.second,arg2.second });
		vector<string> followsArgType ({ arg1Type,arg2Type });
		Clause followsRelation("follows", followsArg, followsArgType);
		_suchThatClauses.push_back(followsRelation);
		query_table_.setSuchThatClause(_suchThatClauses);
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = rel_table_.isArg1Valid("follows*", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = rel_table_.isArg1Valid("follows*", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = rel_table_.isArg1Valid("follows*", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = rel_table_.isArg2Valid("follows*", syn_to_entity_map_[arg2.second]);
			arg2Type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = rel_table_.isArg2Valid("follows*", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = rel_table_.isArg2Valid("follows*", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> followsStarArg({ arg1.second,arg2.second });
		vector<string> followsStarArgType({ arg1Type,arg2Type });
		Clause followsStarRel("follows*", followsStarArg, followsStarArgType);
		_suchThatClauses.push_back(followsStarRel);
		query_table_.setSuchThatClause(_suchThatClauses);
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = rel_table_.isArg1Valid("parent", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = rel_table_.isArg1Valid("parent", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = rel_table_.isArg1Valid("parent", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = rel_table_.isArg2Valid("parent", syn_to_entity_map_[arg2.second]);
			arg2Type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = rel_table_.isArg2Valid("parent", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = rel_table_.isArg2Valid("parent", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> parentArg({ arg1.second,arg2.second });
		vector<string> parentArgType({ arg1Type,arg2Type });
		Clause parentClause("parent", parentArg, parentArgType);
		Clause parentRel("parent", parentArg, parentArgType);
		_suchThatClauses.push_back(parentRel);
		query_table_.setSuchThatClause(_suchThatClauses);
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = rel_table_.isArg1Valid("parent*", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = rel_table_.isArg1Valid("parent*", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = rel_table_.isArg1Valid("parent*", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg2Valid = rel_table_.isArg2Valid("parent*", syn_to_entity_map_[arg2.second]);
			arg2Type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = rel_table_.isArg2Valid("parent*", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = rel_table_.isArg2Valid("parent*", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> parentStarArg({ arg1.second,arg2.second });
		vector<string> parentStarArgType({ arg1Type,arg2Type });
		Clause parentStarRel("parent*", parentStarArg, parentStarArgType);
		_suchThatClauses.push_back(parentStarRel);
		query_table_.setSuchThatClause(_suchThatClauses);
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = rel_table_.isArg1Valid("modifies", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = rel_table_.isArg1Valid("modifies", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = rel_table_.isArg1Valid("modifies", "constant");
		arg1Type = "constant";
	}
	// arg 1 is a procedure e.g. Modifies("First", "x")
	else if (arg1.first == STRING) {
		arg1Valid = rel_table_.isArg1Valid("modifies", "string");
		arg1Type = "string";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = rel_table_.isArg2Valid("modifies", syn_to_entity_map_[arg2.second]);
			arg2Type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = rel_table_.isArg2Valid("modifies", "_");
		arg2Type = "any";
	}
	else if (arg2.first == STRING) {
		arg2Valid = rel_table_.isArg2Valid("modifies", "string");
		arg2Type = "string";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> modifiesArg({ arg1.second,arg2.second });
		vector<string> modifiesArgType({ arg1Type,arg2Type });
		Clause modifiesRel("modifies", modifiesArg, modifiesArgType);
		_suchThatClauses.push_back(modifiesRel);
		query_table_.setSuchThatClause(_suchThatClauses);
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = rel_table_.isArg1Valid("uses", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = rel_table_.isArg1Valid("uses", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = rel_table_.isArg1Valid("uses", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = rel_table_.isArg2Valid("uses", syn_to_entity_map_[arg2.second]);
			arg2Type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = rel_table_.isArg2Valid("uses", "_");
		arg2Type = "any";
	}
	else if (arg2.first == STRING) {
		arg2Valid = rel_table_.isArg2Valid("uses", "string");
		arg2Type = "string";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> usesArg({ arg1.second,arg2.second });
		vector<string> usesArgType({ arg1Type,arg2Type });
		Clause usesRel("uses", usesArg, usesArgType);
		_suchThatClauses.push_back(usesRel);
		query_table_.setSuchThatClause(_suchThatClauses);
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = rel_table_.isArg1Valid("calls", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = rel_table_.isArg1Valid("calls", "_");
		arg1Type = "any";
	}
	else if (arg1.first == STRING) {
		arg1Valid = rel_table_.isArg1Valid("calls", "string");
		arg1Type = "string";
	}
	/* call rel does not accept int as parameter
	else if (arg1.first == INTEGER) {
		arg1Valid = rel_table_.isArg1Valid("calls", "constant");
		arg1Type = "constant";
	}
	*/
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = rel_table_.isArg2Valid("calls", syn_to_entity_map_[arg2.second]);
			arg2Type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = rel_table_.isArg2Valid("calls", "_");
		arg2Type = "any";
	}
	else if (arg2.first == STRING) {
		arg2Valid = rel_table_.isArg2Valid("calls", "string");
		arg2Type = "string";
	}
	/* call rel does not accept int as parameter
	else if (arg2.first == INTEGER) {
		arg2Valid = rel_table_.isArg2Valid("calls", "constant");
		arg2Type = "constant";
	}
	*/
	if (arg1Valid && arg2Valid) {
		vector<string> callsArg({ arg1.second,arg2.second });
		vector<string> callsArgType({ arg1Type,arg2Type });
		Clause callsRel("calls", callsArg, callsArgType);
		_suchThatClauses.push_back(callsRel);
		query_table_.setSuchThatClause(_suchThatClauses);
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = rel_table_.isArg1Valid("calls*", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = rel_table_.isArg1Valid("calls*", "_");
		arg1Type = "any";
	}
	else if (arg1.first == STRING) {
		arg1Valid = rel_table_.isArg1Valid("calls*", "string");
		arg1Type = "string";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg2Valid = rel_table_.isArg2Valid("calls*", syn_to_entity_map_[arg2.second]);
			arg2Type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = rel_table_.isArg2Valid("calls*", "_");
		arg2Type = "any";
	}
	else if (arg2.first == STRING) {
		arg2Valid = rel_table_.isArg2Valid("calls*", "string");
		arg2Type = "string";
	}
	if (arg1Valid && arg2Valid) {
		vector<string> callsStarArg({ arg1.second,arg2.second });
		vector<string> callsStarArgType({ arg1Type,arg2Type });
		Clause callsStarRel("calls*", callsStarArg, callsStarArgType);
		_suchThatClauses.push_back(callsStarRel);
		query_table_.setSuchThatClause(_suchThatClauses);
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = rel_table_.isArg1Valid("next", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = rel_table_.isArg1Valid("next", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = rel_table_.isArg1Valid("next", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = rel_table_.isArg2Valid("next", syn_to_entity_map_[arg2.second]);
			arg2Type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = rel_table_.isArg2Valid("next", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = rel_table_.isArg2Valid("next", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> nextArg({ arg1.second,arg2.second });
		vector<string> nextArgType({ arg1Type,arg2Type });
		Clause nextRel("next", nextArg, nextArgType);
		_suchThatClauses.push_back(nextRel);
		query_table_.setSuchThatClause(_suchThatClauses);
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
		if (syn_to_entity_map_[arg1.second] != "") {
			_synToUseCountMap[arg1.second] += 1;
			arg1Valid = rel_table_.isArg1Valid("next*", syn_to_entity_map_[arg1.second]);
			arg1Type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		arg1Valid = rel_table_.isArg1Valid("next*", "_");
		arg1Type = "any";
	}
	else if (arg1.first == INTEGER) {
		arg1Valid = rel_table_.isArg1Valid("next*", "constant");
		arg1Type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			_synToUseCountMap[arg2.second] += 1;
			arg2Valid = rel_table_.isArg2Valid("next*", syn_to_entity_map_[arg2.second]);
			arg2Type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		arg2Valid = rel_table_.isArg2Valid("next*", "_");
		arg2Type = "any";
	}
	else if (arg2.first == INTEGER) {
		arg2Valid = rel_table_.isArg2Valid("next*", "constant");
		arg2Type = "constant";
	}

	if (arg1Valid && arg2Valid) {
		vector<string> nextStarArg({ arg1.second,arg2.second });
		vector<string> nextStarArgType({ arg1Type,arg2Type });
		Clause nextStarRel("next*", nextStarArg, nextStarArgType);
		_suchThatClauses.push_back(nextStarRel);
		query_table_.setSuchThatClause(_suchThatClauses);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for nextStar"));
	}
}

pair<int,string> QueryValidator::matchStmtRef() {
	// stmtRef : synonym | ‘_’ | INTEGER
	int tokenType;
	string argument;
	if (next_token_.getTokenType() == IDENT) {
		argument = next_token_.getTokenName();
		tokenType = next_token_.getTokenType();
		match(argument);
	}
	else if (next_token_.getTokenName() == "_") {
		argument = "_";
		tokenType = next_token_.getTokenType();
		match(UNDERSCORE);
	}
	else if (next_token_.getTokenType() == INTEGER) {
		argument = next_token_.getTokenName();
		tokenType = next_token_.getTokenType();
		match(argument);
	}
	else {
		// Invalid Query
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.getTokenName() + "'; Expected stmtRef token"));
	}
	return pair<int, string>(tokenType, argument);
}

pair<int,string> QueryValidator::matchEntRef() {
	// entRef : synonym | ‘_’ | ‘"’ IDENT ‘"’ | INTEGER
	string argument;
	int tokenType;
	if (next_token_.getTokenType() == IDENT) {
		// synonym
		argument = next_token_.getTokenName();
		tokenType = next_token_.getTokenType();
		match(argument);
	}
	else if (next_token_.getTokenName() == "_") {
		// '_'
		argument = "_";
		tokenType = next_token_.getTokenType();
		match(argument);
	}
	else if (next_token_.getTokenName() == "\"") {
		// '"' IDENT '"'
		match("\"");
		argument = next_token_.getTokenName();
		tokenType = STRING;
		match(argument);
		// Some funny thing here
		match("\"");
	}
	else if (next_token_.getTokenType() == INTEGER) {
		argument = next_token_.getTokenName();
		tokenType = next_token_.getTokenType();
		match(argument);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.getTokenName() + "'; Expected entRef token"));
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