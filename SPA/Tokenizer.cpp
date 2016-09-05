#include "Tokenizer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

class Tokenizer {
public:
	vector<string> tokens;

	Tokenizer::Tokenizer(string filename) {
		static const string arr[] = { "{", "}", ";", "=" };
		vector<string> specials(arr, arr + sizeof(arr) / sizeof(arr[0]));
		char whitespace = ' ';

		ifstream infile(filename);
		string line;
		string token;
		while (std::getline(infile, line)) {
			int length = line.length;
			for (int i = 0; i < length; i++) {
				if (std::find(specials.begin(), specials.end(), line[i]) != specials.end()) {
					if (token.length != 0) {
						tokens.push_back(token);
					}

					stringstream ss;
					ss << line[i];
					ss >> token;
					tokens.push_back(token);
				}
				else if (line[i] == whitespace) {
					if (token.length != 0) {
						tokens.push_back(token);
					}
				}
				else {
					token += line[i];
				}
			}
		}
	}

	Tokenizer::~Tokenizer() {
	}

	string Tokenizer::getNextToken() {
		return NULL;
	}
};