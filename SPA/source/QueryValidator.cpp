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
		if (synTypeAndAttrNameMatches(result_syn_type, attribute_type) == true) {
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
	matchAttrCompare();
	while (next_token_.getTokenName() == "and") {
		match("and");
		matchAttrCompare();
	}
}

void QueryValidator::matchAttrCompare()
{
	vector<string> with_arg, with_arg_type;
	Ref left_ref = matchRef();
	match("=");
	Ref right_ref = matchRef();

	if (IsRefCompatible(left_ref, right_ref) == true) {
		with_arg.push_back(left_ref.value);
		with_arg.push_back(right_ref.value);
		with_arg_type.push_back(left_ref.arg_type);
		with_arg_type.push_back(right_ref.arg_type);
		Clause with_clause("with", with_arg, with_arg_type);
		query_table_.AddWithClause(with_clause);
	}
	else {
		throw(QueryException("Invalid Query : Mismatched Ref '" + left_ref.value + "' and '" + right_ref.value + "' in Pattern clause"));
	}
}

QueryValidator::Ref QueryValidator::matchRef()
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
				Ref with_ref;
				with_ref.value = with_syn;
				with_ref.arg_type = "prog_line";
				with_ref.with_type = "number";
				return with_ref;
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
			Ref with_ref;
			with_ref.value = with_syn;
			if (with_syn_type == "call") {
				if (attrName == "procName") {
					with_ref.arg_type = "call_name";
					with_ref.with_type = "name";
					return with_ref;
				}
				else if (attrName == "stmt#") {
					with_ref.arg_type = "call_number";
					with_ref.with_type = "number";
					return with_ref;
				}
			}
			
			if (synTypeAndAttrNameMatches(with_syn_type, attrName) == true) {
				with_ref.arg_type = syn_to_entity_map_[with_syn];
				if (with_syn_type == "constant") {
					with_ref.arg_type = "value";
				}
				else {
					with_ref.with_type = getWithTypeByAttrName(attrName);
				}
				return with_ref;
			}
			else {
				throw(QueryException("Invalid Query : Unexpected attibute '" + attrName + "' for synonym :'" + with_syn + "'"));
			}
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
		Ref with_ref;
		with_ref.value = argument;
		with_ref.arg_type = "string";
		with_ref.with_type = "name";
		return with_ref;
	}
	else if (next_token_.getTokenType() == INTEGER) {
		argument = next_token_.getTokenName();
		tokenType = next_token_.getTokenType();
		match(argument);
		Ref with_ref;
		with_ref.value = argument;
		with_ref.arg_type = "number";
		with_ref.with_type = "number";
		return with_ref;
		return with_ref;
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.getTokenName() + "'; Expected Ref token"));
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

bool QueryValidator::IsRefCompatible(Ref left_ref, Ref right_ref)
{
	if (left_ref.with_type == "number" && right_ref.with_type == "number") {
		return true;
	}
	else if (left_ref.with_type == "name" && right_ref.with_type == "name") {
		return true;
	}
	return false;
}

string QueryValidator::getWithTypeByAttrName(string attrName)
{
	if (attrName == "varName" || attrName == "procName") {
		return "name";
	}
	else if (attrName == "value" || attrName == "stmt#") {
		return "number";
	}
}

void QueryValidator::matchPatternAssign()
{
	string assign_syn = next_token_.getTokenName();
	string assign_var_ref_syn, assign_var_ref_type, expression_type;
	pair<int, string> assign_var_ref;
	bool is_sub_expr = false;
	bool is_any = false;
	bool is_assign_var_ref_valid = false;

	if (syn_to_entity_map_[assign_syn] == "assign") {
		match(assign_syn);
		match("(");
		assign_var_ref = matchVarRef();
		match(",");
		if (next_token_.getTokenType() == UNDERSCORE) {
			match("_");
			is_any = true;
			expression_string_ = "_";
			if (next_token_.getTokenType() == DOUBLE_QUOTE) {
				is_sub_expr = true;
				is_any = false;
				match("\"");
				expression_string_ = "";
				matchExpr();
				match("\"");
				match("_");
			}
		}
		else if (next_token_.getTokenType == DOUBLE_QUOTE) {
			match("\"");
			matchExpr();
			match("\"");
		}

		if (assign_var_ref.first == IDENT) {
			if (syn_to_entity_map_[assign_var_ref.second] != "") {
				assign_var_ref_type = syn_to_entity_map_[assign_var_ref.second];
				is_assign_var_ref_valid = rel_table_.isArg1Valid("patternAssign", assign_var_ref_type);
			}
		}
		else if (assign_var_ref.first == UNDERSCORE) {
			is_assign_var_ref_valid = true;
			assign_var_ref_type = "any";
		}
		else if (assign_var_ref.first == STRING) {
			is_assign_var_ref_valid = true;
			assign_var_ref_type = "string";
		}

		if (is_assign_var_ref_valid == true) {
			if (is_sub_expr == true) {
				expression_type = "sub_expr";
			} 
			else if (is_any == true) {
				expression_type = "any";
			}
			else {
				expression_type = "expr";
			}
			vector<string> assign_arg = { assign_syn, assign_var_ref.second , expression_string_ };
			vector<string> assign_arg_type = { "assign", assign_var_ref_type, expression_type };
			Clause pattern_assign_clause("pattern", assign_arg, assign_arg_type);
			query_table_.AddPatternClause(pattern_assign_clause);
		}
		else {
			throw(QueryException("Invalid Query : Unexpected varRef for pattern-assign '" + assign_var_ref_type + "'"));
		}
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.getTokenName() + "'; Expected valid assign synonym"));
	}
}

