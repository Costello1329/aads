#include <iostream>
#include <fstream>
#include <vector>
#include <list>

using namespace std;


// Класс (Ориентированного) статического (нельзя менять кол-во вершин и удалять ребра) графа:
template <class Edge>
class Graph {
	private:
		vector<list<Edge> > edges;

	public:
		// Конструкторы:
		Graph () {}

		Graph (const typename Edge::IndexType &verticesCount) {
			edges.clear();
			edges.resize(verticesCount);
		}

		// Добавить ребро:
		void addEdge (const Edge &edge) {
			edges[edge.from].push_back(edge);
		}

		// Получить константные изходящие ребра:
		const list<Edge>& getConstOuterEdges (const typename Edge::IndexType &vertex) const {
			return edges[vertex];
		}

		// Получить изходящие ребра (с возможностью что-то в них отредактировать и даже сломать):
		list<Edge>& getOuterEdges (const typename Edge::IndexType &vertex) const {
			return edges[vertex];
		}

		// Получить кол-во вершин в графе:
		typename Edge::IndexType getVerticesCount () const {
			return edges.size();
		}

		void print (ostream &os) const {
			for (typename Edge::IndexType vertex = 0; vertex < getVerticesCount(); vertex ++) {
				os << vertex << ": ";

				for (const Edge &edge: edges[vertex]) {
					edge.print(os);
					os << " ";
				}

				os << endl;
			}
		}
};


class Automate {
	private:
		struct AutomateVertex {
			int parent;
			char edgeSymbol;
			vector<int> edges;
			int suffixLink;
			vector<int> automateMove;
			int terminalLink;
			vector<unsigned> terminationStringsIndexes;

			AutomateVertex (const int &parent = -1, const char &edgeSymbol = '\0') {
				this->parent = parent;
				this->edgeSymbol = edgeSymbol;
				edges = vector<int>(2, -1);
				suffixLink = -1;
				automateMove = vector<int>(2, -1);
				terminalLink = -1;
				terminationStringsIndexes = vector<unsigned>(0);
			}
		};

		vector<AutomateVertex> vertices;
		vector<string> strings;

		
		void setEdge (AutomateVertex &vertex, const char &edgeSymbol, const unsigned &edgeVertexIndex) {
			vertex.edges[edgeSymbol - '0'] = edgeVertexIndex;
		}
		
		int getEdge (const AutomateVertex &vertex, const char &edgeSymbol) {
			return vertex.edges[edgeSymbol - '0'];
		}
 
		unsigned getSuffixLink (const unsigned &vertexIndex) {
			if (vertices[vertexIndex].suffixLink == -1) {
				if (vertexIndex == 0 || vertices[vertexIndex].parent == 0)
					vertices[vertexIndex].suffixLink = 0;
				
				else
					vertices[vertexIndex].suffixLink = makeAutomateMove(getSuffixLink(vertices[vertexIndex].parent), vertices[vertexIndex].edgeSymbol);
			}

			return vertices[vertexIndex].suffixLink;
		}

		void setAutomateMove (AutomateVertex & vertex, const char &edgeSymbol, const unsigned &automateMoveVertexIndex) {
			vertex.automateMove[edgeSymbol - '0'] = automateMoveVertexIndex;
		}

		int getAutomateMove (const AutomateVertex &vertex, const char &edgeSymbol) {
			return vertex.automateMove[edgeSymbol - '0'];
		}

		unsigned getTerminalLink (const unsigned &vertexIndex) {
			if (vertices[vertexIndex].terminalLink == -1) {
				unsigned suffixLink = getSuffixLink(vertexIndex);

				if (suffixLink == 0)
					vertices[vertexIndex].terminalLink = 0;

				else
					vertices[vertexIndex].terminalLink = (vertices[suffixLink].terminationStringsIndexes.size() > 0 ? suffixLink : getTerminalLink(suffixLink));
			}

			return vertices[vertexIndex].terminalLink;
		}

