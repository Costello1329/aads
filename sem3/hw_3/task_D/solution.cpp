#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>


using namespace std;


class SuffixAutomaton {
	private:
		struct Node {
			Node *suffixLink;
			vector<Node*> edges;
			size_t maxLength;

			Node () {
				edges.resize(26, nullptr);
			}
		};

		Node *root;
		Node *last;

	public:
		explicit SuffixAutomaton (const string &str = "") {
			root = new Node();
			root->suffixLink = nullptr;
			root->maxLength = 0;
			last = root;

			for (const char ch: str)
				addSymbol(ch);
		}

		void addSymbol (const char &ch) {
			Node *newLast = new Node;
			newLast->maxLength = last->maxLength + 1;
			newLast->suffixLink = nullptr;

			// Connect newLast to automaton:
			Node *current = last;
			
			while (current != nullptr && current->edges[ch - 'a'] == nullptr) {
				current->edges[ch - 'a'] = newLast;
				current = current->suffixLink;
			}

			// Calculate suffix link:
			if (current == nullptr)
				newLast->suffixLink = root;

			else if (current->maxLength + 1 == current->edges[ch - 'a']->maxLength) 
				newLast->suffixLink = current->edges[ch - 'a'];

			else {
				Node *nextCurrent = current->edges[ch - 'a'];
				Node *clone = new Node;
				clone->suffixLink = nextCurrent->suffixLink;
				clone->maxLength = nextCurrent->maxLength;
				clone->edges = nextCurrent->edges;

				clone->maxLength = current->maxLength + 1;
				nextCurrent->suffixLink = newLast->suffixLink = clone;

				while (current != nullptr && current->edges[ch - 'a'] == nextCurrent) {
					current->edges[ch - 'a'] = clone;
					current = current->suffixLink;
				}
			}

			// Update last:
			last = newLast;
		}

		bool checkForSubstr () {
			Node *current = root;
			bool flag = true;

			while (true) {
				char ch = getc(stdin);

				if (ch == '\n')
					break;				

				else if (ch < 'a' || ch > 'z')
					ch = ch + 'a' - 'A';
				

				if (current->edges[ch - 'a'] == nullptr)
					flag = false;

				else
					current = current->edges[ch - 'a'];
			}

			return flag;
		}
};


void toLowerCase(string &str) {
	for (char &ch: str)
		if (ch >= 'A' && ch <= 'Z')
			ch = 'a' + (ch - 'A');
}

int main () {
	SuffixAutomaton suffixAutomaton;

	while (true) {
		char command = getc(stdin);

		if (command == 'A') {
			getc(stdin);
			while (true) {
				char ch = getc(stdin);

				if (ch == '\n')
					break;

				else if (ch < 'a' || ch > 'z')
					ch = ch + 'a' - 'A';

				suffixAutomaton.addSymbol(ch);
			}
		}

		else if (command == '?') {
			getc(stdin);
			const bool flag = suffixAutomaton.checkForSubstr();
			printf("%s\n", flag ? "YES" : "NO");
		}

		else
			break;
	}

	return 0;
}
