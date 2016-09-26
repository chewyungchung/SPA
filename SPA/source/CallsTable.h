#pragma once

#include <list>
#include <string>
#include <unordered_map>

using namespace std;

class CallsTable
{
public:
	CallsTable();
	~CallsTable();

	void addCalls(string caller, string callee);

	list<string> getCallees(string caller);
	list<string> getCallers(string callee);
	bool isCallsEmpty();
	bool isCalls(string caller, string callee);
	bool isCallsStar(string caller, string callee);
	list<string> getCalleesStar(string caller);
	list<string> getCallersStar(string callee);

private:
	unordered_map<string, list<string>> callsTable; // key: caller procName; value: list of callee procNames
	list<string> callsStarHelperChecked;
	list<string> getCalleesStarHelperList;
	list<string> getCallersStarHelperList;

	bool isCallsStarHelper(string caller, string callee);
	void getCalleesStarHelper(string caller);
	void getCallersStarHelper(string callee);
};
