#include <algorithm>
#include <iostream>
#include <climits>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>

typedef unsigned Data;

using namespace std;


/* Node class: */

struct Node {
	Data data;
	signed priority;
	Node *left;
	Node *right;
	Node *parent;
	size_t weight;
	bool reversed;
	bool isCycle;

	Node (Data data) {
		this->data = data;
		this->priority = rand();
		this->left = nullptr;
		this->right = nullptr;
		this->parent = nullptr;
		this->weight = 1;
		this->reversed = false;
		this->isCycle = false;
	}

	Node (const Node *other) {
		this->data = other->data;
		this->priority = other->priority;
		this->left = other->left;
		this->right = other->right;
		this->parent = other->parent;
		this->weight = other->weight;
		this->reversed = other->reversed;
		this->isCycle = false;
	}
};

void update (Node *node) {
	if (node == nullptr)
		return;

	node->weight = 1 + (node->left == nullptr ? 0 : node->left->weight) + (node->right == nullptr ? 0 : node->right->weight);
}

void push (Node *node) {
	if (node == nullptr || !node->reversed)
		return;

	if (node->left != nullptr)
		node->left->reversed ^= true;

	if (node->right != nullptr)
		node->right->reversed ^= true;

	swap(node->left, node->right);
	node->reversed = false;
}

void pushSubTree (Node *node) {
	if (node == nullptr)
		return;

	push(node);

	if (node->left)
		pushSubTree(node->left);

	if (node->right)
		pushSubTree(node->right);
}

pair<Node*, Node*> split (size_t index, Node *node) {
	if (node == nullptr)
		return {nullptr, nullptr};

	push(node);
	size_t currentIndex = (node->left != nullptr ? node->left->weight : 0) + 1;

	if (currentIndex <= index) {
		pair<Node*, Node*> treap = split(index - currentIndex, node->right);
		node->right = treap.first;
		
		if (treap.first)
			treap.first->parent = node;

		if (treap.second)
			treap.second->parent = nullptr;
		
		update(node);
		return {node, treap.second};
	}

	else {
		pair<Node*, Node*> treap = split(index, node->left);
		node->left = treap.second;

		if (treap.first)
			treap.first->parent = nullptr;

		if (treap.second)
			treap.second->parent = node;
		
		update(node);
		return {treap.first, node};
	}
}

Node* merge (Node *&left, Node *&right) {
	push(left);
	push(right);

	if (left == nullptr)
		return right;

	if (right == nullptr)
		return left;

	if (left->priority > right->priority) {
		left->right = merge(left->right, right);

		if (left->right)
			left->right->parent = left;

		update(left);
		return left;
	}

	else {
		right->left = merge(left, right->left);

		if (right->left)
			right->left->parent = right;
		
		update(right);
		return right;
	}
}

Node *getRoot (Node *node) {
	if (node == nullptr)
		return node;

	while (node->parent != nullptr)
		node = node->parent;

	return node;
}

size_t getPos (Node *node) {
	stack<Node*> nodeStack;

	while (node->parent != nullptr) {
		nodeStack.push(node);
		node = node->parent;	
	}

	size_t index = (node->left != nullptr ? node->left->weight : 0);

	while (!nodeStack.empty()) {
		Node *child = nodeStack.top();
		nodeStack.pop();
		push(node);
		push(node->left);
		push(node->right);

		if (node->left == child) {
			index -= 1 + (node->left->right != nullptr ? node->left->right->weight : 0);
			node = node->left;
		}

		else {
			index += 1 + (node->right->left != nullptr ? node->right->left->weight : 0);
			node = node->right;
		}
	}

	return index;
}


/* Treap class: */

class Treap {
	private:
		Node *root;

	public:
		// Constructors:

		explicit Treap (Node *root = nullptr) {
			this->root = root;
		}

		explicit Treap (vector<Data> &array) {
			root = nullptr;
			set(array);
		}

		// Setters:

		void set (vector<Data> &array) {
			root = nullptr;

			for (Data data: array)
				insert(data);
		}

		// Getters:

		Node *getRoot () {
			return root;
		}

		size_t size () {
			update(root);
			
			if (root)
				return root->weight;

			else
				return 0;
		}

		Data get (size_t pos) {
			pair<Node*, Node*> treap1 = split(pos, root);
			pair<Node*, Node*> treap2 = split(1, treap1.second);
			Data data = treap2.first->data;
			treap1.second = merge(treap2.first, treap2.second);
			root = merge(treap1.first, treap1.second);
			return data;
		}

