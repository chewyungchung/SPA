#include "TestWrapper.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool TestWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
  // create any objects here as instance variables of this class
  // as well as any initialization required for your spa program
	
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
	// call your parser to do the parsing
  // ...rest of your code...
	
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){
// call your evaluator to evaluate the query here
  // ...code to evaluate query...
	string queryString = "assign a; Select a";
	QueryValidator qv(queryString);
	QueryTable qt = qv.parse();
	string isQtNull;
	if (qt.isNullQuery() == true) {
		isQtNull = "true";
	}
	else {
		isQtNull = "false";
	}
	cout << "Is it NULL???? : " << isQtNull << "!!!!!!!!!!!!!!!!!!" << endl;
	
	
	QueryEvaluator qe(qt);
	QueryTable evaluationResults = qe.evaluate();
	vector<string> selectResults = evaluationResults.getSelectResult().getArg1ResultList();
	QueryResultProjector qrp(evaluationResults);
	list<string> finalResults = qrp.getResults();

	for (vector<string>::iterator it = selectResults.begin(); it != selectResults.end(); it++) {
		cout << *it << endl;
	}

  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.
	results.push_back("1");
	results.push_back("2");
	results.push_back(to_string(finalResults.size()));
}