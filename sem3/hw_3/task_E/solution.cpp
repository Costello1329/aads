#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>


using namespace std;

const size_t maxLength = 1000000000;



struct Refrain {
	string str;
	size_t occuranceCount;
};


// Suffix tree:


class SuffixTree {
	private:
		struct Node {
			map<char, Node*> edge;
			Node *parent;
			size_t edgeLeft;
			size_t edgeLength;
			size_t num;
			Node *suffixLink = nullptr;
			size_t occuranceCount = 0;
			size_t distance = 0;

			Node () {};
			Node (Node *other) : edge(other->edge), parent(other->parent), edgeLeft(other->edgeLeft), edgeLength(other->edgeLength), suffixLink(other->suffixLink) {}
		};

		struct Position { 
			Node *node;
			size_t distance;

			Position () : node(nullptr), distance(0) {}
			Position (Node *node, const size_t &distance) : node(node), distance(distance) {}

			// RULE 2:
			void splitEdge (const string &str, size_t &size) {
				Node *middle = new Node();
				middle->edge[str[node->edgeLeft + node->edgeLength - distance]] = node;
				middle->parent = node->parent;
				middle->parent->edge[str[node->edgeLeft]] = middle;
				middle->edgeLeft = node->edgeLeft;
				middle->edgeLength = node->edgeLength - distance;
				middle->num = (size ++);
				middle->suffixLink = nullptr;

				node->parent = middle;
				node->edgeLeft += middle->edgeLength;
				node->edgeLength = distance;

				this->node = middle;
				distance = 0;
			}

			void appendEdge (const char &ch, const string &str, size_t &size) {
				Node *newLeaf = new Node();
				newLeaf->num = (size ++);
				newLeaf->parent = node;
				newLeaf->edgeLength = maxLength;
				newLeaf->edgeLeft = str.size();
				newLeaf->suffixLink = nullptr;

				node->edge[ch] = newLeaf;
			}

			// RULE 3:
			bool canMoveBySymbol (const char &ch, const string &str) {
				if (distance == 0)
					return node->edge[ch] != nullptr;

				else
					return str[node->edgeLeft + node->edgeLength - distance] == ch;
			}

			// This method launches only when we can move by symbol ch:
			void moveBySymbol (const char &ch) {
				if (distance == 0) {
					node = node->edge[ch];
					distance = node->edgeLength - 1;
				}

				else
					distance --;
			}
		};

		size_t size;
		string str;
		Node *root;
		Position posLastNotLeaf;
		size_t lastNotLeaf;

	public:
		SuffixTree (const string &str) {
			root = new Node();
			root->num = ((size = 0) ++);
			root->parent = nullptr;
			root->edgeLeft = 0;
			root->edgeLength = 0;

			lastNotLeaf = 0;
			posLastNotLeaf = Position(root, 0);
 
			for (size_t i = 0; i < str.size(); i ++) {
				const char ch = str[i];
				addSymbol(ch);
			}
		}

		Refrain getRefrain () {
			setTerminals();
			setOccuranceCount(root);
			Node *refrainNode = getRefrainNode(root);
			string refrainString = getRefrainString(refrainNode);

			Refrain refrain;
			refrain.occuranceCount = refrainNode->occuranceCount;
			refrain.str = refrainString;
			return refrain;
		}

		void addSymbol (const char &ch) {
			while (true) {
				if (lastNotLeaf > str.size())
					break;

				// RULE 3:
				if (posLastNotLeaf.canMoveBySymbol(ch, str)) {
					posLastNotLeaf.moveBySymbol(ch);
					break;
				}

				// RULE 2:
				else {
					if (posLastNotLeaf.distance != 0) {
						posLastNotLeaf.splitEdge(str, size);
						posLastNotLeaf.node->suffixLink = nullptr;
					}

					posLastNotLeaf.appendEdge(ch, str, size);

					if (posLastNotLeaf.node != root)
						posLastNotLeaf = Position(countSuffixLink(posLastNotLeaf.node), 0);
					
					lastNotLeaf ++;
				}
			}

			str += ch;
		}

