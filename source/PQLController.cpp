#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "PQLController.h"
#include "PQLParser.h"
#include "PQLInput.h"

using namespace std;

int main()
{
	PQLParser pqlParser;
	PQLInput pqlInput;
	vector<string> tets;

	pqlParser.Start(pqlInput.getDeclaration(), pqlInput.getQueries()); 
	getchar();
}