#include <iostream>
#include <vector>


using namespace std;


template <typename Type>
ostream& operator << (ostream &os, const vector<Type> &vec) {
	for (Type el: vec)
		os << el << " ";

	return os;
}


// Edge:

struct Edge {
	unsigned from;
	unsigned to;

	Edge (const unsigned &from = 0, const unsigned &to = 0) {
		this->from = from;
		this->to = to;
	}

	void scan (istream &is = cin) {
		is >> from >> to;
	}

	void print (ostream &os = cout) const {
		os << from << " -> " << to;
	}
};

ostream& operator << (ostream &os, const Edge &edge) {
	edge.print(os);
	return os;
}

bool operator == (const Edge &firstEdge, const Edge &secondEdge) {
	return firstEdge.from == secondEdge.from && firstEdge.to == secondEdge.to;
}

template <class Edge>
Edge reverseEdge (const Edge &edge) {
	Edge reversedEdge = edge;
	reversedEdge.from = edge.to;
	reversedEdge.to = edge.from;
	return reversedEdge;
}


// Graph:

template <class Edge>
class Graph {
	private:
	public:
		vector<vector<Edge> > edges;

		Graph (const unsigned &verticesCount = 0) {
			edges.resize(verticesCount);
		}

		void addEdge (const Edge &edge) {
			edges[edge.from].push_back(edge);
		}

		bool isEdgeIn (const Edge &edge) const {
			for (const Edge &currentEdge: edges[edge.from])
				if (currentEdge == edge)
					return true;

			return false;
		}

		vector<Edge> const & getOuterEdges (const unsigned &vertex) const {
			return this->edges[vertex];
		}

		unsigned verticesCount () const {
			return edges.size();
		}

		void print (ostream &os = cout) const {
			for (unsigned vertex = 0; vertex < edges.size(); vertex ++) {
				os << vertex << ": ";
		
				for (Edge edge: edges[vertex]) {
					edge.print(os);
					os << " ";
				}
		
				os << endl;
			}
		}
};


// Max pairing:

template <class Edge>
bool maxPairingDFS (const Graph<Edge> &graph, const unsigned &vertex, vector<bool> &used, vector<int> &pairing) {
	if (used[vertex])
		return false;

	used[vertex] = true;

	for (const Edge &edge: graph.getOuterEdges(vertex)) {
		if (pairing[edge.to] == -1 || maxPairingDFS(graph, pairing[edge.to], used, pairing)) {
			pairing[edge.to] = edge.from;
			return true;
		}
	}

	return false;
}

template <class Edge>
void findMaxPairingInBigraph (const Graph<Edge> &graph, vector<Edge> &edges) {
	vector<int> pairing(graph.verticesCount(), -1);
	vector<bool> used(graph.verticesCount(), false);

	for (unsigned vertex = 0; vertex < graph.verticesCount(); vertex ++) {
		if (maxPairingDFS(graph, vertex, used, pairing))
			used.assign(used.size(), false);
	}

	for (unsigned i = 0; i < graph.verticesCount(); i ++)
		if (pairing[i] != -1)
			edges.push_back(Edge(pairing[i], i));
}


// Min controll set:

enum Part {Left, Right};

template <class Edge>
void minControllSetDFS (const Graph<Edge> &graph, const unsigned &vertex, vector<bool> &used) {
	if (used[vertex])
		return;

	used[vertex] = true;

	for (const Edge &edge: graph.getOuterEdges(vertex))
		minControllSetDFS(graph, edge.to, used);
}

