#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <queue>

typedef int Data;

using namespace std;

struct Node {
	Data key;
	int priority;
	int weight = 1;
	Node *left;
	Node *right;

	Node () {
		left = nullptr;
		right = nullptr;
	}

	Node (Data key, int priority) {
		this->key = key;
		this->priority = priority;
		left = nullptr;
		right = nullptr;
	}
};

class Treap {
	private:
		Node *root;

		void update (Node *node) {
  			node->weight = 1 + (node->left == nullptr ? 0 : node->left->weight) + (node->right == nullptr ? 0 : node->right->weight);
		}

		void split (Node *current, Data key, Node *&left, Node *&right) {
			if (current == nullptr) {
				left = right = nullptr;
				return;
			}

			if (current->key < key) {
				split(current->right, key, current->right, right);
				left = current;
				update(current);
			}

			else {
				split(current->left, key, left, current->left);
				right = current;
				update(current);
			}
		}

		Node* merge (Node *&left, Node *&right) {
			if (left == nullptr)
				return right;
		
			if (right == nullptr)
				return left;
		
			if (left->priority > right->priority) {
				left->right = merge(left->right, right);
				update(left);
				return left;
			}
		
			else {
				right->left = merge(left, right->left);
				update(right);
				return right;
			}
		}

	public:
		int size;

		Treap (Node *root) {
			this->root = root;
			size = 1;
		}

		Treap (Data key, int priority) {
			root = new Node(key, priority);
			size = 1;
		}

		Treap () {
			root = nullptr;
			size = 0;
		}

		Node *getRoot () const {
			return root;
		}

		void insert (Data key, int priority) {
			size ++;
			Node *left;
			Node *right;
			split(root, key, left, right);
			Node *insertion = new Node(key, priority);
			Node *newLeft = merge(left, insertion);
			root = merge(newLeft, right);
		}

		void remove (Data key) {
			size --;
			Node *left;
			Node *right;
			split(root, key + 1, left, right);
			Node *deletion;
			split(left, key, left, deletion);
			root = merge(left, right);
			delete deletion;
		}

		Data kth (int k) {
			Node *current = root;
			int i = current->right == nullptr ? 0 : current->right->weight;

			while (i != k) {
				if (i > k) {
					current = current->right;
					i -= 1 + (current->left == nullptr ? 0 : current->left->weight);
				}

				else {
					current = current->left;
					i += 1 + (current->right == nullptr ? 0 : current->right->weight);
				}
			}

			return current->key;
		}

		void print () const {
			queue<Node*> que;
			que.push(root);
		
			while (!que.empty()) {
				Node *current = que.front();
				que.pop();
		
				if (current == nullptr)
					continue;
		
				cout << current->key << " (" << current->weight << "): left – " << (current->left == nullptr ? 0 : current->left->key) << " right – " << (current->right == nullptr ? 0 : current->right->key) << endl;
				que.push(current->left);
				que.push(current->right);
			}
		}
};

ostream& operator << (ostream& os, const Treap &treap) {
	queue<Node*> que;
	que.push(treap.getRoot());

	while (!que.empty()) {
		Node *current = que.front();
		que.pop();

		if (current == nullptr)
			continue;

		os << current->key << " (" << current->weight << "): left – " << (current->left == nullptr ? 0 : current->left->key) << " right – " << (current->right == nullptr ? 0 : current->right->key) << endl;
		que.push(current->left);
		que.push(current->right);
	}

	return os; 
}


int main () {
	Treap treap;

	/*Node *root = new Node(100, 9);
	Node *node11 = new Node(80, 7);
	Node *node12 = new Node(110, 8);
	root->left = node11;
	root->right = node12;
	Node *node21 = new Node(70, 3);
	Node *node22 = new Node(90, 4);
	node11->left = node21;
	node11->right = node22;
	Node *node23 = new Node(105, 5);
	Node *node24 = new Node(120, 6);
	node12->left = node23;
	node12->right = node24;
	Node *node31 = new Node(60, 0);
	Node *node32 = new Node(75, 2);
	node21->left = node31;
	node21->right = node32;
	Node *node33 = new Node(95, -1);
	node22->right = node33;
	Node *node34 = new Node(115, 1);
	node24->left = node34;*/

	int n;
	cin >> n;

	for (int i = 0; i < n; i ++) {
		int type;
		cin >> type;

		if (type == 1) {
			Data key;
			cin >> key;
			int priority = rand();
			treap.insert(key, priority);
		}

		else if (type == -1) {
			Data key;
			cin >> key;
			treap.remove(key);
		}

		else if (type == 0) {
			int k;
			cin >> k;
			cout << treap.kth(k - 1) << endl;
		}
	}

	return 0;
}

