#include <iostream>
#include <limits>
#include <vector>
#include <string>
#include <list>

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
			list<unsigned> terminationStringsIndexes;

			AutomateVertex (const int &parent = -1, const char &edgeSymbol = '\0') {
				this->parent = parent;
				this->edgeSymbol = edgeSymbol;
				edges = vector<int>(10, -1);
				suffixLink = -1;
				automateMove = vector<int>(10, -1);
				terminalLink = -1;
				terminationStringsIndexes = list<unsigned>(0);
			}
		};

		vector<AutomateVertex> vertices;

		
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

		void addStringToAutomate (const string &str, const unsigned &index) {
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

			vertices[currentVertexIndex].terminationStringsIndexes.push_back(index);
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

		list<unsigned> getTerminalLinksTerminationStringsIndexes (const unsigned &vertexIndex) {
			list<unsigned> terminationStringIndexes;

			unsigned currentVertexIndex = vertexIndex;

			while (currentVertexIndex != 0) {
				if (vertices[currentVertexIndex].terminationStringsIndexes.size() != 0)
					for (const unsigned &terminationStringIndex: vertices[currentVertexIndex].terminationStringsIndexes)
						terminationStringIndexes.push_back(terminationStringIndex);

				currentVertexIndex = getTerminalLink(currentVertexIndex);
			}

			return terminationStringIndexes;
		}
};


vector<list<unsigned> > findSubstringsInString (const string& text, const vector<string> &patterns) {
	Automate automate;
	
	for (unsigned i = 0; i < patterns.size(); i ++)
		automate.addStringToAutomate(patterns[i], i);	

	//automate.debug();


	vector<list<unsigned> > findings(patterns.size());
	unsigned automateState = 0;
	
	for (unsigned charIndex = 0; charIndex < text.size(); charIndex ++) {
		automateState = automate.makeAutomateMove(automateState, text[charIndex]);
		list<unsigned> terminationStringIndexes = automate.getTerminalLinksTerminationStringsIndexes(automateState);
		
		for (const unsigned &terminationStringIndex: terminationStringIndexes)
			findings[terminationStringIndex].push_back(charIndex - patterns[terminationStringIndex].size() + 1);
	}

	return findings;
}


pair<unsigned, int> solveDp (const vector<string> &patterns, const vector<list<unsigned> > &patternsStartingAtIndexIndexes, vector<pair<unsigned, int> > &dp, vector<bool> &used, const unsigned &index = 0) {
	if (used[index])
		return dp[index];

	used[index] = true;

	// ----------------------------------------------------------------------------------------------------

	dp[index] = make_pair(numeric_limits<unsigned>::max(), -1);

	for (const unsigned &patternIndex: patternsStartingAtIndexIndexes[index]) {
		if (index + patterns[patternIndex].size() == dp.size())
			return dp[index] = make_pair(1, patternIndex);

		else {
			pair<unsigned, int> betterValue = solveDp(patterns, patternsStartingAtIndexIndexes, dp, used, index + patterns[patternIndex].size());
			betterValue.second = patternIndex;
			dp[index] = min<pair<unsigned, int> >(dp[index], betterValue);
		}
	}

	if (dp[index].first != numeric_limits<unsigned>::max()) {
		dp[index].first ++;
		return dp[index];
	}

	else
		return dp[index];
}


int main () {
	while (true) {
		string text;
		cin >> text;
	
		if (text[0] == '-')
			break;
	
		const char decimals[26] = {2,2,2,3,3,3,4,4,1,1,5,5,6,6,0,7,0,7,7,8,8,8,9,9,9,0};
		unsigned patternsCount;
		cin >> patternsCount;
		vector<string> patterns;
		vector<string> words;
	
		for (unsigned patternIndex = 0; patternIndex < patternsCount; patternIndex ++) {
			string word;
			cin >> word;
			words.push_back(word);
	
			for (char &ch: word)
				ch = decimals[ch - 'a'] + '0';
	
			patterns.push_back(word);
		}
	
	
		vector<list<unsigned> > findings = findSubstringsInString(text, patterns);
	
	
		vector<pair<unsigned, int> > dp(text.size());
		vector<bool> used(text.size(), false);
		vector<list<unsigned> > patternsStartingAtIndexIndexes(text.size());
	
		for (unsigned patternIndex = 0; patternIndex < patternsCount; patternIndex ++)
			for (const unsigned &i: findings[patternIndex])
				patternsStartingAtIndexIndexes[i].push_back(patternIndex);
	
	
		solveDp(patterns, patternsStartingAtIndexIndexes, dp, used);
	
	
		if (dp[0].first == numeric_limits<unsigned>::max())
			cout << "No solution." << endl;
	
		else {
			unsigned index = 0;
			
			while (index < text.size()) {
				pair<unsigned, int> val = dp[index];
	
				if (val.second == -1)
					break;
	
				else {
					cout << words[val.second] << " ";
					index += patterns[val.second].size();
				}
			}
	
			cout << endl;
		}
	}

	return 0;
}