	private:
		Node* countSuffixLink (Node *node) {
			if (node->suffixLink != nullptr)
				return node->suffixLink;

			else {
				// Определяем откуда и по какой подстроке будем спускаться:
				Node *halfSuffixLink;
				size_t substringLeft;
				size_t substringLength;

				if (node->parent != root) {
					halfSuffixLink = countSuffixLink(node->parent);
					substringLeft = node->edgeLeft;
					substringLength = node->edgeLength;
				}

				else {
					halfSuffixLink = root;
					substringLeft = node->edgeLeft + 1;
					substringLength = node->edgeLength - 1;
				}

				return node->suffixLink = goDown(halfSuffixLink, substringLeft, substringLength);
			}
		}

		Node* goDown (Node *halfSuffixLink, const size_t &substringLeft, const size_t &substringLength) {
			// Объявляем счетчик, который равен пройденному пути от halfSuffixLink:
			size_t currentLength = 0;

			// Спускаемся много раз:
			while (currentLength < substringLength) {
				halfSuffixLink = halfSuffixLink->edge[str[substringLeft + currentLength]];
				currentLength += halfSuffixLink->edgeLength;
			}

			// Если пришли в вершину, тогда все хорошо:
			if (currentLength == substringLength)
				return halfSuffixLink;

			// Иначе:
			else {
				// Сдвигаемся по ребру:
				Position position(halfSuffixLink, halfSuffixLink->edgeLength);
				position.distance = currentLength - substringLength;

				// Разделяем ребро в этом месте и возвращаем вершину на ребре:
				position.splitEdge(str, size);
				return position.node;
			}	
		}

		void setTerminals () {
			Position endPos = Position(root, 0);

			for (const char &ch: str)
				endPos.moveBySymbol(ch);

			endPos.splitEdge(str, size);

			Node *cur = endPos.node;

			while (cur != root) {
				cur->occuranceCount = 1;
				cur = countSuffixLink(cur);
			}
		}

		size_t setOccuranceCount (Node *node) {
			node->distance = (node->parent != nullptr ? node->parent->distance : 0) + node->edgeLength;

			for (auto it = node->edge.begin(); it != node->edge.end(); it ++)
				if (it->second != nullptr)
					node->occuranceCount += setOccuranceCount(it->second);

			return node->occuranceCount;
		}

		Node* getRefrainNode (Node *node) {
			if (node->distance > str.size())
				return nullptr;

			Node* best = node;

			for (auto it = node->edge.begin(); it != node->edge.end(); it ++) {
				if (it->second != nullptr) {
					Node* nextBest = getRefrainNode(it->second);
					if (nextBest == nullptr)
						continue;

					else if (nextBest->distance * nextBest->occuranceCount > best->distance * best->occuranceCount)
						best = nextBest;
				}
			}

			return best;
		}

		string getRefrainString (Node *refrainNode) {
			string refrainString = "";
			Node *cur = refrainNode;

			while (cur != nullptr) {
				string edge = str.substr(cur->edgeLeft, cur->edgeLength);
				reverse(edge.begin(), edge.end());
				refrainString += edge;

				cur = cur->parent;
			}

			reverse(refrainString.begin(), refrainString.end());
			return refrainString;
		}


	public:
		// PRINT ALL SUBSTRINGS:
		void printAllSubstringsDFS (ostream &os, Node *node, const size_t &depth = 0) {
			for (size_t i = 0; i < depth; i ++)
				os << "  ";

			if (node != root) {
				os << str.substr(node->edgeLeft, min<size_t>(node->edgeLength, str.size() - node->edgeLeft));
				os << "[" << node->parent->num << "] ";
				os << "(" << node->num << ", " << node->occuranceCount << ", " << node->distance << ")" << endl;
			}
			
			else
				os << "X" << endl;

			for (auto it = node->edge.begin(); it != node->edge.end(); it ++)
				if (it->second != nullptr)
					printAllSubstringsDFS(os, it->second, depth + 1);
		}

		void printAllSubstrings (ostream &os) {
			printAllSubstringsDFS(os, root);
		}


		// COUNT ALL SUBSTRINGS:
		size_t countAllSubstringsDFS (Node *node) {
			size_t count = min<size_t>(node->edgeLength, str.size() - node->edgeLeft);

			for (auto it = node->edge.begin(); it != node->edge.end(); it ++)
				if (it->second != nullptr)
					count += countAllSubstringsDFS(it->second);

			return count;
		}

