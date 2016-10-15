#pragma once

using namespace std;

class CallsGraph
{
public:
	CallsGraph();
	~CallsGraph();
	void initializeCallsGraph(int vertexCount);
	void addCallsGraphEdge(int i, int j);
	bool hasArcCallsGraph(int i, int j);
	bool isCallsGraphCyclic();

private:
	int** adjM;
	int vertexCount;
	bool isCyclicHelper(int v, bool visited[], bool* recStack);
};
