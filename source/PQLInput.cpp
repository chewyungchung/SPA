#include "PQLInput.h"

using namespace std;

string PQLInput::getDeclaration()
{
	// string stringDeclaration = "assignment a, a1; variable v;";
	// string stringDeclaration = "assignment a; variable v;";
	// string stringDeclaration = "assign a; while w;";
	return  "stmt s, s1; assign a, a1, a2; while w, w1; if ifstmt, iftstmt1; procedure p; variable v; constant c; prog_line n, n1, n2;";
}
vector<string> PQLInput::getQueries()
{

	vector<string> queries;
	queries.push_back("           Select BOOLEAN such that Modifies (_, “x”)"); //valid queries 1
	queries.push_back("Select w such that Parent       (ifstmt, w)"); //valid queries2
	queries.push_back("Select                a                 ");//valid queries3
	queries.push_back("                     Select a pattern           a(“x”                      , _)");//valid queries4
	queries.push_back("Select a such that Modifies(a, “                  x”)                       ");//valid queries5
	queries.push_back("Select a pattern a(“X”, _) such that Follows(a, w)");//valid queries6
	queries.push_back("Select a such that Follows (w, a) pattern a (“x”,_) ");//valid queries7
	queries.push_back("Select s such that Follows (1, s)");//valid queries8
	queries.push_back("Select a pattern a (“x”, _“y”         _)");//valid queries9
	queries.push_back("Select a pattern a (“x”, _“y”         _)");//invalid queries10
	queries.push_back("Select a pattesrn a (“x”, _“y”_)");//pattern is spelt wrongly12
	queries.push_back("Select a pattern a(“X”, _) such that Follows(a!@, w)");//invalid queries10
	queries.push_back("Select s suaach that Follows (1, s)");//such is spelt wrongly13
	queries.push_back("Select a pattern aqweqwe (“x”, _“y”_)");//no aqweqwe14
	queries.push_back("Select s such that Followsaaaaa (1, s)");//followsaaaaa is wrong15
	queries.push_back("Select s such that Follows (1, s))");//double bracket16
	queries.push_back("Select a pattern a(“x”, _“y”_)");//double bracket17
	queries.push_back("Select a pattern a(“x”, _“10”_)");//double bracket18
	queries.push_back("Select a pattern a(“x”, _)");//double bracket19
	queries.push_back("Select a pattern a(_, _“y”_)");//double bracket20
	queries.push_back("Select a pattern a(_, _“10”_)");//double bracket21
	queries.push_back("Select a pattern a(_, _)");//double bracket22
	queries.push_back("Select a pattern a(v, “y”)");//double bracket23
	queries.push_back("Select a pattern a(v, “y”, 5)");//double bracket23
	queries.push_back("Select a such that Follows (w, a)  a (“x”,_) ");//invalid queries11

													   //	queries.push_back("Select a pattern a(v, _“10”_)");//double bracket24
													   //	queries.push_back("Select a pattern a(v, _)");//double bracket25




													   //queries.push_back("Select s such that Follows (,)");//error

	return queries;
}