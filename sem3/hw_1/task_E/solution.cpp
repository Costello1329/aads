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

	void print (ostream &os = cout) {
		os << from << " -> " << to;
	}
};

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
bool dfs (const Graph<Edge> &graph, const unsigned &vertex, vector<bool> &used, vector<int> &matching) {
	if (used[vertex])
		return false;

	used[vertex] = true;

	for (const Edge &edge: graph.getOuterEdges(vertex)) {
		if (matching[edge.to] == -1 || dfs(graph, matching[edge.to], used, matching)) {
			matching[edge.to] = edge.from;
			return true;
		}
	}

	return false;
}


template <class Edge>
void findMaxMatchingInBipartiteGraph (const Graph<Edge> &graph, vector<Edge> &edges) {
	vector<int> matching(graph.verticesCount(), -1);
	vector<bool> used(graph.verticesCount(), false);

	for (unsigned vertex = 0; vertex < graph.verticesCount(); vertex ++) {
		if (dfs(graph, vertex, used, matching))
			used.assign(used.size(), false);
	}

	for (unsigned i = 0; i < graph.verticesCount(); i ++)
		if (matching[i] != -1)
			edges.push_back(Edge(matching[i], i));
}


enum fieldColor {black = 0, white = 1};

fieldColor getColor (const unsigned &i, const unsigned &j) {
	return (fieldColor)((i + j) % 2);
}



int main () {
	// Считываем исходные данные:

	unsigned height, width, A, B;
	cin >> height >> width >> A >> B;

	unsigned brokenCount = 0;
	vector<vector<int> > brokenVertexIndex(height, vector<int>(width, -1));

	for (unsigned i = 0; i < height; i ++) {
		for (unsigned j = 0; j < width; j ++) {
			char c;
			cin >> c;

			if (c == '*')
				brokenVertexIndex[i][j] = (brokenCount ++);
		}
	}


	// Эвристика:

	if (A >= 2 * B) {
		cout << brokenCount * B << endl;
		return 0;
	}


	// Строим граф:
	Graph<Edge> graph(brokenCount);

	for (unsigned i = 0; i < height; i ++) {
		for (unsigned j = 0; j < width; j ++) {
			if (brokenVertexIndex[i][j] == -1 || getColor(i, j) == black)
				continue;

			if (j != 0 && brokenVertexIndex[i][j - 1] != -1)
				graph.addEdge(Edge(brokenVertexIndex[i][j], brokenVertexIndex[i][j - 1]));

			if (j != width - 1 && brokenVertexIndex[i][j + 1] != -1)
				graph.addEdge(Edge(brokenVertexIndex[i][j], brokenVertexIndex[i][j + 1]));

			if (i != 0 && brokenVertexIndex[i - 1][j] != -1)
				graph.addEdge(Edge(brokenVertexIndex[i][j], brokenVertexIndex[i - 1][j]));

			if (i != height - 1 && brokenVertexIndex[i + 1][j] != -1)
				graph.addEdge(Edge(brokenVertexIndex[i][j], brokenVertexIndex[i + 1][j]));
		}
	}


	// Находим максимальное паросочетание:

	vector<Edge> maxMatching;
	findMaxMatchingInBipartiteGraph(graph, maxMatching);
	unsigned maxMatchingSize = 2 * maxMatching.size();
	cout << maxMatchingSize / 2 * A + (brokenCount - maxMatchingSize) * B << endl;

	return 0;
}
