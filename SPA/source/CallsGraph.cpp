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
	/*for (int i = 0; i < vertexCount; i++)
	{
		delete[] adjM[i];
	}
	delete[] adjM;*/
}

void CallsGraph::InitializeCallsGraph(int vertex_count)
{
	this->vertex_count_ = vertex_count;
	adjacency_matrix_ = new int*[vertex_count];
	for (int i = 0; i < vertex_count; i++)
	{
		adjacency_matrix_[i] = new int[vertex_count];
		for (int j = 0; j < vertex_count; j++)
		{
			adjacency_matrix_[i][j] = 0;
		}
	}
}

void CallsGraph::AddCallsGraphEdge(int i, int j)
{
	adjacency_matrix_[i][j] = 1;
}

bool CallsGraph::HasArcCallsGraph(int i, int j)
{
	if (adjacency_matrix_[i][j] == 1) {
		return true;
	}
	return false;
}

bool CallsGraph::IsCallsGraphCyclic()
{
	bool *visited = new bool[vertex_count_];
	bool *rec_stack = new bool[vertex_count_];
	for (int i = 0; i < vertex_count_; i++)
	{
		visited[i] = false;
		rec_stack[i] = false;
	}

	for (int i = 0; i < vertex_count_; i++)
	{
		if (IsCyclicHelper(i, visited, rec_stack))
		{
			return true;
		}
	}
	return false;
}

bool CallsGraph::IsCyclicHelper(int v, bool visited[], bool* rec_stack)
{
	if (visited[v] == false)
	{
		visited[v] = true;
		rec_stack[v] = true;

		// Recur for all the vertices adjacent to this vertex
		for (int j = 0; j < vertex_count_; j++)
		{
			if (adjacency_matrix_[v][j] == 1)
			{
				if (!visited[j] && IsCyclicHelper(j, visited, rec_stack))
				{
					return true;
				}
				else if (rec_stack[j])
				{
					return true;
				}
			}
		}
	}
	rec_stack[v] = false;
	return false;
}