void QueryValidator::matchPatternWhile() {
	// while : synonym (varRef ,"_")
	// varRef : synonym | "_" | "IDENT"
	string while_syn = next_token_.getTokenName();
	string while_control_var_type;
	bool is_control_variable_valid = false;
	pair<int, string> while_control_variable;
	if (syn_to_entity_map_[while_syn] == "while") {
		match(while_syn);
		match("(");
		while_control_variable = matchVarRef();
		match(",");
		match("_");
		match(")");

		if (while_control_variable.first == IDENT) {
			if (syn_to_entity_map_[while_control_variable.second] != "") {
				while_control_var_type = syn_to_entity_map_[while_control_variable.second];
				is_control_variable_valid = rel_table_.isArg1Valid("patternWhile", while_control_var_type);
			}
		}
		else if (while_control_variable.first == UNDERSCORE) {
			is_control_variable_valid = true;
			while_control_var_type = "any";
		}
		else if (while_control_variable.first == STRING) {
			is_control_variable_valid = true;
			is_control_variable_valid = "string";
		}

		if (is_control_variable_valid == true) {
			vector<string> while_arg = { while_syn, while_control_variable.second };
			vector<string> while_arg_type = { "while", while_control_var_type };
			Clause pattern_while_clause("while", while_arg, while_arg_type);
			query_table_.AddPatternClause(pattern_while_clause);
		}
		else {
			throw(QueryException("Invalid Query : Unexpected control variable for pattern-while '" + while_control_var_type + "'"));
		}
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.getTokenName() + "'; Expected valid while synonym"));
	}
}

void QueryValidator::matchPatternIf() {
	// if : synonym (varRef, "_", "_")
	// varRef : synonym | "_" | "IDENT"
	string if_syn = next_token_.getTokenName();
	string if_control_var_type;
	bool is_control_variable_valid = false;
	pair<int, string> if_control_variable;
	if (syn_to_entity_map_[if_syn] == "if") {
		match(if_syn);
		match("(");
		if_control_variable = matchVarRef();
		match(",");
		match("_");
		match(",");
		match("_");
		match(")");

		if (if_control_variable.first == IDENT) {
			if (syn_to_entity_map_[if_control_variable.second] != "") {
				if_control_var_type = syn_to_entity_map_[if_control_variable.second];
				is_control_variable_valid = rel_table_.isArg1Valid("patternIf", if_control_var_type);
			}
		}
		else if (if_control_variable.first == UNDERSCORE) {
			is_control_variable_valid = true;
			if_control_var_type = "any";
		}
		else if (if_control_variable.first == STRING) {
			is_control_variable_valid = true;
			is_control_variable_valid = "string";
		}

		if (is_control_variable_valid == true) {
			vector<string> if_arg = { if_syn, if_control_variable.second };
			vector<string> if_arg_type = { "if", if_control_var_type };
			Clause pattern_if_clause("if", if_arg, if_arg_type);
			query_table_.AddPatternClause(pattern_if_clause);
		}
		else {
			throw(QueryException("Invalid Query : Unexpected control variable for pattern-if '" + if_control_var_type + "'"));
		}
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.getTokenName() + "'; Expected valid if synonym"));
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
		query_table_.AddSuchThatClause(followsRelation);

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
		query_table_.AddSuchThatClause(followsStarRel);
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
		query_table_.AddSuchThatClause(parentRel);
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
		query_table_.AddSuchThatClause(parentStarRel);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for parent*"));
	}
}

void QueryValidator::matchModifies() {
	match("(");
	pair<int,string> arg1 = matchEntRef();
	match(",");
	pair<int, string> arg2 = matchEntRef();
	match(")");
	
	int arg1Valid = -1;
	int arg2Valid = -1;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
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
	else if (arg1.first == STRING) {
		arg1Valid = rel_table_.isArg1Valid("modifies", "string");
		arg1Type = "string";
	}

	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
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
	match("(");
	pair<int,string> arg1 = matchEntRef();
	match(",");
	pair<int,string> arg2 = matchEntRef();
	match(")");

	// Validate arg1 and arg2
	int arg1Valid = -1;
	int arg2Valid = -1;
	string arg1Type, arg2Type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
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
	else if (arg1.first == STRING) {
		arg1Valid = rel_table_.isArg1Valid("uses", "string");
		arg1Type = "string";
	}

	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
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
		query_table_.AddSuchThatClause(callsRel);
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
		query_table_.AddSuchThatClause(callsStarRel);
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
		query_table_.AddSuchThatClause(nextRel);
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
		query_table_.AddSuchThatClause(nextStarRel);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for nextStar"));
	}
}

pair<int,string> QueryValidator::matchExpr()
{
	return pair<int, string>();
}

pair<int,string> QueryValidator::matchSubExpr()
{
	return pair<int, string>();
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

pair<int, string> QueryValidator::matchVarRef()
{
	// entRef : synonym | ‘_’ | ‘"’ IDENT ‘"’
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
		match("\"");
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.getTokenName() + "'; Expected entRef token"));
	}

	return pair<int, string>(tokenType, argument);
}