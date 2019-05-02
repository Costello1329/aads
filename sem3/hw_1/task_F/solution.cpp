#include <iostream>
#include <vector>


using namespace std;


template <typename Type>
ostream& operator << (ostream &os, const vector<Type> &vec) {
	for (Type el: vec)
		os << el << " ";

	return os;
}


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
class Graph {
	private:
	public:
		vector<vector<Edge> > edges;

		Graph (const unsigned &verticesCount) {
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
void findMaxPairing (const Graph<Edge> &graph, vector<Edge> &edges) {
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


template <class Edge>
Edge reverseEdge (const Edge &edge) {
	Edge reversedEdge = edge;
	reversedEdge.from = edge.to;
	reversedEdge.to = edge.from;
	return reversedEdge;
}


template <class Edge>
void minControllSetDFS (const Graph<Edge> &graph, const unsigned &vertex, vector<bool> &used) {
	//cout << "dfs: " << vertex << endl;

	if (used[vertex])
		return;

	used[vertex] = true;

	for (const Edge &edge: graph.getOuterEdges(vertex))
		minControllSetDFS(graph, edge.to, used);
}


enum Part {Left = 0, Right = 1};

template <class Edge>
void findMinControllSet (const Graph<Edge> &graph, const vector<Part> &parts, vector<unsigned> &controllSet) {
	vector<Edge> maxPairing;
	findMaxPairing(graph, maxPairing);

	//cout << "max pairing: " << maxPairing << endl;

	Graph<Edge> newGraph(graph.verticesCount());

	for (const Edge &edge: maxPairing)
		newGraph.addEdge(reverseEdge(edge));

	for (unsigned vertex = 0; vertex < graph.verticesCount(); vertex ++)
		for (const Edge &edge: graph.getOuterEdges(vertex))
			if (!newGraph.isEdgeIn(reverseEdge(edge)))
				newGraph.addEdge(edge);

	//cout << endl << "newGraph" << endl;
	//newGraph.print();
	//cout << endl;

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


template <typename Edge>
Graph<Edge> inverse (const Graph<Edge> &graph, vector<Part> &parts) {
	Graph<Edge> inverseGraph(graph.verticesCount());

	for (unsigned from = 0; from < graph.verticesCount(); from ++)
		for (unsigned to = 0; to < graph.verticesCount(); to ++)
			if (!graph.isEdgeIn(Edge(from, to)) && !graph.isEdgeIn(Edge(to, from)) && parts[from] == Right && parts[to] == Left) {
				inverseGraph.addEdge(Edge(to, from));
			}

	//cout << endl << "inverseGraph" << endl;
	//inverseGraph.print();
	//cout << endl;
	
	return inverseGraph;
}


/*


1  
5 5 
1 2 0
2 3 4 5 0
2 3 4 5 0
4 5 0
5 0


*/


int main () {
	unsigned k;
	cin >> k;

	for (unsigned t = 0; t < k; t ++) {
		unsigned malesCount, femalesCount;
		cin >> malesCount >> femalesCount;

		Graph<Edge> graph(malesCount + femalesCount);

		for (unsigned from = 0; from < malesCount; from ++) {
			while (true) {
				unsigned to;
				cin >> to;
				
				if (to == 0)
					break;

				else
					graph.addEdge(Edge(from, malesCount + to - 1));
			}
		}

		vector<Part> parts(malesCount + femalesCount);

		for (unsigned i = 0; i < parts.size(); i ++) {
			if (i < malesCount)
				parts[i] = Left;

			else
				parts[i] = Right;
		}


		vector<unsigned> controllSetOnInverseGraph;
		findMinControllSet(inverse(graph, parts), parts, controllSetOnInverseGraph);
		//cout << "control set on inverse graph: " << controllSetOnInverseGraph << endl;


		vector<bool> maximalBicliqueCharacteristic(graph.verticesCount(), true);

		for (const unsigned &vertex: controllSetOnInverseGraph)
			maximalBicliqueCharacteristic[vertex] = false;

		vector<unsigned> maximalBiclique;

		for (unsigned vertex = 0; vertex < graph.verticesCount(); vertex ++)
			if (maximalBicliqueCharacteristic[vertex])
				maximalBiclique.push_back(vertex);

		//cout << "max clique: " << maximalBiclique << endl;



		unsigned maxBicliqueSize = maximalBiclique.size();
		unsigned maxBicliqueLeftSize = 0;
		unsigned maxBicliqueRightSize;

		for (unsigned vertex: maximalBiclique)
			if (parts[vertex] == Left)
				maxBicliqueLeftSize ++;

		maxBicliqueRightSize = maxBicliqueSize - maxBicliqueLeftSize;

		cout << maxBicliqueSize << endl;
		cout << maxBicliqueLeftSize << " " << maxBicliqueRightSize << endl;

		for (unsigned vertex: maximalBiclique)
			if (parts[vertex] == Left)
				cout << vertex + 1 << " ";

		cout << endl;

		for (unsigned vertex: maximalBiclique)
			if (parts[vertex] == Right)
				cout << vertex - malesCount + 1 << " ";

		cout << endl;
	}

	return 0;
}
