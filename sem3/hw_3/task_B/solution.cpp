#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <array>

using namespace std;


struct PalindromicTree {
	struct Node {
		array<Node*, 26> next;
		Node *suffixLink;
		size_t occuranceCount;
		long long left, right;
		vector<Node*> reversedLinks;

		Node () {
			for (Node *&node: next)
				node = nullptr;
		}
		
		long long length () const {
			return right - left;
		}
	} *oddRoot, *evenRoot, *lastPalindrome;

	string str;


	explicit PalindromicTree (const string &str) : str("") {
		oddRoot = new Node();
		oddRoot->suffixLink = oddRoot;
		oddRoot->left = 1;
		oddRoot->right = 0;
		oddRoot->occuranceCount = 0;
		evenRoot = new Node();
		evenRoot->suffixLink = oddRoot;
		evenRoot->left = 0;
		evenRoot->right = 0;
		evenRoot->occuranceCount = 0;

		oddRoot->reversedLinks = {evenRoot};

		lastPalindrome = evenRoot;

		for (const char &ch: str)
			addSymbol(ch);

		relaxTree();
	}

	~PalindromicTree () {
		clearDFS(oddRoot);
		clearDFS(evenRoot);
	}

	void clearDFS (Node *node) {
		for (char ch = 'a'; ch <= 'z'; ch ++) {
			if (node->next[ch - 'a'] == nullptr)
				continue;
			
			clearDFS(node->next[ch - 'a']);
		}

		delete node;
	}


	bool canExtend (Node *node, const char &ch) {
		if (node == oddRoot)
			return true;

		else if (node == evenRoot)
			return str.size() > 0 && str.back() == ch;

		else {
			const long long index = str.size() - 1 - node->length();
			return index >= 0 && str[index] == ch;
		}
	}

	Node* extend (Node *node, const char &ch) {
		bool isNew = (node->next[ch - 'a'] == nullptr);

		if (isNew) {
			node->next[ch - 'a'] = new Node();
			node->next[ch - 'a']->reversedLinks.clear();
			node->next[ch - 'a']->occuranceCount = 0;
		}

		Node *newNode = node->next[ch - 'a'];
		newNode->left = str.size() - node->length() - 1;
		newNode->right = str.size() + 1;
		newNode->occuranceCount ++;

		if (newNode->length() == 1)
			newNode->suffixLink = evenRoot;

		else {
			do node = node->suffixLink;
			while (!canExtend(node, ch));

			newNode->suffixLink = node->next[ch - 'a'];
		}

		if (isNew)
			newNode->suffixLink->reversedLinks.push_back(newNode);

		return newNode;
	}

	void addSymbol (const char &ch) {
		Node *current = lastPalindrome;

		while (!canExtend(current, ch))
			current = current->suffixLink;

		lastPalindrome = extend(current, ch);
		str.push_back(ch);
	}

	void relaxTree () {
		relaxTreeDFS(oddRoot);
		oddRoot->occuranceCount = 0;
		evenRoot->occuranceCount = 0;
	}

	size_t relaxTreeDFS (Node *node) {
		for (Node *reversedLink: node->reversedLinks)
			node->occuranceCount += relaxTreeDFS(reversedLink);

		return node->occuranceCount;
	}


	// DEBUG:


	void print (ostream &os) {
		printDFS(os, evenRoot, 1);
		printDFS(os, oddRoot, 1);
		os << "last: ";
		printNode(os, lastPalindrome);
		os << endl;
	}

	void printNode (ostream &os, Node *node, bool flag = true) {
		if (node == nullptr)
			os << "X";

		else if (node == evenRoot)
			os << "even";

		else if (node == oddRoot)
			os << "odd";

		else {
			os << str.substr(node->left, node->length());
			
			if (flag) {
				os << "[" << node->left << "; " << node->right << ")";
				os << "{" << node->occuranceCount << "}";
			}
		}
	}

	void printDFS (ostream &os, Node *node, const size_t &depth) {
		if (node != oddRoot && node != evenRoot) {
			printNode(os, node);
			os << ", (";
			printNode(os, node->suffixLink, false);
			os << ")" << endl;
		}

		else if (node == oddRoot)
			os << "odd" << endl;

		else
			os << "even" << endl;
		
		for (char ch = 'a'; ch <= 'z'; ch ++) {
			if (node->next[ch - 'a'] == nullptr)
				continue;

			for (size_t i = 0; i < depth; i ++)
				os << "\t";

			os << "--" << ch << "->\t";
			
			printDFS(os, node->next[ch - 'a'], depth + 1);
		}
	}
};


size_t solveDFS (const PalindromicTree &first, const PalindromicTree &second, PalindromicTree::Node *firstNode, PalindromicTree::Node *secondNode) {
	size_t count = firstNode->occuranceCount * secondNode->occuranceCount;

	for (char ch = 'a'; ch <= 'z'; ch ++) {
		if (firstNode->next[ch - 'a'] == nullptr || secondNode->next[ch - 'a'] == nullptr)
			continue;

		count += solveDFS(first, second, firstNode->next[ch - 'a'], secondNode->next[ch - 'a']);
	}

	return count;
}

size_t solve (const PalindromicTree &first, const PalindromicTree &second) {
	size_t count = 0;
	count += solveDFS(first, second, first.oddRoot, second.oddRoot);
	count += solveDFS(first, second, first.evenRoot, second.evenRoot);
	return count;
}


int main () {
	unsigned T;
	cin >> T;

	for (unsigned i = 0; i < T; i ++) {	
		string str1, str2;
		cin >> str1 >> str2;

		PalindromicTree first(str1);
		PalindromicTree second(str2);

		cout << "Case #" << (i + 1) << ": " << solve(first, second) << "\n";
	}

	return 0;
}