template <class Edge>
void findMinControllSetOfBigraph (const Graph<Edge> &graph, const vector<Part> &parts, vector<unsigned> &controllSet) {
	vector<Edge> maxPairing;
	findMaxPairingInBigraph(graph, maxPairing);

	Graph<Edge> newGraph(graph.verticesCount());

	for (const Edge &edge: maxPairing)
		newGraph.addEdge(reverseEdge(edge));

	for (unsigned vertex = 0; vertex < graph.verticesCount(); vertex ++)
		for (const Edge &edge: graph.getOuterEdges(vertex))
			if (!newGraph.isEdgeIn(reverseEdge(edge)))
				newGraph.addEdge(edge);

	vector<bool> isVertexInPairing(newGraph.verticesCount(), false);

	for (const Edge &edge: maxPairing) {
		isVertexInPairing[edge.from] = true;
		isVertexInPairing[edge.to] = true;
	}


	vector<bool> used(newGraph.verticesCount(), false);

	for (unsigned vertex = 0; vertex < newGraph.verticesCount(); vertex ++)
		if (parts[vertex] == Left && !isVertexInPairing[vertex])
			minControllSetDFS(newGraph, vertex, used);


	for (unsigned vertex = 0; vertex < newGraph.verticesCount(); vertex ++)
		if ((!used[vertex] && parts[vertex] == Left && graph.getOuterEdges(vertex).size() != 0) || (used[vertex] && parts[vertex] == Right))
			controllSet.push_back(vertex);
}


// Graph utils:

template <typename Edge>
void transitiveClosureDFS (const Graph<Edge> &graph, const unsigned &vertex, vector<bool> &used) {
	if (used[vertex])
		return;

	used[vertex] = true;

	for (const Edge &edge: graph.getOuterEdges(vertex))
		transitiveClosureDFS(graph, edge.to, used);
}

template <typename Edge>
Graph<Edge> buildTransitiveClosure (const Graph<Edge> &graph) {
	Graph<Edge> transitiveClosureGraph(graph.verticesCount());

	for (unsigned vertex = 0; vertex < graph.verticesCount(); vertex ++) {
		vector<bool> used(graph.verticesCount(), false);
		transitiveClosureDFS(graph, vertex, used);

		for (unsigned to = 0; to < graph.verticesCount(); to ++) {
			if (used[to] && vertex != to) {
				Edge edge;
				edge.from = vertex;
				edge.to = to;
				transitiveClosureGraph.addEdge(edge);
			}
		}
	}
	
	return transitiveClosureGraph;
}


// Partially ordered set's graph utils:

template <typename Edge>
void buildBiGraphOfPOS (const Graph<Edge> &graph, Graph<Edge> &biGraph, vector<Part> &parts) {
	biGraph = Graph<Edge>(2 * graph.verticesCount());

	for (unsigned vertex = 0; vertex < graph.verticesCount(); vertex ++)
		for (const Edge &edge: graph.getOuterEdges(vertex))
			biGraph.addEdge(Edge(edge.from, graph.verticesCount() + edge.to));

	parts.resize(2 * graph.verticesCount());
	
	for (unsigned i = 0; i < graph.verticesCount(); i ++) {
		parts[i] = Left;
		parts[i + graph.verticesCount()] = Right;
	}
}

template <typename Edge>
vector<unsigned> findMaxAntichainInPOS (const Graph<Edge> &graph) {
	vector<Part> parts;
	Graph<Edge> biGraph;
	buildBiGraphOfPOS(graph, biGraph, parts);


	vector<unsigned> controllSetOfBigraph;
	findMinControllSetOfBigraph(biGraph, parts, controllSetOfBigraph);


	vector<bool> controllSetOfBigraphCharacteristic(2 * graph.verticesCount(), false);

	for (const unsigned &el: controllSetOfBigraph)
		controllSetOfBigraphCharacteristic[el] = true;


	vector<unsigned> maxAntichain;

	for (unsigned i = 0; i < graph.verticesCount(); i ++)
		if (controllSetOfBigraphCharacteristic[i] == false && controllSetOfBigraphCharacteristic[i + graph.verticesCount()] == false)
			maxAntichain.push_back(i + 1);

	return maxAntichain;
}


int main () {
	unsigned hobbitsCount;
	cin >> hobbitsCount;


	Graph<Edge> graph(hobbitsCount);

	for (unsigned to = 0; to < hobbitsCount; to ++) {
		for (unsigned from = 0; from < hobbitsCount; from ++) {
			bool isGreater;
			cin >> isGreater;

			if (isGreater)
				graph.addEdge(Edge(from, to));
		}
	}


	Graph<Edge> transitiveClosureGraph = buildTransitiveClosure(graph);
	vector<unsigned> maxAntichain = findMaxAntichainInPOS(transitiveClosureGraph);


	cout << maxAntichain.size() << endl;

	for (const unsigned &el: maxAntichain)
		cout << el << " ";
	
	cout << endl;

	return 0;
}
