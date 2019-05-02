#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>


using namespace std;




// Graph: --------------------------------------------------


template <class Edge>
class Graph {
	private:
		vector<list<Edge> > edges;

	public:
		Graph () {}

		Graph (const typename Edge::IndexType &verticesCount) {
			edges.clear();
			edges.resize(verticesCount);
		}

		void addEdge (const Edge &edge) {
			edges[edge.from].push_back(edge);
		}

		const list<Edge>& getConstOuterEdges (const typename Edge::IndexType &vertex) const {
			return edges[vertex];
		}

		list<Edge>& getOuterEdges (const typename Edge::IndexType &vertex) const {
			return edges[vertex];
		}

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

struct CharedEdge {
	typedef unsigned IndexType;
	IndexType from, to;
	char ch;

	CharedEdge (const IndexType &from, const IndexType &to, const char &ch) {
		this->from = from;
		this->to = to;
		this->ch = ch;
	}

	void print (ostream &os) const {
		os << "(" << from << " -> " << to << ")";
	}
};


// Automate: --------------------------------------------------


class Automate {
	private:
		struct AutomateVertex {
			static const ushort alphabetFirstSymbolIndex = 'a';
			static const ushort alphabetSize = 26;

			int parent;
			char edgeSymbol;
			vector<int> edges;
			int suffixLink;
			vector<int> automateMove;
			int terminalLink;
			unsigned terminationStringsCount;

			AutomateVertex (const int &parent = -1, const char &edgeSymbol = '\0') {
				this->parent = parent;
				this->edgeSymbol = edgeSymbol;
				edges = vector<int>(alphabetSize, -1);
				suffixLink = -1;
				automateMove = vector<int>(alphabetSize, -1);
				terminalLink = -1;
				terminationStringsCount = 0;
			}
		};

		vector<AutomateVertex> vertices;

		
		void setEdge (AutomateVertex &vertex, const char &edgeSymbol, const unsigned &edgeVertexIndex) {
			vertex.edges[edgeSymbol - vertex.alphabetFirstSymbolIndex] = edgeVertexIndex;
		}
		
		long long getEdge (const AutomateVertex &vertex, const char &edgeSymbol) {
			return vertex.edges[edgeSymbol - vertex.alphabetFirstSymbolIndex];
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
			vertex.automateMove[edgeSymbol - vertex.alphabetFirstSymbolIndex] = automateMoveVertexIndex;
		}

		long long getAutomateMove (const AutomateVertex &vertex, const char &edgeSymbol) {
			return vertex.automateMove[edgeSymbol - vertex.alphabetFirstSymbolIndex];
		}

		unsigned getTerminalLink (const unsigned &vertexIndex) {
			if (vertices[vertexIndex].terminalLink == -1) {
				unsigned suffixLink = getSuffixLink(vertexIndex);

				if (suffixLink == 0)
					vertices[vertexIndex].terminalLink = 0;

				else
					vertices[vertexIndex].terminalLink = (vertices[suffixLink].terminationStringsCount > 0 ? suffixLink : getTerminalLink(suffixLink));
			}

			return vertices[vertexIndex].terminalLink;
		}

	public:
		Automate () {
			vertices.push_back(AutomateVertex());
		}

		Automate (const vector<string> &strings) {
			vertices.push_back(AutomateVertex());

			for (const string &str: strings)
				addStringToAutomate(str);
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
			
			vertices[currentVertexIndex].terminationStringsCount += 1;
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

		unsigned getTerminationStringsCount (const unsigned &vertexIndex) {
			unsigned terminationStringsCount = 0;
			unsigned currentVertexIndex = vertexIndex;

			while (currentVertexIndex != 0) {
				if (vertices[currentVertexIndex].terminationStringsCount != 0)
					terminationStringsCount += vertices[currentVertexIndex].terminationStringsCount;

				currentVertexIndex = getTerminalLink(currentVertexIndex);
			}

			return terminationStringsCount;
		}
};


size_t dfs (const Graph<CharedEdge> &trie, Automate &automate, const unsigned vertex = 0, const unsigned &state = 0) {
	size_t count = automate.getTerminationStringsCount(state);

	for (const CharedEdge &edge: trie.getConstOuterEdges(vertex))
		count += dfs(trie, automate, edge.to, automate.makeAutomateMove(state, edge.ch));

	return count;
}


int main () {
	unsigned verticesCount;
	cin >> verticesCount;

	Graph<CharedEdge> trie(verticesCount);

	for (unsigned from = 0; from < verticesCount; from ++) {
		unsigned nextCount;
		cin >> nextCount;

		for (unsigned i = 0; i < nextCount; i ++) {
			unsigned to;
			cin >> to;
			char ch;
			cin >> ch;

			trie.addEdge(CharedEdge(from, to - 1, ch));
		}
	}

	//trie.print(cout);

	unsigned stringsCount;
	cin >> stringsCount;
	vector<string> strings(stringsCount);

	for (string &str: strings)
		cin >> str;

	Automate automate(strings);
	
	cout << dfs(trie, automate, 0) << endl;

	return 0;
}
