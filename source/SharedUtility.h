#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class SharedUtility {
public:
	enum colours { FOLLOWS, FOLLOWSTAR, PARENT, PARENTSTAR, MODIFIES, USES, SELECT, ASSIGNPATTERN };

	const char *colour_names[] = { "FOLLOWS","FOLLOWSTAR",
		"PARENT","PARENTSTAR",
		"MODIFIES",
		"USES",
		"SELECT",
		"ASSIGNPATTERN" };

private:


};