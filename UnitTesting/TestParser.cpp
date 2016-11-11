/*
We test that the Tokenizer is correctly feeding tokens into the Parser
*/
#include "stdafx.h"

#include <iterator>
#include <algorithm>
#include <string>
#include <vector>
#include <list>
#include "CppUnitTest.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "PKB.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTesting
{
	TEST_CLASS(TestParser)
	{
	public:
		TEST_METHOD(TestTokenizer1_IT1)
		{
			Tokenizer tk("SIMPLE_test_1.txt");
			list<string> expectedList;
			vector<string> expectedVec;
			expectedVec = { "procedure", "ABC", "{",
							"x", "=", "1", ";",
							"b", "=", "2", ";",
							"y", "=", "x", "+", "i", ";",
							"while", "i", "{",
							"while", "z", "{",
							"good", "=", "b2y", ";", "}",
							"apple", "=", "orange", ";", "}", "}" };
			vecToListHelper(expectedVec, expectedList);

			list<string>::iterator it = expectedList.begin();

			for (; it != expectedList.end(); ++it) {
				string token = tk.GetNextToken();
				Assert::AreEqual(*it, token);
			}
		}

		TEST_METHOD(TestTokenizer2_IT1)
		{
			Tokenizer tk("SIMPLE_test_2.txt");
			list<string> expectedList;
			vector<string> expectedVec;
			expectedVec = {
				"procedure", "ABC", "{", "x", "=", "1", ";", "b", "=", "2", ";",
				"while", "i", "{", "apple", "=", "orange", ";", "banana", "=", "pear",
				";", "}", "while", "x", "{", "s", "=", "t", ";", "while", "y", "{",
				"r", "=", "2", ";", "mango", "=", "durian", ";", "}", "while", "z", "{",
				"papaya", "=", "watermelon", ";", "}", "}", "}" };
			vecToListHelper(expectedVec, expectedList);

			list<string>::iterator it = expectedList.begin();

			for (; it != expectedList.end(); ++it) {
				string token = tk.GetNextToken();
				Assert::AreEqual(*it, token);
			}
		}

		TEST_METHOD(TestTokenizer3_IT1)
		{
			Tokenizer tk("SIMPLE_test_3.txt");
			list<string> expectedList;
			vector<string> expectedVec;
			expectedVec =
			{
				"procedure", "XYZ", "{", "x", "=", "1", ";", "x", "=", "z", ";",
				"while", "i", "{", "apple", "=", "orange", ";", "banana", "=", "pear",
				";", "}", "while", "x", "{", "s", "=", "t", ";", "while", "y", "{", "r",
				"=", "2", ";", "x", "=", "y", ";", "mango", "=", "durian", ";", "}",
				"while", "z", "{", "papaya", "=", "watermelon", ";", "}", "}", "}"
			};
			vecToListHelper(expectedVec, expectedList);

			list<string>::iterator it = expectedList.begin();

			for (; it != expectedList.end(); ++it) {
				string token = tk.GetNextToken();
				Assert::AreEqual(*it, token);
			}
		}

		TEST_METHOD(TestTokenizer1_IT2)
		{
			Tokenizer tk("SIMPLE_test_4.txt");
			
			/*
				procedure ABC {
			1		x = 1;
			2		moonbe4ms = y+77 * zebras * r   + 2;
			3		if butterfly then {
			4			call HENDRIX;
			5			k = (kappa123 + 4) + (y-57);
			6			while fairytales {
			7				call Jimmy;
			8				while wind {
			9					j = (stars    + i);}}}
					else {
			10			litteWing = 5 -   2;}}
		
				procedure HENDRIX {
			11		SSB = 1;
			12		call Jimmy;}

				procedure Jimmy {
			13		if c  then {
			14			k = r;}
					else  {
			15			banner    = 34;}}
			*/

			list<string> expectedList;
			vector<string> expectedVec;
			expectedVec =
			{
				"procedure", "ABC", "{", "x", "=", "1", ";", "moonbe4ms", "=", "y", "+", "77", "*", "zebras", "*", "r", "+", "2", ";",
				"if", "butterfly", "then", "{", "call", "HENDRIX", ";", "k", "=", "(", "kappa123", "+", "4", ")", "+", "(", "y", "-", "57", ")", ";",
				"while", "fairytales", "{", "call", "Jimmy", ";", "while", "wind", "{", "j", "=", "(", "stars", "+", "i", ")", ";", "}", "}", "}",
				"else", "{", "littleWing", "=", "5", "-", "2", ";", "}", "}",
				"procedure", "HENDRIX", "{", "SSB", "=", "1", ";", "call", "Jimmy", ";", "}",
				"procedure", "Jimmy", "{", "if", "c", "then", "{", "k", "=", "r", ";", "}",
				"else", "{", "banner", "=", "34", ";", "}", "}"
			};
			vecToListHelper(expectedVec, expectedList);

			list<string>::iterator it = expectedList.begin();

			for (; it != expectedList.end(); ++it) {
				string token = tk.GetNextToken();
				Assert::AreEqual(*it, token);
			}
		}

	public:

		bool listCmpHelper(list<string> list1, list<string> list2)
		{
			if (list1.size() != list2.size())
			{
				return false;
			}
			else
			{
				list<string>::iterator it = list1.begin();
				for (; it != list1.end(); ++it)
				{
					if (find(list2.begin(), list2.end(), *it) == list2.end())
					{
						return false;
					}
				}
				return true;
			}
		}

		bool listCmpHelper(list<int> list1, list<int> list2)
		{
			if (list1.size() != list2.size())
			{
				return false;
			}
			else
			{
				list<int>::iterator it = list1.begin();
				for (; it != list1.end(); ++it)
				{
					if (find(list2.begin(), list2.end(), *it) == list2.end())
					{
						return false;
					}
				}
				return true;
			}
		}

		// Swap with empty vector to free memory
		void clearVector(vector<string> v)
		{
			vector<string>().swap(v);
		}

		void clearVector(vector<int> v)
		{
			vector<int>().swap(v);
		}

		void vecToListHelper(vector<string> v, list<string> & s)
		{
			s.clear();
			copy(v.begin(), v.end(), back_inserter(s));
		}

		void vecToListHelper(vector<int> v, list<int> & s)
		{
			s.clear();
			copy(v.begin(), v.end(), back_inserter(s));
		}		
	};
}