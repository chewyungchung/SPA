#include <iostream>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>
#include <list>
#include <iostream>
#include "PQLParser.h"
#include "Clause.h"
#include "PQLVerifyTable.h"

void PQLParser::Start(string declaration, vector<string> queries)
{
	entityTable = declarationAnalyzer(declaration);

	for (int x = 0; x < queries.size(); x++)
	{
		string stringQuery = queries[x];
		queryAnalyzer(stringQuery);
		unverifyQuery.push_back(tempResultQuery);
		tempResultQuery.clear();
	}

	vector<string> allEntities;//list of all entites use in the declaration
	for (int x = 0; x < entityTable.size(); x++)
	{
		for (int y = 1; y < entityTable[x].size(); y++)
		{
			allEntities.push_back(entityTable[x][y]);
		}
	}

	//printEntityTable(entityTable);
	//printQueryTable(unverifyQuery);
	vector<Clause> clausesVector;
	verifyQuery(unverifyQuery, entityTable, allEntities, clausesVector);

	getchar();
}
void PQLParser::printEntityTable(vector<vector<string>> entityTable)
{
	cout << "\n";

	cout << "This is entity table [with declarations verifed] in arraylist of arraylist \n";

	cout << "\n";

	for (int x = 0; x < entityTable.size(); x++)
	{
		for (int y = 0; y < entityTable[x].size(); y++)
		{
			cout << entityTable[x][y] + " ";
		}
		cout << "\n";
	}
	cout << "\n";

	cout << "This is entity table [with declarations verifed] in arraylist of arraylist \n";

	cout << "\n";
}
void PQLParser::printQueryTable(vector<vector<string>> unverifyQuery)
{
	cout << "\n";

	cout << "This is unverify query table in arraylist of arraylist \n";

	cout << "\n";

	for (int y = 0; y < unverifyQuery.size(); y++)
	{
		for (int x = 0; x < unverifyQuery[y].size(); x++)
		{
			cout << unverifyQuery[y][x] + "--";
		}
		cout << "\n";
		cout << "\n";

	}
	cout << "\n";

	cout << "This is unverify query table in arraylist of arraylist \n";

	cout << "\n";
}
pair<bool, vector<string>> PQLParser::suchThat(vector<string> currentQuery, vector<string> allEntities, int startPos)
{
	stringstream ss;
	ss << currentQuery.size();
	string size = ss.str();
	vector< string> sep;
	const char *clauses[] = { "Follows", "Follows*", "Parent","Parent*","Modifies","Uses" };


	if (currentQuery[startPos] == "such" && currentQuery[startPos + 1] == "that")
	{//CHECK IS SUCH THAT CLAUSE
		bool relationExist = false;
		for (int x = 0; x < 6; x++)
		{
			if (currentQuery[startPos + 2] == clauses[x]) //any of "Follows", "Follows*", "Parent","Parent*","Modifies","Uses"
			{

				removeSpaces(currentQuery[startPos + 3]);
				currentQuery[startPos + 3].pop_back();
				currentQuery[startPos + 3] = currentQuery[startPos + 3].substr(1).append("");
				vector< string> sep = split(currentQuery[startPos + 3], ',');//split by ","
				if (sep.size() > 2)
				{
					return make_pair(false, sep);
				}
				relationExist = true;
				if (sep[0].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_“”") != string::npos || sep[1].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_“”") != string::npos)
				{
					return make_pair(false, sep);
				}
				return make_pair(relationExist, sep);
			}
		}
		if (relationExist == false)
		{
			cout << "wrong query, relation don't exist\n";
			vector<string> sep;
			return make_pair(false, sep);
		}
	}
	else
	{
		cout << "wrong query, not such that\n";
		vector<string> sep;
		return make_pair(false, sep);
	}
}
pair<bool, vector<string>> PQLParser::pattern(vector<string> currentQuery, vector<string> allEntities, int startPos)
{
	stringstream ss;
	ss << currentQuery.size();
	string size = ss.str();

	//const char *types[] = { "Follows", "Follows*", "Parent","Parent*","Modifies","Uses" };

	if (currentQuery[startPos] == "pattern" && checkStringIsEntity(currentQuery[startPos + 1], allEntities))
	{//CHECK "PATTERN" AND IS ENTITY
		removeSpaces(currentQuery[startPos + 2]);
		currentQuery[startPos + 2].pop_back();
		currentQuery[startPos + 2] = currentQuery[startPos + 2].substr(1).append("");
		vector<string> sep = split(currentQuery[startPos + 2], ',');//split by ","
		if (sep.size() > 2)
		{
			return make_pair(false, sep);
		}
		sep.push_back(currentQuery[startPos + 1]); //example pattern a(x,y) a will be stored

		if (sep[0].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_“”") != string::npos || sep[1].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_“”") != string::npos)
		{

			return make_pair(false, sep);
		}
		return make_pair(true, sep);

	}
	else
	{
		cout << "wrong query, not pattern or no such entites\n";
		vector<string> sep;
		return make_pair(false, sep);

	}

}
string PQLParser::printWholeArray(vector<string> arrayOfString)
{
	string result;
	for (int x = 0; x < arrayOfString.size(); x++)
	{
		result += arrayOfString[x] + " ";
	}
	return result;
}
bool PQLParser::checkStringIsEntity(string input, vector<string> allEntities)
{
	if (input == "BOOLEAN")
	{
		return true;
	}
	for (int k = 0; k < allEntities.size(); k++)
	{
		if (input == allEntities[k])
		{
			return true;
		}
	}
	return false;
}
vector<string> PQLParser::split(string str, char delimiter) {
	vector< string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}
