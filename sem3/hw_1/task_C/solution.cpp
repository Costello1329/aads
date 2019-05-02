#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits.h>

using namespace std;
typedef unsigned long long ull;
const int INF = INT_MAX;

ostream& operator << (ostream& os, const vector<int> &vec) {
	for (int el: vec)
		os << el << " ";

	return os;  
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

	Edge (int to = -1, int flow = -1, int capacity = -1) {
		this->to = to;
		this->flow = flow;
		this->capacity = capacity;
	}

	int recidualCapacity () const {
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

		Edge getEdge (int from, int to) {
			for (const Edge &edge: edges[from])
				if (edge.to == to)
					return edge;

			return Edge(-1, -1, -1);
		}

		void increaseFlow (int from, int to, int delta) {
			for (int i = 0; i < edges[from].size(); i ++) {
				if (edges[from][i].to == to) {
					edges[from][i].flow += delta;
					return;
				}
			}
		}

		vector<Edge> const & getEdges (int vertex) const {
			return this->edges[vertex];
		}

		vector<Edge> & getEdgesNonConst (int vertex) {
			return this->edges[vertex];
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

	for (const Edge &edge: net.getEdges(vertex)) {
		int to = edge.to;
		int recidualCapacity = edge.recidualCapacity();

		if (layers[to] != layers[vertex] + 1 || recidualCapacity == 0) {
			pointers[vertex] ++;
			continue;
		}

		int newPushedFlow = min<int>(pushedFlow, recidualCapacity);
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

		for (Edge edge: net.getEdges(vertex)) {
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


bool tryIncreaseFlowDFS (Net &net, int vertex, int end, vector<bool> &used) {
	if (used[vertex])
		return false;

	else if (vertex == end)
		return true;

	used[vertex] = true;

	for (const Edge &edge: net.getEdges(vertex)) {
		if (edge.recidualCapacity() > 0 && tryIncreaseFlowDFS(net, edge.to, end, used)) {
			net.increaseFlow(vertex, edge.to, 1);
			net.increaseFlow(edge.to, vertex, -1);
			return true;
		}
	}

	return false;
}

bool tryIncreaseFlow (Net &net) {
	vector<bool> used(net.size(), false);
	return tryIncreaseFlowDFS(net, net.source, net.sink, used);
}


void clearFlow (Net &net) {
	for (unsigned vertex = 0; vertex < net.size(); vertex ++)
		for (Edge &edge: net.getEdgesNonConst(vertex))
			edge.flow = 0;
}


int main () {
	int verticesCount;
	cin >> verticesCount;

	Net net(verticesCount);
	net.source = 0;
	net.sink = verticesCount - 1;

	int edgesCount;
	cin >> edgesCount;

	for (int i = 0; i < edgesCount; i ++) {
		int from, to, capacity;
		cin >> from >> to >> capacity;
		net.addEdge(from - 1, to - 1, capacity);
	}

	ull maxFlow = dinic(net);
	cout << maxFlow << endl;


	unsigned t;
	cin >> t;

	for (unsigned i = 0; i < t; i ++) {
		unsigned type;
		cin >> type;

		unsigned from, to;
		cin >> from >> to;
		from --;
		to --;

		if (type == 1) {
			net.addEdge(from, to, 1);

			if (tryIncreaseFlow(net))
				maxFlow ++;
		}

		else if (type == 2) {
			net.addEdge(from, to, -1);

			clearFlow(net);
			maxFlow = dinic(net);
		}

		cout << maxFlow << endl;
	}

	return 0;
}
