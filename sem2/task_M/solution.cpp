#include <fstream>
#include <iostream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::istream;
using std::vector;
using std::make_pair;
using std::ofstream;
using std::ostream;
using std::pair;


class Graph {
	private:
		vector<vector<unsigned> > edges;

	public:
		Graph (unsigned verticesCount = 0) {
			edges = vector<vector<unsigned> >(verticesCount);
		}

		void addEdge (unsigned from, unsigned to) {
			if (std::max<unsigned>(from, to) >= edges.size())
				edges.resize(std::max<unsigned>(from, to) + 1);

			edges[from].push_back(to);
		}

		void getEdges (unsigned vertex, vector<unsigned> &edges) const {
			edges = this->edges[vertex];
		}

		unsigned size () const {
			return edges.size();
		}
};


enum vertexType {lose, draw, win};

bool buildGraphs (istream &is, Graph &graph, Graph &transposedGraph) {
	size_t vertices;
	size_t edges;

	if (!(is >> vertices >> edges))
		return false;

	graph = Graph(vertices);
	transposedGraph = Graph(vertices);

	for (size_t i = 0; i < edges; i ++) {
		size_t from, to;
		is >> from >> to;
		from --;
		to --;

		graph.addEdge(from, to);
		transposedGraph.addEdge(to, from);
	}

	return true;
}

void premarkLose (Graph &graph, vector<vertexType> &marks) {
	for (size_t vertex = 0; vertex < graph.size(); vertex ++) {
		vector<unsigned> edges;
		graph.getEdges(vertex, edges);

		if (edges.size() == 0)
			marks[vertex] = vertexType::lose;
	}
}

void dfs (Graph &graph, Graph &transposedGraph, unsigned vertex, vector<vertexType> &marks, vector<bool> &used, vector<unsigned> &winCount) {
	if (used[vertex])
		return;

	used[vertex] = true;
	vector<unsigned> edges;
	transposedGraph.getEdges(vertex, edges);

	for (unsigned next: edges) {
		if (marks[next] != vertexType::draw)
			continue;

		if (marks[vertex] == vertexType::lose) {
			marks[next] = vertexType::win;
			dfs(graph, transposedGraph, next, marks, used, winCount);
		}

		else if (marks[vertex] == vertexType::win) {
			vector<unsigned> winEdges;
			graph.getEdges(next, winEdges);

			if (++ winCount[next] == winEdges.size()) {
				marks[next] = vertexType::lose;
				dfs(graph, transposedGraph, next, marks, used, winCount);
			}
		}
	}
}

void retroAnalysis (Graph &graph, Graph &transposedGraph, vector<vertexType> &marks) {
	vector<bool> used(marks.size());
	vector<unsigned> winCount(marks.size(), 0);

	for (unsigned vertex = 0; vertex < marks.size(); vertex ++)
		if (marks[vertex] == vertexType::lose)
			dfs(graph, transposedGraph, vertex, marks, used, winCount);
}

void printMarks (ostream &os, vector<vertexType> &marks) {
	for (unsigned i = 0; i < marks.size(); i ++) {
		if (marks[i] == vertexType::win)
			os << "FIRST" << endl;
		
		else if (marks[i] == vertexType::lose)
			os << "SECOND" << endl;

		else if (marks[i] == vertexType::draw)
			os << "DRAW" << endl;
	}
}


bool solve (istream &is, ostream &os) {
	Graph graph, transposedGraph;
	
	if (!buildGraphs(is, graph, transposedGraph))
		return false;
	
	vector<vertexType> marks(transposedGraph.size(), vertexType::draw);
	premarkLose(graph, marks);
	retroAnalysis(graph, transposedGraph, marks);
	printMarks(os, marks);
	return true;
}


int main () {
	cin.tie(0);
	std::ios_base::sync_with_stdio(false);

	while (true) {
		if (solve(cin, cout))
			cout << endl;

		else
			break;
	}

	return 0;
}