		void inOrderTraversal (vector<Node*> &nodes, Node *node = nullptr) const {
			if (node == nullptr) {
				if (root != nullptr)
					node = root;

				else
					return;
			}

			if (node->left != nullptr)
				inOrderTraversal(nodes, node->left);

			nodes.push_back(node);

			if (node->right != nullptr)
				inOrderTraversal(nodes, node->right);
		}

		vector<Data> getArray (Node *node = nullptr) {
			pushSubTree(root);
			vector<Data> array;
			
			if (node == nullptr) {
				if (root == nullptr)
					return array;

				else
					node = root;
			}

			vector<Node*> nodes;
			inOrderTraversal(nodes, root);

			for (Node *node: nodes)
				array.push_back(node->data);

			return array;
		}

		// Main operations:

		void insert (Data data, size_t pos = INT_MAX - 1) {
			pair<Node*, Node*> treap1 = split(pos, root);
			Node *insertion = new Node(data);
			treap1.first = merge(treap1.first, insertion);
			root = merge(treap1.first, treap1.second);
		}

		void remove (size_t pos = INT_MAX - 1) {
			pair<Node*, Node*> treap1 = split(pos - 1, root);
			pair<Node*, Node*> treap2 = split(1, treap1.second);
			delete treap2.first;
			treap2.first = nullptr;
			root = merge(treap1.first, treap2.second);
		}

		// Subsegment methods:

		void reverseOnSubsegment (size_t l, size_t r) {
			pair<Node*, Node*> treap1 = split(l, root);
			pair<Node*, Node*> treap2 = split(r - l + 1, treap1.second);
			treap2.first->reversed ^= true;
			treap1.second = merge(treap2.first, treap2.second);
			root = merge(treap1.first, treap1.second);
		}

		void moveSubsegmentToFront (size_t l, size_t r) {
			Node *left, *middle, *right;
			pair<Node*, Node*> treap1 = split(l, root);
			pair<Node*, Node*> treap2 = split(r - l + 1, treap1.second);
			treap1.first = merge(treap2.first, treap1.first);
			root = merge(treap1.first, treap2.second);
		}

		// Debug

		void print () const {
			queue<Node*> que;
			que.push(root);

			while (!que.empty()) {
				Node *current = que.front();
				que.pop();
		
				if (current == nullptr)
					continue;
		
				cout << current->data << " (" << (current->parent ? current->parent->data : -1) << ", " << current->priority << "): left – " << (current->left == nullptr ? -1 : current->left->data) << " right – " << (current->right == nullptr ? -1 : current->right->data) << endl;
				que.push(current->left);
				que.push(current->right);
			}
		}
};


/* Graph class: */

class Graph {
	private:
		vector<vector<unsigned> > edges;

	public:
		Graph (unsigned V = 0) {
			edges.resize(V, vector<unsigned>(0));
		}

		void addEdge (unsigned from, unsigned to) {
			if (max(from, to) >= edges.size())
				edges.resize(max(from, to) + 1);

			edges[from].push_back(to);
		}

		void getEdges (unsigned vertex, vector<unsigned> &edges) const {
			edges = this->edges[vertex];
		}

		unsigned size () const {
			return edges.size();
		}

		void print () const {
			for (unsigned i = 0; i < edges.size(); i ++) {
				cout << i << ": ";

				for (unsigned next: edges[i])
					cout << next << ", ";

				cout << endl;
			}
		}
};

void dfs (const Graph &graph, unsigned vertex, vector<unsigned> &path, vector<bool> &used) {
	vector<unsigned> edges;
	graph.getEdges(vertex, edges);
	path.push_back(vertex);
	used[vertex] = true;

	for (unsigned next: edges)
		if (!used[next])
			dfs(graph, next, path, used);
}

signed getStartVertex (const Graph &graph, vector<unsigned> &path) {
	for (unsigned node: path) {
		vector<unsigned> edges;
		graph.getEdges(node, edges);

		if (edges.size() <= 1)
			return node;
	}

	return -1;
}


/* Solving the problem: */

Graph buildGraph (istream &is, unsigned &queriesCount) {
	unsigned verticesCount, edgesCount;
	is >> verticesCount >> edgesCount >> queriesCount;

	Graph graph(verticesCount);

	for (unsigned i = 0; i < edgesCount; i ++) {
		unsigned from, to;
		is >> from >> to;
		
		if (from != to) {
			from --;
			to --;
			graph.addEdge(from, to);
			graph.addEdge(to, from);
		}
	}

	return graph;
}

