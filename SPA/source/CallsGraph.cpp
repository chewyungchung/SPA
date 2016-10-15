/*
Observe here we elect to use adjacency matrix 
because we need to do reverse travel when updating mod/uses for procedures,
adjacency list for this is inefficient
*/
#pragma once

#include "CallsGraph.h"

CallsGraph::CallsGraph()
{
}

CallsGraph::~CallsGraph()
{
	for (int i = 0; i < vertexCount; i++)
	{
		delete[] adjM[i];
	}
	delete[] adjM;
}

void CallsGraph::initializeCallsGraph(int vertexCount)
{
	this->vertexCount = vertexCount;
	adjM = new int*[vertexCount];
	for (int i = 0; i < vertexCount; i++)
	{
		adjM[i] = new int[vertexCount];
		for (int j = 0; j < vertexCount; j++)
		{
			adjM[i][j] = 0;
		}
	}
}

void CallsGraph::addCallsGraphEdge(int i, int j)
{
	adjM[i][j] == 1;
}

bool CallsGraph::hasArcCallsGraph(int i, int j)
{
	return adjM[i][j] == 1;
}

bool CallsGraph::isCallsGraphCyclic()
{
	bool *visited = new bool[vertexCount];
	bool *recStack = new bool[vertexCount];
	for (int i = 0; i < vertexCount; i++)
	{
		visited[i] = false;
		recStack[i] = false;
	}

	for (int i = 0; i < vertexCount; i++)
	{
		if (isCyclicHelper(i, visited, recStack))
		{
			return true;
		}
	}
	return false;
}

bool CallsGraph::isCyclicHelper(int v, bool visited[], bool* recStack)
{
	if (visited[v] == false)
	{
		visited[v] = true;
		recStack[v] = true;

		// Recur for all the vertices adjacent to this vertex
		for (int j = 0; j < vertexCount; j++)
		{
			if (adjM[v][j] == 1)
			{
				if (!visited[j] && isCyclicHelper(j, visited, recStack))
				{
					return true;
				}
				else if (recStack[j])
				{
					return true;
				}
			}
		}
	}
	recStack[v] = false;
	return false;
}