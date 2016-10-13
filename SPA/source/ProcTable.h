#pragma once

#include <list>
#include <string>
#include <unordered_map>

using namespace std;

class ProcTable
{
public:
	ProcTable();
	~ProcTable();

	void addProc(string procName);
	void addProcMod(string procName, string var);
	void addProcUses(string procName, string var);

	string getProcName(int procIndex);
	int getProcIndex(string procName)
};