		size_t countAllSubstrings () {
			return countAllSubstringsDFS(root);
		}
};


// Suffix automaton class: -------------------------------------------------------------------------


template <class NodeData>
class SuffixAutomaton {
	private:
		struct Node {
			NodeData data;
			map<char, Node*> edge;
			Node *suffixLink;
			Node *parent;
			char parentEdgeChar;
			size_t longestStringInEquivalentClassLength;
			bool deleted;

			explicit Node (Node *suffixLink = nullptr, Node *parent = nullptr, const char &parentEdgeChar = '\0', const size_t longestStringInEquivalentClassLength = 0) : suffixLink(suffixLink), parent(parent), parentEdgeChar(parentEdgeChar), longestStringInEquivalentClassLength(longestStringInEquivalentClassLength), deleted(false) {}
		} *root, *last;

		void addSymbol (const char &ch) {
			// Create newLast:
			Node *newLast = new Node(nullptr, last, ch, last->longestStringInEquivalentClassLength + 1);

			// Connect newLast to automaton:		
			Node *current = last;
			
			while (current != nullptr && current->edge[ch] == nullptr) {
				current->edge[ch] = newLast;
				current = current->suffixLink;
			}

			// Calculate suffixLink:
			if (current == nullptr)
				newLast->suffixLink = root;

			else if (current->longestStringInEquivalentClassLength + 1 == current->edge[ch]->longestStringInEquivalentClassLength) 
				newLast->suffixLink = current->edge[ch];

			else {
				Node *nextCurrent = current->edge[ch];

				Node *clone = new Node(nextCurrent->suffixLink, current, ch, current->longestStringInEquivalentClassLength + 1);
				clone->edge = nextCurrent->edge;
				nextCurrent->suffixLink = newLast->suffixLink = clone;

				while (current != nullptr && current->edge[ch] == nextCurrent) {
					current->edge[ch] = clone;
					current = current->suffixLink;
				}
			}

			// Update last:
			last = newLast;
		}

		void deleteNode (Node *node) {
			if (node->deleted || node == nullptr)
				return;

			node->deleted = true;

			for (auto edgeIter = node->edge.begin(); edgeIter != node->edge.end(); edgeIter ++)
				deleteNode(edgeIter->second);

			delete node;
			node = nullptr;
		}

	public:
		class NodeIterator {
			private:
				Node *node;

				void invalid () const {
					if (node == nullptr)
						throw "Invalid SuffixAutomaton NodeIterator\n";
				}

			public:
				NodeIterator () {
					node = nullptr;
				}

				NodeIterator (const NodeIterator &other) {
					node = other.node;
				}

				explicit NodeIterator (Node *node) : node(node) {}

				explicit operator bool () const {
					return node != nullptr;
				}

				const NodeData& getData () const {
					invalid();
					return node->data;
				}

				void setData (const NodeData &data) const {
					node->data = data;
				}

				bool canMoveByEdge (const char &ch) const {
					invalid();
					return node->edge[ch] != nullptr;
				}

				NodeIterator moveByEdge (const char &ch) const {
					invalid();
					return NodeIterator(node->edge[ch]);
				}

				map<char, NodeIterator> getEdges () const {
					map<char, NodeIterator> edge;

					for (auto iter = node->edge.begin(); iter != node->edge.end(); iter ++)
						edge[iter->first] = NodeIterator(iter->second);

					return edge;
				}

				bool canMoveBySuffixLink () const {
					invalid();
					return node->suffixLink != nullptr;
				}

				NodeIterator moveBySuffixLink () const {
					invalid();
					return NodeIterator(node->suffixLink);
				}

				bool canMoveByParent () const {
					invalid();
					return node->parent != nullptr;
				}

				NodeIterator moveByParent () const {
					invalid();
					return NodeIterator(node->parent);
				}

				char getParentEdgeChar () const {
					invalid();
					return node->parentEdgeChar;
				}

				size_t getMaxLength () const {
					invalid();
					return node->longestStringInEquivalentClassLength;
				}

				size_t getNum () const {
					return node->num;
				}
		};