	public:
		Automate () {
			vertices.push_back(AutomateVertex());
		}

		void addStringToAutomate (const string &str) {
			unsigned currentVertexIndex = 0;

			for (const char &ch: str) {
				if (getEdge(vertices[currentVertexIndex], ch) != -1)
					currentVertexIndex = getEdge(vertices[currentVertexIndex], ch);

				else {
					vertices.push_back(AutomateVertex(currentVertexIndex, ch));
					unsigned nextVertexIndex = vertices.size() - 1;
					setEdge(vertices[currentVertexIndex], ch, nextVertexIndex);
					currentVertexIndex = nextVertexIndex;
				}
			}
			
			strings.push_back(str);
			vertices[currentVertexIndex].terminationStringsIndexes.push_back(strings.size() - 1);
		}

		unsigned makeAutomateMove (const unsigned &vertexIndex, const char &edgeSymbol) {
			if (getAutomateMove(vertices[vertexIndex], edgeSymbol) == -1) {
				if (getEdge(vertices[vertexIndex], edgeSymbol) != -1)
					setAutomateMove(vertices[vertexIndex], edgeSymbol, getEdge(vertices[vertexIndex], edgeSymbol));

				else
					setAutomateMove(vertices[vertexIndex], edgeSymbol, vertexIndex == 0 ? 0 : makeAutomateMove(getSuffixLink(vertexIndex), edgeSymbol));
			}

			return getAutomateMove(vertices[vertexIndex], edgeSymbol);
		}

		bool isStateTerminal (const unsigned &vertexIndex) {
			unsigned currentVertexIndex = vertexIndex;

			while (currentVertexIndex != 0) {
				if (vertices[currentVertexIndex].terminationStringsIndexes.size() != 0)
					return true;

				currentVertexIndex = getTerminalLink(currentVertexIndex);
			}

			return false;
		}

		unsigned getStatesCount () {
			return vertices.size();
		}
};

// Алгоритм (dfs с помечением цвета ребер):
enum vertexColor {white, grey, black};

template <typename Edge>
bool doesCycleExistDFS (const Graph<Edge> &graph, const typename Edge::IndexType &vertex, vector<vertexColor> &colors) {
	if (colors[vertex] == grey)
		return true;

	else if (colors[vertex] == black)
		return false;

	colors[vertex] = grey;

	for (const Edge &edge: graph.getConstOuterEdges(vertex))
		if (doesCycleExistDFS(graph, edge.to, colors))
			return true;

	colors[vertex] = black;
	return false;
}

template <typename Edge>
bool doesCycleExist (const Graph<Edge> &graph) {
	vector<vertexColor> colors(graph.getVerticesCount(), white);
	return doesCycleExistDFS(graph, 0, colors);
}

struct SimpleEdge {
	typedef unsigned IndexType;
	IndexType from, to;

	SimpleEdge (const IndexType &from, const IndexType &to) {
		this->from = from;
		this->to = to;
	}

	void print (ostream &os) const {
		os << "(" << from << " -> " << to << ")";
	}
};


int main () {
	Automate automate;

	unsigned patternsCount;
	cin >> patternsCount;

	for (unsigned i = 0; i < patternsCount; i ++) {
		string str;
		cin >> str;
		automate.addStringToAutomate(str);
	}


	Graph<SimpleEdge> graph(automate.getStatesCount());

	for (unsigned i = 0; i < automate.getStatesCount(); i ++) {
		if (automate.isStateTerminal(i))
			continue;

		if (!automate.isStateTerminal(automate.makeAutomateMove(i, '0')))
			graph.addEdge(SimpleEdge(i, automate.makeAutomateMove(i, '0')));
		
		if (!automate.isStateTerminal(automate.makeAutomateMove(i, '1')))
			graph.addEdge(SimpleEdge(i, automate.makeAutomateMove(i, '1')));
	}


	cout << (doesCycleExist(graph) ? "TAK" : "NIE") << endl;

	
	return 0;
}
