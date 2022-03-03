#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <algorithm>

#define M_PI 3.14159265358979323846



struct Node {
	int ID;
	int ID_path_via;
	float path_length;
	float distance_from_the_end;
	float fitness;
} buffer;

struct Edge_t {
	int from;
	int to;
}bufferEdge;

int compare(const void* p1, const void* p2)
{
	const struct Node* elem1 = (Node*)p1;
	const struct Node* elem2 = (Node*)p2;

	if (elem1->fitness < elem2->fitness)
		return 1;
	else if (elem1->fitness > elem2->fitness)
		return -1;
	else
		return 0;
}

float distance(float a1[2], float a2[2])
{
	float lat1, lat2, lon1, lon2, dlat, dlon;
	lat1 = a1[0] * M_PI / 180;
	lat2 = a2[0] * M_PI / 180;
	lon1 = a1[1] * M_PI / 180;
	lon2 = a2[1] * M_PI / 180;
	dlat = lat2 - lat1;
	dlon = lon2 - lon1;
	float a = pow(sin(dlat/2), 2) + (cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2));
	float c = 2 * atan2f(sqrt(a), sqrt(1 - a));
	float d = 6371000 * c;
	return d;
}

int main()
{
	int START = 11, END = 46;
	std::cout << "Submit starting node: ";
	std::cin >> START;
	std::cout << "Submit end node: ";
	std::cin >> END;
	std::cout << std::endl;

	std::cout << "SETUP:" << std::endl << "    START: " << START << std::endl << "    END: " << END << std::endl;
	std::cout << std::endl;
	std::ifstream wej;
	std::vector<Edge_t> edges;
	std::vector<Node> NodesQueue;
	std::vector<Node> ClosedNodes;
	std::cout << "Loading the graph..." << std::endl;
	wej.open("Graph.txt"); //reading the number of nodes and the list of edges
	if (wej.is_open())
	{
		std::cout << "File opened" << std::endl;
	}
	else
	{
		std::cout << "Error: Failed to open the file." << std::endl;
	}
	int n;
	Edge_t edge;
	std::cout << "Reading..." << std::endl;
	wej >> n;
	while (wej >> edge.from)
	{
		wej >> edge.to;
		edges.push_back(edge);
	}
	wej.close();

	std::cout << "The graph has been read!" << std::endl;
	std::cout << std::endl;

	float** coordinates = new float* [n];
	for (int i = 0; i < n; i++)
	{
		coordinates[i] = new float[2];
	}

	int bufferint;
	std::cout << "Loading the coordinates..." << std::endl;
	wej.open("coordinates.txt");                                                 //reading coordinates of points
	if (wej.is_open())
	{
		std::cout << "File opened" << std::endl;
	}
	else
	{
		std::cout << "Error: Failed to open the file." << std::endl;
	}
	while (wej >> bufferint)
	{
		wej >> coordinates[bufferint][0] >> coordinates[bufferint][1];
	}
	wej.close();
	std::cout << "The coordinates have been read!" << std::endl;
	std::cout << std::endl;

	std::cout << "Setting up Nodes..." << std::endl;

	for (int i = 0; i < n; i++)
	{
		buffer.ID = i;
		buffer.ID_path_via = -1;
		buffer.distance_from_the_end = distance(coordinates[i], coordinates[END]);
		buffer.path_length = INFINITY;
		buffer.fitness = INFINITY;
		NodesQueue.push_back(buffer);
	}


	for (int i = 0; i < n; i++)
	{
		if (NodesQueue[i].ID == START)
		{
			NodesQueue[i].ID_path_via = NodesQueue[i].ID;
			NodesQueue[i].path_length = 0;
			NodesQueue[i].fitness = NodesQueue[i].path_length + NodesQueue[i].distance_from_the_end;
			break;
		}
	}

	qsort(&NodesQueue.front(), NodesQueue.size(), 20, compare);  //Do uzupe³nienia

	int currentID, potential_next;
	float new_path_length;

	std::cout << "Seeking the shortest path..." << std::endl;

	while (NodesQueue.back().ID != END)
	{
		if (NodesQueue.back().fitness == INFINITY)
		{
			std::cout << "Error: No route!" << std::endl;
			return 0;
		}

		ClosedNodes.push_back(NodesQueue.back());
		NodesQueue.pop_back();
		currentID = ClosedNodes.back().ID;
		for (int i = 0; i < edges.size(); i++)
		{
			if (edges[i].from == currentID)
			{
				potential_next = edges[i].to;
				for (int j = 0; j < NodesQueue.size(); j++)
				{
					if (NodesQueue[j].ID == potential_next)
					{
						new_path_length = ClosedNodes.back().path_length + distance(coordinates[currentID], coordinates[NodesQueue[j].ID]);
						if (new_path_length < NodesQueue[j].path_length)
						{
							NodesQueue[j].path_length = new_path_length;
							NodesQueue[j].ID_path_via = currentID;
							NodesQueue[j].fitness = NodesQueue[j].distance_from_the_end + NodesQueue[j].path_length;
						}
					}
				}
			}
		}

		qsort(&NodesQueue.front(), NodesQueue.size(), 20, compare);  
	}
	ClosedNodes.push_back(NodesQueue.back());
	NodesQueue.pop_back();

	std::vector<Edge_t> route;
	currentID = END;

	std::cout << "Retracing the route..." << std::endl;
	std::cout << std::endl;

	while (currentID != START)
	{
		for (int i = 0; i < ClosedNodes.size(); i++)
		{
			if (ClosedNodes[i].ID == currentID)
			{
				bufferEdge.from = ClosedNodes[i].ID_path_via;
				bufferEdge.to = currentID;
				route.push_back(bufferEdge);
				currentID = bufferEdge.from;
				break;
			}
		}
	}
	std::reverse(route.begin(), route.end());

	std::cout << "Done! The shortest route is:" << std::endl;

	std::cout << route[0].from;

	for (int i = 0; i < route.size(); i++)
	{
		std::cout << " -> " << route[i].to;
	}
	std::cout << std::endl;
	std::cout << "Path's length: " << ClosedNodes.back().fitness << "m" << std::endl;
	std::cout << std::endl;

	std::cout << "Cleaning out..." << std::endl;

	for (int i = 0; i < n; i++)
	{
		delete coordinates[i];
	}
	delete coordinates;

	std::cout << "Exiting..." << std::endl;

	return 0;
}