#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Tokenizer
{
	Tokenizer(FILE);
	string getToken();
};

