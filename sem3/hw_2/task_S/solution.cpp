#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


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
				edges = vector<int>(95, -1);
				suffixLink = -1;
				automateMove = vector<int>(95, -1);
				terminalLink = -1;
				terminationStringsIndexes = vector<unsigned>(0);
			}
		};

		vector<AutomateVertex> vertices;
		vector<string> strings;

		
		void setEdge (AutomateVertex &vertex, const char &edgeSymbol, const unsigned &edgeVertexIndex) {
			vertex.edges[edgeSymbol - 32] = edgeVertexIndex;
		}
		
		int getEdge (const AutomateVertex &vertex, const char &edgeSymbol) {
			return vertex.edges[edgeSymbol - 32];
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
			vertex.automateMove[edgeSymbol - 32] = automateMoveVertexIndex;
		}

		int getAutomateMove (const AutomateVertex &vertex, const char &edgeSymbol) {
			return vertex.automateMove[edgeSymbol - 32];
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
};


int main () {
	ifstream cin("input.txt");
	string patternsCount;
	getline(cin, patternsCount);

	vector<string> patterns(stoi(patternsCount), "");

	for (string &str: patterns)
		getline(cin, str);



	Automate automate;

	for (const string &pattern: patterns)
		automate.addStringToAutomate(pattern);

	vector<string> answer;


	while (!cin.eof()) {
		string str;
		getline(cin, str);

		unsigned automateState = 0;
		
		for (const char &ch: str) {
			automateState = automate.makeAutomateMove(automateState, ch);
			
			if (automate.isStateTerminal(automateState)) {
				answer.push_back(str);
				break;
			}
		}
	}


	for (const string &str: answer)
		cout << str << '\n';

	return 0;
}
