#include <iostream>
#include <vector>
#include <queue>
#include <limits.h>

using namespace std;
typedef unsigned long long ull;
const int INF = INT_MAX;

ostream& operator << (ostream& os, const vector<int> &vec) {
	for (int el: vec)
		os << el << " ";

	return os;  
}

int min (ull a, int b) {
	return a < b ? a : b;
}


struct Edge {
	int to;
	int flow;
	int capacity;

	Edge () {
		this->to = -1;
		this->flow = -1;
		this->capacity = -1;
	}

	Edge (int to, int flow, int capacity) {
		this->to = to;
		this->flow = flow;
		this->capacity = capacity;
	}

	int recidualCapacity () {
		return capacity - flow;
	}
};

class Net {
	private:
		vector< vector<Edge> > edges;

		void addEdgeWithCheck (int from, int to, int capacity) {
			if (max(to, from) >= edges.size())
				edges.resize(max(to, from) + 1);

			for (int i = 0; i < edges[from].size(); i ++) {
				if (edges[from][i].to == to) {
					edges[from][i].capacity += capacity;
					return;
				}
			}

			edges[from].push_back(Edge(to, 0, capacity));
		}

	public:
		int source;
		int sink;

		Net (int size) {
			edges.resize(size);
		}

		void addEdge (int from, int to, int capacity) {
			addEdgeWithCheck(from, to, capacity);
			addEdgeWithCheck(to, from, 0);
		}

		void increaseFlow (int from, int to, int delta) {
			for (int i = 0; i < edges[from].size(); i ++) {
				if (edges[from][i].to == to) {
					edges[from][i].flow += delta;
					return;
				}
			}
		}

		void getEdges (int vertex, vector<Edge> &edges) const {
			edges = this->edges[vertex];
		}

		int size () const {
			return edges.size();
		}

		void print () const {
			for (int vertex = 0; vertex < edges.size(); vertex ++) {
				cout << (vertex + 1) << ": ";
		
				for (Edge edge: edges[vertex])
					cout << "(" << (edge.to + 1) << " – " << edge.flow << "/" << edge.capacity << ") ";
		
				cout << endl;
			}
		}
};


int findIncreasingPath (Net &net, const vector<int> &layers, vector<int> &pointers, int vertex, int end, int pushedFlow = INF) {
	if (vertex == end)
		return pushedFlow;

	vector<Edge> edges;
	net.getEdges(vertex, edges);

	for (int i = pointers[vertex]; i < edges.size(); i ++) {
		int to = edges[i].to;
		int recidualCapacity = edges[i].recidualCapacity();

		if (layers[to] != layers[vertex] + 1 || recidualCapacity == 0) {
			pointers[vertex] ++;
			continue;
		}

		int newPushedFlow = min(pushedFlow, recidualCapacity);
		int delta = findIncreasingPath(net, layers, pointers, to, end, newPushedFlow);

		if (delta > 0) {;
			net.increaseFlow(vertex, to, delta);
			net.increaseFlow(to, vertex, - delta);
			return delta;
		}

		else
			pointers[vertex] ++;
	}

	return 0;
}

bool buildLayeredNet (Net &net, vector<int> &layers) {
	queue<int> verticesQueue;
	verticesQueue.push(net.source);

	layers = vector<int>(net.size(), -1);
	layers[net.source] = 0;

	while (!verticesQueue.empty()) {
		int vertex = verticesQueue.front();
		verticesQueue.pop();

		vector<Edge> edges;
		net.getEdges(vertex, edges);

		for (Edge edge: edges) {
			if (layers[edge.to] == -1 && edge.recidualCapacity() > 0) {
				layers[edge.to] = layers[vertex] + 1;
				verticesQueue.push(edge.to);
			}
		}
	}

	return (layers[net.sink] != -1);
}

ull dinic (Net &net) {
	ull maxFlow = 0;
	vector<int> layers(net.size(), -1);

	while (buildLayeredNet(net, layers)) {
		int blockingFlow;
		vector<int> pointers(net.size(), 0);
		
		do {
			blockingFlow = findIncreasingPath(net, layers, pointers, net.source, net.sink);
			maxFlow += blockingFlow;
		} while (blockingFlow != 0);
	}

	return maxFlow;
}








int main () {
	while (true) {
		int verticesCount;
		cin >> verticesCount;

		if (verticesCount == 0)
			break;

		Net net(verticesCount);
		net.source = 0;
		net.sink = verticesCount - 1;
		cin >> net.source;
		cin >> net.sink;
		net.source --;
		net.sink --;

		int edgesCount;
		cin >> edgesCount;

		for (int i = 0; i < edgesCount; i ++) {
			int from, to, capacity;
			cin >> from >> to >> capacity;
			net.addEdge(from - 1, to - 1, capacity);
			net.addEdge(to - 1, from - 1, capacity);
		}

		cout << dinic(net) << endl;
	}

	return 0;
}