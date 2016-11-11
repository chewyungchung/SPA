#pragma once

using namespace std;

class CallsGraph
{
public:
	CallsGraph();
	~CallsGraph();
	void InitializeCallsGraph(int vertex_count);
	void AddCallsGraphEdge(int i, int j);
	bool HasArcCallsGraph(int i, int j);
	bool IsCallsGraphCyclic();

private:
	int** adjacency_matrix_;
	int vertex_count_ = 0;
	bool IsCyclicHelper(int v, bool visited[], bool* rec_stack);
};
