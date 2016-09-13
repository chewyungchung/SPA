#include "PQLVerifyTable.h"
#include "Clause.h"
#include "QueryTable.h"

using namespace std;

	QueryTable qt;	

	int intFormatSize;
	vector<string> unverifiedQuery;
	vector<string> allEntities;
	vector<string> allDeclarations;
	string strSelect;
	string strSuch;
	string strThat;
	string strPattern;

	vector<Clause> allClauses;
	string clauseRelation;
	vector<string> arg;
	vector<string> argType;



PQLVerifyTable::PQLVerifyTable(vector<vector<string>> declaration, vector<string> entities)
{
	strSelect = "select";
	allEntities= addBoolean(entities);
	strSuch = "such";
	strThat = "that";
	strPattern="pattern";
	allDeclarations = extractDeclaration(declaration);
}
bool PQLVerifyTable::check(int formatSize , vector<string> unverifiedQuery)
{	
	if(ensureSelectSomethingExist(unverifiedQuery[0],unverifiedQuery[1],allEntities)==false){return false;}
	if(formatSize==9){// (pattern + such that) or (such that +pattern)
		if((suchThat(unverifiedQuery,2,3,4,5) && pattern(unverifiedQuery,6,7,8)))
		{
			qt.setIsSuchThatAvail(true);
			qt.setIsPatternAvail(true);
			return true;
		}
		else if(pattern(unverifiedQuery,2,3,4) && suchThat(unverifiedQuery,5,6,7,8))
		{
			qt.setIsPatternAvail(true);
			qt.setIsSuchThatAvail(true);
			return true;
		}
		return false;
	}
	else if(formatSize==6){ // such that
		if(suchThat(unverifiedQuery,2,3,4,5))
		{
			qt.setIsSuchThatAvail(true);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(formatSize==5){ //pattern

		if(pattern(unverifiedQuery,2,3,4))
		{
			qt.setIsPatternAvail(true);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(formatSize==2){ //Select something.
		return true;
	}
	else //dosent exist
	{
		return false;
	}
}

	//										 ensureSelectSomethingExist()
	//										             |
	//	------------------suchThat()------------------------------------------------pattern()-------------
	//	|        |            |             |                     			       |        |            | 
	//isSuch()  isThat() isValidClause()   isValidContent()             isPattern()  isEntity() isValidPattern() 
bool PQLVerifyTable::ensureSelectSomethingExist(string selectStatement,string someVariable, vector<string> allEntitiesDeclarations)
{
	transform(selectStatement.begin(), selectStatement.end(), selectStatement.begin(), ::tolower);

	for(int x=0;x<allEntitiesDeclarations.size();x++)
	{
		if(selectStatement == strSelect && someVariable == allEntitiesDeclarations[x])
		{
			return true;
		}
	}
	return false;

}

	//	 ---------suchThat()----------------- create clause here---------------------------
	//	 |        |            |            |
	//isSuch()  isThat() isValidClause() isValidContent ()
bool PQLVerifyTable::suchThat(vector<string> unverifiedQuery, int intSuchPos, int intThatPos, int intClausePos, int intBracketContentPos)
{
	if(isSuch(unverifiedQuery[intSuchPos]) && isThat(unverifiedQuery[intThatPos]) && isValidClause(unverifiedQuery[intClausePos]) && isValidContent(unverifiedQuery[intClausePos],unverifiedQuery[intBracketContentPos]))
	{
		clauseRelation = unverifiedQuery[intClausePos]; //clause name
		allClauses.push_back(Clause(clauseRelation,arg,argType));
		clauseRelation="";
		arg.clear();
		argType.clear();
		return true;
	}
	return false;
}

	//isSuch()  isThat() isValidClause() isValidContent() clauseContentCheck()
bool PQLVerifyTable::isSuch(string content)
{
	transform(content.begin(), content.end(), content.begin(), ::tolower);
	if(content==strSuch)
	{
		return true;
	}
	return false;
}
bool PQLVerifyTable::isThat(string content)
{
	transform(content.begin(), content.end(), content.begin(), ::tolower);
	if(content==strThat)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool PQLVerifyTable::isValidClause(string content)
{
	int numOfClauses=6;
	string clauses[6] = { "follows", "follows*", "parent","parent*", "modifies","uses" };//replace with hashmap, remove for loop
	transform(content.begin(), content.end(), content.begin(), ::tolower);//lower case everything

	for(int x=0;x<numOfClauses;x++)
	{
		if(content==clauses[x])
		{
			return true;
		}
	}
	return false;
}
bool PQLVerifyTable::isValidContent(string clause,string content)
{
	if(bracketCheck(content) && briefContentCheck(content) &&clausesBracketContentCheck(clause,content))
	{
		return true;
	}
	else
	{
		return false;
	}

}

	//   ---------pattern()----------create clause here------------------------------------
	//	|			 |	            |				
	//isPattern()  isEntity() isValidPattern() 
bool PQLVerifyTable::pattern(vector<string> unverifiedQuery, int intPatternPos, int intEntityPos, int intPatternContentPos)
{
	if(isPattern(unverifiedQuery[intPatternPos]) && isEntity(unverifiedQuery[intEntityPos]) && isValidPattern(unverifiedQuery[intPatternContentPos]))
	{
		clauseRelation = unverifiedQuery[intPatternPos]; //clause name
		allClauses.push_back(Clause(clauseRelation,arg,argType));
		cout<<"new clause created";
		clauseRelation="";
		arg.clear();
		argType.clear();
		return true;
	}
	return false;
}

	//isPattern()  isEntity() isValidPattern() 
bool PQLVerifyTable::isPattern(string content)
{
	transform(content.begin(), content.end(), content.begin(), ::tolower);
	if(content==strPattern)
	{
		return true;
	}
	return false;
	
}
bool PQLVerifyTable::isEntity(string content)
{
	for(int x=0;x<allEntities.size();x++)
	{
		if(content==allEntities[x])
		{
			return true;
		}
	}
	return false;
}
bool PQLVerifyTable::isValidPattern(string content)
{
	if(bracketCheck(content) && briefContentCheck(content) && patternBracketContentCheck(content) )
	{
			return true;

	}
	else
	{
		return false;
	}

}

	//miscellaneous methods
vector<string> PQLVerifyTable::addBoolean(vector<string> entities)
{
	entities.push_back("BOOLEAN");
	return entities;
}
vector<string> PQLVerifyTable::extractDeclaration(vector<vector<string>> declaration)
{
	vector<string> result;
	for(int x=0;x<declaration.size();x++)
	{
		result.push_back(declaration[x][0]); 
		
	}
	//this will print: stmt assign while if procedure variable constant prog_line
	//for(int x=0;x<declaration.size();x++)
	//{
	//	cout<<declaration[x][0];
	//  cout<<"\n";
	//}
	return result;
}
string PQLVerifyTable::removeSpaces(string content)
{
	content.erase(remove(content.begin(), content.end(), ' '), content.end());
	return content;
}
string PQLVerifyTable::removeFrontAndBack(string content)
{
	content.erase(remove(content.begin(), content.end(), ' '), content.end());	//remove spaces 
	content.pop_back();
	content = content.substr(1).append("");
	return content;
}
vector<string> PQLVerifyTable::split(string str, char delimiter) {
	 vector< string> internal;
	 stringstream ss(str); // Turn the string into a stream.
	 string tok;

	while ( getline(ss, tok, delimiter))
	{
		internal.push_back(tok);
	}
	return internal;
}

	//check methods
bool PQLVerifyTable::briefContentCheck(string content)//check bracket content size and number of commas
{	
	content = removeSpaces(content);
	int minContentSizeInBracket=5;
	int numberOfCommas=	count(content.begin(), content.end(), ',');

	//check if content is correct size of at least 5 (a,b) ---[(] [a] [,] [b] [)]
	//check if number of commas has only 1
	if(content.size()<minContentSizeInBracket || numberOfCommas>1)
	{
		return false;
	}

	//check is input is a string
	

	return true;

}
bool PQLVerifyTable::bracketCheck(string content)
{
	string openBracket = "(";
	string closeBracket = ")";
	
	if(content.substr(0,1)==openBracket && content.substr(content.size()-1,1)==closeBracket)
	{
		return true;
	}
	return false;
}
bool PQLVerifyTable::clausesBracketContentCheck(string clause,string content)
{
	transform(clause.begin(), clause.end(), clause.begin(), ::tolower);
	transform(content.begin(), content.end(), content.begin(), ::tolower);

	//1-(stmtRef,stmtRef) 2-(stmtRef,entRef) 
	vector<pair<string,int>> vectorClauseID;
	vectorClauseID.push_back(make_pair("modifies",2));
	vectorClauseID.push_back(make_pair("uses",2));
	vectorClauseID.push_back(make_pair("parent",1));
	vectorClauseID.push_back(make_pair("parent*",1));
	vectorClauseID.push_back(make_pair("follows",1));
	vectorClauseID.push_back(make_pair("follows*",1));

	content = removeSpaces(content);
	content = removeFrontAndBack(content);
	if(content.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_“”,") != string::npos)//if found unknown char return false
	{
			return false;
	}	
	vector< string> sep = split(content, ',');// cfm is 2 cos check earlier in briefContentCheck()
	for(int x=0;x<vectorClauseID.size();x++)
	{
		if(vectorClauseID[x].first==clause)
		{
			if(vectorClauseID[x].second==1)//1-(stmtRef,stmtRef)
			{
				if(stmtRefCheck(sep[0]) && stmtRefCheck(sep[1]))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else if(vectorClauseID[x].second==2)//2-(stmtRef,entRef) 
			{
				if(entRefCheck(sep[0]) && entRefCheck(sep[1]))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return false;
}
bool PQLVerifyTable::patternBracketContentCheck(string content)
{
	transform(content.begin(), content.end(), content.begin(), ::tolower);
	content = removeSpaces(content);
	content = removeFrontAndBack(content);
	if(content.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_“”,") != string::npos)//if found unknown char return false
	{
			return false;
	}	
	vector< string> sep = split(content, ',');// cfm is 2 cos check earlier in briefContentCheck()
	if(entRefCheck(sep[0]) && patternExpressionCheck(sep[1]))
	{
		
		return true;
	}
	else
	{
		return false;
	}
}

bool PQLVerifyTable::stmtRefCheck(string content)
{
	for(int x=0;x<allEntities.size();x++)
	{
		if(content==allEntities[x])
		{
			arg.push_back(content);
			argType.push_back("synonym");
			cout<<" synonym ";
			return true;
		}
	}
	if(content=="_")
	{
		arg.push_back(content);
		argType.push_back("underscore");
		cout<<" underscore ";
		return true;
	}
	const char *cstr = content.c_str();
	if(isdigit(*cstr))
	{
		arg.push_back(content);
		argType.push_back("integer");
		cout<<" integer ";
		return true;
	}
	return false;
}
bool PQLVerifyTable::entRefCheck(string content)
{
	for(int x=0;x<allEntities.size();x++)
	{
		if(content==allEntities[x])
		{
			arg.push_back(content);
			argType.push_back("synonym");
			cout<<" synonym ";
			return true;
		}
	}
	if(content=="_")
	{
		arg.push_back(content);
		argType.push_back("underscore");
		cout<<" underscore ";
		return true;
	}
	if(content.substr(0,1)=="“" && content.substr(content.size()-1, 1) == "”")
	{
		content.pop_back();
		content = content.substr(1).append("");
		const char *cstr = content.c_str();
		cout<<" ident-";

		if(content.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"))
		{
			arg.push_back(content);
			argType.push_back("letter");
			cout<<"letter ";
			return true;
		}
		if(content.size()==1 && isdigit(*cstr))
		{
			arg.push_back(content);
			argType.push_back("digit");
			cout<<" digit ";
			return true;
		}

	}
	return false;
}
bool PQLVerifyTable::patternExpressionCheck(string content)
{
	if(content=="_")
	{
		arg.push_back(content);
		argType.push_back("underscore");
		cout<<" underscore ";
		return true;
	}
	if(content.substr(0,1)=="“" && content.substr(content.size()-1, 1) == "”")
	{
		content.pop_back();
		content = content.substr(1).append("");
		const char *cstr = content.c_str();

		if(content.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"))
		{
			arg.push_back(content);
			argType.push_back("letter");
			cout<<"letter ";
			return true;
		}
		if(content.size()==1 && isdigit(*cstr))
		{
			arg.push_back(content);
			argType.push_back("digit");
			cout<<" digit ";
			return true;
		}
	}
	if(content.substr(0,1)=="_" && content.substr(content.size()-1, 1) == "_")
	{
		
		content = removeFrontAndBack(removeFrontAndBack(content));
		const char *cstr = content.c_str();
		cout<<" ident-";

		if(content.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"))
		{
			//for(int x=0;x<allEntities.size();x++)
			//{
				//if(content==allEntities[x])
				//{
				//	cout<<"variable name ";
				//	return true;
				//}
			//}
			arg.push_back(content);
			argType.push_back("variable");
			cout<<" variable ";
			return true;
		}
		if(isdigit(*cstr))
		{
			arg.push_back(content);
			argType.push_back("digit");
			cout<<" digit";
			return true;
		}

	}
	return true;
}

void PQLVerifyTable::getClausesList()
{
	cout<<allClauses.size();
}

