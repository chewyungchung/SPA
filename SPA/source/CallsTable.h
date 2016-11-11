#pragma once

#include <list>
#include <string>
#include <map>

using namespace std;

class CallsTable
{
public:
	CallsTable();
	~CallsTable();

	void AddCalls(string caller, string callee);

	list<string> GetCallee(string caller);
	list<string> GetCaller(string callee);
	bool IsCallExist();
	bool IsCall(string caller, string callee);
	bool IsCallStar(string caller, string callee);
	list<string> GetCalleeStar(string caller);
	list<string> GetCallerStar(string callee);

private:
	map<string, list<string>> calls_table_; // key: caller procName; value: list of callee procNames
	list<string> calls_star_helper_checked_list_;
	list<string> get_callees_star_helper_list_;
	list<string> get_callers_star_helper_list_;

	bool IsCallsStarHelper(string caller, string callee);
	void GetCalleesStarHelper(string caller);
	void GetCallersStarHelper(string callee);
};
