#include <list>
#include <vector>
#include <string>
#include <map>

using namespace std;

class ConstantTable
{
public:
	ConstantTable();
	void AddConstant(int constant, int stmt_line);
	list<int> GetConstantList();
	list<int> GetStmtlineByConstant(int constant); //input: constant, output: stmtline

private:
	map<int, list<int>> constant_table_;
	list<int> constant_list_;
};