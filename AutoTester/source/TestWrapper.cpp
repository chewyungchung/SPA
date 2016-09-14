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
	results.clear();
	Parser p("C:\\Users\\Einlanz\\Documents\\GitSPA\\Release\\Sample-Source.txt");
	QueryValidator qv = QueryValidator(query);
	QueryTable qt = qv.parse();
	QueryEvaluator qe = QueryEvaluator(qt, p.process());
	QueryTable evaluationResults = qe.evaluate();

	/*for (vector<string>::iterator it1 = ab1.begin(); it1 != ab1.end(); ++it1) {
		cout << "select: " << *it1 << endl;
	}

	for (vector<string>::iterator it2 = ab2.begin(); it2 != ab2.end(); ++it2) {
		cout << "suchthat: " << *it2 << endl;
	}
	for (vector<string>::iterator it3 = ab3.begin(); it3 != ab1.end(); ++it3) {
		cout << "pattern: " << *it3 << endl;
	}*/

	QueryResultProjector qrp = QueryResultProjector(evaluationResults);
	list<string> finalResults = qrp.getResults();

	cout << "Size of final results :" << to_string(finalResults.size()) << endl;
  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.
	for (auto& str : finalResults) {
		results.push_back(str);
	}
}