vector<vector<string>> PQLParser::declarationAnalyzer(string strDeclaration)
{

	//[0] procedure [1] stmtlst   [2] stmt     [3] while      [4] assign
	//[5] if        [6] variable  [7] constant

	//stmt s, s1;assign a, a1, a2; while w, w1; if ifstmt, iftstmt1; procedure p; variable v; constant c; prog_line n, n1, n2;
	vector<vector< string>> entityTable;
	vector< string> sep = split(strDeclaration, ';');//split by ";"
	for (int x = 0; x < sep.size(); x++)
	{
		istringstream buf(sep[x]);
		istream_iterator< string> beg(buf), end;//split by " "
		vector< string> tokens(beg, end);
		bool firstTime = true;

		for (int y = 1; y < tokens.size(); y++)
		{

			if (tokens[y].substr(tokens[y].size() - 1, 1) == ",")
			{
				tokens[y].pop_back();
			}
			if (tokens[y].find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_") != string::npos)
			{
				cerr << "ERROR WRONG DECLARATION";
			}
			// cout << "y :" + tokens[y] + "\n";
			if (tokens[0] == "procedure")
			{
				if (firstTime == true)
				{
					pdeProcedureList.push_back("procedure");
					entityTable.push_back(pdeProcedureList);
					firstTime = false;
				}
				pdeProcedureList.push_back(tokens[y]);
				entityTable[entityTable.size() - 1] = pdeProcedureList;

			}
			else if (tokens[0] == "stmtlst")
			{
				if (firstTime == true)
				{
					pdeStmtListList.push_back("stmtlst");
					entityTable.push_back(pdeStmtListList);

					firstTime = false;
				}

				pdeStmtListList.push_back(tokens[y]);
				entityTable[entityTable.size() - 1] = pdeStmtListList;

			}
			else if (tokens[0] == "stmt")
			{
				if (firstTime == true)
				{
					pdeStmtList.push_back("stmt");
					entityTable.push_back(pdeStmtList);
					firstTime = false;
				}

				pdeStmtList.push_back(tokens[y]);
				entityTable[entityTable.size() - 1] = pdeStmtList;

			}
			else if (tokens[0] == "assign")
			{
				if (firstTime == true)
				{
					pdeAssignList.push_back("assign");
					entityTable.push_back(pdeAssignList);
					firstTime = false;
				}

				pdeAssignList.push_back(tokens[y]);
				entityTable[entityTable.size() - 1] = pdeAssignList;

			}
			else if (tokens[0] == "assignment")
			{
				if (firstTime == true)
				{
					pdeAssignmentList.push_back("assignment");
					entityTable.push_back(pdeAssignmentList);
					firstTime = false;
				}

				pdeAssignmentList.push_back(tokens[y]);
				entityTable[entityTable.size() - 1] = pdeAssignmentList;

			}
			else if (tokens[0] == "call")
			{
				if (firstTime == true)
				{
					pdeCallList.push_back("call");
					entityTable.push_back(pdeCallList);
					firstTime = false;
				}

				pdeCallList.push_back(tokens[y]);
				entityTable[entityTable.size() - 1] = pdeCallList;

			}
			else if (tokens[0] == "while")
			{
				if (firstTime == true)
				{
					pdeWhileList.push_back("while");
					entityTable.push_back(pdeWhileList);

					firstTime = false;
				}

				pdeWhileList.push_back(tokens[y]);
				entityTable[entityTable.size() - 1] = pdeWhileList;

			}
			else if (tokens[0] == "if")
			{
				if (firstTime == true)
				{
					pdeIfList.push_back("if");
					entityTable.push_back(pdeIfList);
					firstTime = false;
				}

				pdeIfList.push_back(tokens[y]);
				entityTable[entityTable.size() - 1] = pdeIfList;

			}
			else if (tokens[0] == "variable")
			{
				if (firstTime == true)
				{
					pdeVariableList.push_back("variable");
					entityTable.push_back(pdeVariableList);
					firstTime = false;
				}

				pdeVariableList.push_back(tokens[y]);
				entityTable[entityTable.size() - 1] = pdeVariableList;

			}
			else if (tokens[0] == "constant")
			{
				if (firstTime == true)
				{
					pdeConstantList.push_back("constant");
					entityTable.push_back(pdeConstantList);
					firstTime = false;
				}

				pdeConstantList.push_back(tokens[y]);
				entityTable[entityTable.size() - 1] = pdeConstantList;

			}
			else if (tokens[0] == "prog_line")
			{
				if (firstTime == true)
				{
					pdeProg_LineList.push_back("prog_line");
					entityTable.push_back(pdeProg_LineList);
					firstTime = false;
				}

				pdeProg_LineList.push_back(tokens[y]);

				entityTable[entityTable.size() - 1] = pdeProg_LineList;
			}
			else
			{
				cout << "ERROR WRONG DECLARATION";
			}

		}

	}
	return entityTable;





















}
void PQLParser::queryAnalyzer(string strQuery)
{

	istringstream buf(strQuery);
	istream_iterator< string> beg(buf), end;//split by " "
	vector< string> tokens(beg, end);

	bool openBracket1 = false;
	bool openBracket2 = false;

	int tempVar;
	vector<string> resultOfSplitting;
	for (int x = 0; x<tokens.size(); x++)
	{

		bool gate = false;
		for (int y = 0; y < tokens[x].size(); y++)
		{
			if (tokens[x].substr(0, 1) == "(")
			{

				resultOfSplitting.push_back(tokens[x].substr(y, tokens[x].size()));
				gate = true;
				break;
			}
			else if (y>0 && tokens[x].substr(y, 1) == "(")
			{
				resultOfSplitting.push_back(tokens[x].substr(0, y));
				resultOfSplitting.push_back(tokens[x].substr(y, tokens[x].size()));
				gate = true;
				break;
			}
		}
		if (gate == false)
		{
			resultOfSplitting.push_back(tokens[x]);
		}
	}
	tokens = resultOfSplitting;


	//this will combine the ()
	bool openBracket = false;
	for (int x = 0; x<tokens.size(); x++)
	{
		if (openBracket == false && tokens[x].substr(0, 1) == "(" && tokens[x].substr(tokens[x].size() - 1, 1) != ")")
		{
			tempVar = x;
			openBracket = true;
		}
		else if (openBracket == true && tokens[x].substr(tokens[x].size() - 1, 1) == ")")
		{
			string tempQuery;
			for (int k = tempVar; k < x + 1; k++) // add string at tempvar to string at current position
			{
				tempQuery += tokens[k];
			}
			tempResultQuery.push_back(tempQuery);

			//cout << "x :" + tempQuery + "\n";
			if (tempQuery.substr(0, 1) == "(")
			{
				//identifyVariablesInBracket(tempQuery);

			}

			openBracket = false;

		}
		else if (openBracket == false)
		{
			tempResultQuery.push_back(tokens[x]);
		}

	}
















	cout << "\n----------------------------------\n";

	for (int x = 0; x < tempResultQuery.size(); x++)
	{
		cout << tempResultQuery[x] + "---";
	}
	cout << "\n----------------------------------\n";

}
string PQLParser::removeSpaces(string input)
{
	input.erase(remove(input.begin(), input.end(), ' '), input.end());
	return input;
}
void PQLParser::identifyVariablesInBracket(string input)
{
	//remove leading and trailing brackets
	removeSpaces(input);
	input.pop_back();
	input = input.substr(1).append("");
	vector< string> sep = split(input, ',');//split by ","

	cout << "sep[0]=" + sep[0] + " sep[1]=" + sep[1] + "\n";

}
vector<vector< string>> PQLParser::getEntityTable()
{
	return entityTable;
}
pair<bool, string> PQLParser::patternCheckSecond(string input, vector<string> allEntities)
{
	cout << " patternCheckSecond";
	return make_pair(true, "test");

}
pair<bool, string> PQLParser::patternCheckFirst(string input, vector<string> allEntities)
{
	cout << " patternCheckFirst";
	return make_pair(true, "test");
}
pair<bool, string> PQLParser::clausesCheckFirst(string input, vector<string> allEntities)
{
	cout << " clausesCheckFirst";
	return make_pair(true, "test");


}
pair<bool, string> PQLParser::clausesCheckSecond(string input, vector<string> allEntities)
{
	cout << " clausesCheckSecond";
	return make_pair(true, "test");



}

void PQLParser::verifyQuery(vector<vector<string>>unverifyQuery, vector<vector<string>> entityTable, vector<string> allEntities, vector<Clause> clausesVector)
{
	printAllEntities(allEntities); //print all entities
	printAllUnverifyQueries(unverifyQuery); //print all unverified query
	PQLVerifyTable vt =PQLVerifyTable(entityTable,allEntities); 

	int counter=0;
	for(int x=0;x<unverifyQuery.size();x++)
	{
		//passing size, and the whole array of single unverified query[string] unverifyQuery[x] = [contain][array][of][string][of][a][query]
		bool queryValidity = vt.check(unverifyQuery[x].size(), unverifyQuery[x]); 
		if(queryValidity)
		{
			counter++;
			for(int k=0;k<unverifyQuery[x].size();k++)
			{
				cout<< unverifyQuery[x][k] + " ";
			}
			cout<<" correct \n";
		}
		else
		{
			for(int k=0;k<unverifyQuery[x].size();k++)
			{
				cout<< unverifyQuery[x][k] + " ";
			}
			cout<<" wrong \n";
		}
		

	

	}
		vt.getClausesList();


		cout<< " correct statements : "+ to_string(counter);


}
void PQLParser::printAllEntities(vector<string> allEntities)
{
	for (int x = 0; x < allEntities.size(); x++)
	{
		cout << allEntities[x] + " ";
	}
}
void PQLParser::printAllUnverifyQueries(vector<vector<string>>unverifyQuery)
{
	cout << "\n";

	for (int x = 0; x < unverifyQuery.size(); x++)
	{
		for (int y = 0; y < unverifyQuery[x].size(); y++)
		{
			cout << unverifyQuery[x][y] + "|";
		}
		cout << "\n";

	}

}

