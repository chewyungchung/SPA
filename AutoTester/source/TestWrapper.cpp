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
	_pkb = PKB();
	_qv = QueryValidator();
	_qe = QueryEvaluator();
	_qrp = QueryResultProjector();
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
	// call your parser to do the parsing
  // ...rest of your code...
	_pkb = _parser.process(filename);
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){
// call your evaluator to evaluate the query here
  // ...code to evaluate query...
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));

	//results.clear();

	//cout << "This line of query: " << query << endl;
	_qv = QueryValidator(query);
	QueryTable _qt = _qv.parse();
	_qe = QueryEvaluator(_qt, _pkb);
	_qrp = QueryResultProjector(_qe.evaluate());
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	list<string> finalResults = _qrp.getResults();
	results = finalResults;
	/*PKB pkb;
	Parser p(textFile);
	pkb = p.process();
	QueryValidator qv = QueryValidator(query);
	QueryTable qt = qv.parse();
	QueryEvaluator qe = QueryEvaluator(qt, pkb);
	QueryTable evaluationResults = qe.evaluate();*/
	/*
	vector<string> patternResults = evaluationResults.getPatternResult().getPatternResultList();
	if (patternResults.empty() == false) {
		cout << "Pattern results:::::" << endl;
		for (vector<string>::iterator it = patternResults.begin(); it != patternResults.end(); it++) {
			cout << *it << endl;
		}
	}
	
	cout << "Delay!" << endl;
	*/

	//QueryResultProjector qrp(evaluationResults);
	//list<string> finalResults = qrp.getResults();

	//// store the answers to the query in the results list (it is initially empty)
	//// each result must be a string.
	//if (finalResults.empty() == false) {
	//	for (list<string>::iterator it = finalResults.begin(); it != finalResults.end(); ++it) {
	//		results.push_back(*it);
	//	}
	//}
	//std::this_thread::sleep_for(std::chrono::milliseconds(400));
}