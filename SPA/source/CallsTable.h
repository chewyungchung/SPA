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

	void addCalls(string caller, string callee);

	list<string> getCallee(string caller);
	list<string> getCaller(string callee);
	bool isCallExist();
	bool isCall(string caller, string callee);
	bool isCallStar(string caller, string callee);
	list<string> getCalleeStar(string caller);
	list<string> getCallerStar(string callee);

private:
	map<string, list<string>> callsTable; // key: caller procName; value: list of callee procNames
	list<string> callsStarHelperChecked;
	list<string> getCalleesStarHelperList;
	list<string> getCallersStarHelperList;

	bool isCallsStarHelper(string caller, string callee);
	void getCalleesStarHelper(string caller);
	void getCallersStarHelper(string callee);
};
