#include "QueryTable.h"

/*
* 
*/

QueryTable::QueryTable()
{

}
Clause QueryTable::getSelectClause()
{
	return Clause();
}

Clause QueryTable::getSuchThatClause()
{
	return Clause();
}

Clause QueryTable::getPatternClause()
{
	return Clause();
}

string QueryTable::getSynType(string syn)
{
	return string();
}