vector<Node*> buildForest (const Graph &graph) {
	vector<bool> used(graph.size(), false);
	vector<Node*> forest(graph.size(), nullptr);

	for (unsigned vertex = 0; vertex < graph.size(); vertex ++) {
		if (used[vertex])
			continue;

		vector<unsigned> path;
		dfs(graph, vertex, path, used);

		signed startVertex = getStartVertex(graph, path);
		
		if (startVertex != -1) {
			vector<bool> newUsed(graph.size(), false);
			path.clear();
			dfs(graph, startVertex, path, newUsed);
		}
		
		Treap treap(path);

		if (startVertex == -1)
			treap.getRoot()->isCycle = true;

		vector<Node*> nodes;
		treap.inOrderTraversal(nodes);

		for (Node *node: nodes)
			forest[node->data] = node;
	}

	return forest;
}

void processQueries (unsigned queriesCount, vector<Node*> &forest, istream &is, ostream &os) {
	for (unsigned q = 0; q < queriesCount; q ++) {
		char type;
		unsigned from, to;
		cin >> type >> from >> to;
		from --;
		to --;

		unsigned fromPos = getPos(forest[from]);
		unsigned toPos = getPos(forest[to]);
		Node *fromRoot = getRoot(forest[from]);
		Node *toRoot = getRoot(forest[to]);
		Treap fromTreap(fromRoot);
		Treap toTreap(toRoot);

		if (type == '+') {
			if (fromRoot != toRoot) {
				if (from == fromTreap.get(fromTreap.size() - 1) && to == toTreap.get(0))
					merge(fromRoot, toRoot);
				
				else if (from == fromTreap.get(0) && to == toTreap.get(toTreap.size() - 1))
					merge(toRoot, fromRoot);
				
				else if (from == fromTreap.get(0) && to == toTreap.get(0)) {
					fromTreap.reverseOnSubsegment(0, fromTreap.size() - 1);
					fromRoot = fromTreap.getRoot();
					merge(fromRoot, toRoot);
				}

				else if (from == fromTreap.get(fromTreap.size() - 1) && to == toTreap.get(toTreap.size() - 1)) {
					toTreap.reverseOnSubsegment(0, toTreap.size() - 1);
					toRoot = toTreap.getRoot();
					merge(fromRoot, toRoot);
				}

				fromRoot = getRoot(fromRoot);
				toRoot = getRoot(toRoot);
				fromRoot->isCycle = false;
				toRoot->isCycle = false;
				fromTreap = Treap(fromRoot);
				toTreap = Treap(toRoot);
			}

			else {
				fromRoot->isCycle = true; 
			}
		}

		else if (type == '-') {
			if (fromRoot->isCycle == true) {
				if (fromPos > toPos) {
					swap(fromPos, toPos);
					swap(from, to);
				}

				if (fromPos == 0 && toPos == fromTreap.size() - 1) {;}

				else
					fromTreap.moveSubsegmentToFront(toPos, fromTreap.size() - 1);

				fromRoot = getRoot(fromRoot);
				toRoot = fromRoot;
				fromRoot->isCycle = false;
				fromTreap = Treap(fromRoot);
				toTreap = Treap(toRoot);
			}

			else {
				unsigned splitPos = max<unsigned>(fromPos, toPos);
				pair<Node*, Node*> splitted = split(splitPos, fromRoot);
				fromRoot = getRoot(splitted.first);
				toRoot = getRoot(splitted.second);
				fromRoot->isCycle = false;
				toRoot->isCycle = false;
				fromTreap = Treap(fromRoot);
				toTreap = Treap(toRoot);
			}
		}

		else if (type == '?') {
			if (from == to) {
				os << 0 << endl;
				continue;
			}

			if (getRoot(forest[from]) != getRoot(forest[to]))
				os << -1 << endl;

			else {
				unsigned delta = abs((signed)fromPos - (signed)toPos);

				if (fromRoot->isCycle == false)
					os << delta - 1 << endl;

				else
					os << min<unsigned>(delta, fromTreap.size() - delta) - 1 << endl;
			}
		}
	}
}

void solve (istream &is, ostream &os) {
	unsigned queriesCount;
	Graph graph = buildGraph(is, queriesCount);
	vector<Node*> forest = buildForest(graph);
	processQueries(queriesCount, forest, is, os);
	
	for (Node *node: forest)
		delete node;
}


/* Main: */

int main () {
	solve(cin, cout);
	return 0;
}