		NodeIterator begin () {
			return NodeIterator(root);
		}

		explicit SuffixAutomaton (const string &str = "") {
			last = root = new Node();
			root->suffixLink = nullptr;
			root->parent = nullptr;
			root->parentEdgeChar = '\0';
			root->longestStringInEquivalentClassLength = 0;

			for (const char &ch: str)
				addSymbol(ch);
		}

		~SuffixAutomaton () {
			deleteNode(root);
		}
};


// Refrain automaton solver class: -----------------------------------------------------------------


class AutomatonRefrainSolver {
	private:
		struct RefrainNodeData {
			size_t equivalenceClassSize;
			bool used;

			RefrainNodeData (const size_t equivalenceClassSize = 0, const bool used = false) : equivalenceClassSize(equivalenceClassSize), used(used) {}
		};

		string str;
		SuffixAutomaton<RefrainNodeData> *suffixAutomaton;
		typedef SuffixAutomaton<RefrainNodeData>::NodeIterator NodeIterator;
		NodeIterator refrainIterator;


		void setEquivalenceCountsOnTerminalVertices () {
			NodeIterator iter = suffixAutomaton->begin();

			for (const char &ch: str)
				iter = iter.moveByEdge(ch);

			NodeIterator newIter = iter;

			while (iter.canMoveBySuffixLink()) {
				iter.setData(RefrainNodeData(1, false));
				iter = iter.moveBySuffixLink();
			}
		}

		void setEquivalenceCountsInReverseTopSortOrder (NodeIterator iter) {
			RefrainNodeData data = iter.getData();
			data.used = true;
			iter.setData(data);

			map<char, NodeIterator> edge = iter.getEdges();

			for (auto edgeIter = edge.begin(); edgeIter != edge.end(); edgeIter ++)
				if (!edgeIter->second.getData().used)
					setEquivalenceCountsInReverseTopSortOrder(edgeIter->second);

			edge = iter.getEdges();

			for (auto edgeIter = edge.begin(); edgeIter != edge.end(); edgeIter ++) {
				RefrainNodeData data = iter.getData();
				data.equivalenceClassSize += edgeIter->second.getData().equivalenceClassSize;
				iter.setData(data);
			}

			if (!(bool)refrainIterator || iter.getData().equivalenceClassSize * iter.getMaxLength() > refrainIterator.getData().equivalenceClassSize * refrainIterator.getMaxLength())
				refrainIterator = iter;
		}

		string getRefrainString () {
			string refrainString = "";

			NodeIterator current = refrainIterator;

			while (current.canMoveByParent()) {
				refrainString += current.getParentEdgeChar();
				current = current.moveByParent();
			}

			reverse(refrainString.begin(), refrainString.end());

			return refrainString;
		}

	public:
		AutomatonRefrainSolver (const string &str) : str(str) {
			// unique_ptr:
			suffixAutomaton = new SuffixAutomaton<RefrainNodeData>(str);

			setEquivalenceCountsOnTerminalVertices();
			setEquivalenceCountsInReverseTopSortOrder(suffixAutomaton->begin());
		}

		Refrain operator () () {
			Refrain refrain;
			refrain.str = getRefrainString();
			refrain.occuranceCount = refrainIterator.getData().equivalenceClassSize;

			return refrain;
		}

		~AutomatonRefrainSolver () {
			delete suffixAutomaton;
		}
};


// Problem: ----------------------------------------------------------------------------------------


string input (istream &is) {
	unsigned strSize;
	is >> strSize;

	ushort alphabetSize;
	is >> alphabetSize;

	string str;

	for (unsigned i = 0; i < strSize; i ++) {
		ushort num;
		is >> num;
		num --;
		str.push_back('0' + num);
	}

	return str;
}

void output (ostream &os, const Refrain &refrain) {
	os << refrain.occuranceCount * refrain.str.size() << endl;
	os << refrain.str.size() << endl;

	for (const char &ch: refrain.str)
		os << ((ushort)(ch - '0') + 1) << " ";

	os << endl;
}

int main () {
	string str = input(cin);
	AutomatonRefrainSolver automatonRefrainSolver(str);
	output(cout, automatonRefrainSolver());
	return 0;
}
