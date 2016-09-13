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
	Parser p("C:\\Users\\Pheng Heong\\Downloads\\AutoTester-Aug15-VS2015\\AutoTester-Aug15-VS2015\\AutoTester\\Sample-Source2.txt");
	QueryValidator qv = QueryValidator(query);
	QueryTable qt = qv.parse();
	
	QueryEvaluator qe = QueryEvaluator(qt, p.process());
	QueryTable evaluationResults = qe.evaluate();
	if (bool selectResults = evaluationResults.getSelectResult().isArg1ResultEmpty()) {
		cout << "empty, man.";
	}
	else {
		cout << "Something here!";
	}


	QueryResultProjector qrp = QueryResultProjector(evaluationResults);
	list<string> finalResults = qrp.getResults();

  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.
	for (auto& str : finalResults) {
		results.push_back(str);
	}
}