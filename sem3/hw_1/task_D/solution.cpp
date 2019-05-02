#include <iterator>
#include <iostream>
#include <vector>
#include <queue>
#include <list>

using namespace std;
const int INF = 1000000;


struct Vertex {
	char type;
	int index;

	Vertex () {
		type = '0';
		index = -1;
	}

	Vertex (char type, int index) {
		this->type = type;
		this->index = index;
	}
};

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
				cout << vertex << ": ";
		
				for (Edge edge: edges[vertex])
					cout << "(" << edge.to << " вЂ“ " << edge.flow << "/" << edge.capacity << ") ";
		
				cout << endl;
			}
		}
};


void buildNet (Net &net, string s, string p, vector<Vertex> &verticies) {
	vector<int> sIndexToVertex(s.size(), -1);
	vector<int> pIndexToVertex(p.size(), -1);

	for (int i = 0; i <= s.size() - p.size(); i ++) {
		for (int j = 0; j < p.size(); j ++) {
			if (s[i + j] == '?' && p[j] == '0') {
				if (sIndexToVertex[i + j] == -1)
					sIndexToVertex[i + j] = net.size();
								
				net.addEdge(0, sIndexToVertex[i + j], 1);
				net.addEdge(sIndexToVertex[i + j], 0, 1);
			}

			else if (s[i + j] == '?' && p[j] == '1') {
				if (sIndexToVertex[i + j] == -1)
					sIndexToVertex[i + j] = net.size();
				
				net.addEdge(1, sIndexToVertex[i + j], 1);
				net.addEdge(sIndexToVertex[i + j], 1, 1);
			}

			else if (s[i + j] == '0' && p[j] == '?') {
				if (pIndexToVertex[j] == -1)
					pIndexToVertex[j] = net.size();
				
				net.addEdge(0, pIndexToVertex[j], 1);
				net.addEdge(pIndexToVertex[j], 0, 1);
			}

			else if (s[i + j] == '1' && p[j] == '?') {
				if (pIndexToVertex[j] == -1)
					pIndexToVertex[j] = net.size();
				
				net.addEdge(1, pIndexToVertex[j], 1);
				net.addEdge(pIndexToVertex[j], 1, 1);
			}

			else if (s[i + j] && p[j] == '?') {
				int size = net.size();

				if (pIndexToVertex[j] == -1)
					pIndexToVertex[j] = size ++;

				if (sIndexToVertex[i + j] == -1)
					sIndexToVertex[i + j] = size ++;

				net.addEdge(sIndexToVertex[i + j], pIndexToVertex[j], 1);
				net.addEdge(pIndexToVertex[j], sIndexToVertex[i + j], 1);
			}
		}
	}

	verticies.resize(net.size());

	for (int i = 0; i < sIndexToVertex.size(); i ++)
		if (sIndexToVertex[i] != -1)
			verticies[sIndexToVertex[i]] = Vertex('s', i);

	for (int j = 0; j < pIndexToVertex.size(); j ++)
		if (pIndexToVertex[j] != -1)
			verticies[pIndexToVertex[j]] = Vertex('p', j);
}

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

int countMaxFlow (Net &net) {
	int maxFlow = 0;
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

void cutGraph (const Net &net, int start, vector<bool> &reachable) {
	reachable[start] = true;
	vector<Edge> edges;
	net.getEdges(start, edges);

	for (Edge edge: edges)
		if (!reachable[edge.to] && edge.recidualCapacity() > 0)
			cutGraph(net, edge.to, reachable);
}

int solveProblem (string &s, string &p) {
	vector<Vertex> verticies;
	Net net(2);
	net.source = 0;
	net.sink = 1;
	buildNet(net, s, p, verticies);

	int hammingDistance = 0;

	for (int i = 0; i <= s.size() - p.size(); i ++)
		for (int j = 0; j < p.size(); j ++)
			if ((s[i + j] == '1' && p[j] == '0') || (s[i + j] == '0' && p[j] == '1'))
				hammingDistance ++;

	hammingDistance += countMaxFlow(net);
	
	vector<bool> reachable(net.size(), false);
	cutGraph(net, 0, reachable);

	for (int vertex = 0; vertex < verticies.size(); vertex ++) {
		if (verticies[vertex].type == 's')
			s[verticies[vertex].index] = reachable[vertex] ? '0' : '1';
	
		else if (verticies[vertex].type == 'p')
			p[verticies[vertex].index] = reachable[vertex] ? '0' : '1';
	}

	return hammingDistance;
}


int main () {
	string s, p;
	cin >> s >> p;
	cout << solveProblem(s, p) << endl;
	cout << s << endl << p << endl;
	return 0;
}
