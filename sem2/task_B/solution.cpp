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

			int currentIndex = (current->left != nullptr ? current->left->weight : 0) + 1;

			if (currentIndex < key) {
				split(current->right, key - currentIndex, current->right, right);
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
		Treap (Node *root) {
			this->root = root;
		}

		Treap (Data key, int priority) {
			root = new Node(key, priority);
		}

		Treap () {
			root = nullptr;
		}

		Node *getRoot () const {
			return root;
		}

		void insert (int pos, Data key, int priority = rand()) {
			Node *left, *right;
			split(root, pos + 1, left, right);
			Node *insertion = new Node(key, priority);
			left = merge(left, insertion);
			root = merge(left, right);
		}

		void remove (int pos) {
			Node *left, *right;
			split(root, pos + 1, left, right);
			Node *deletion;
			split(right, 2, deletion, right);
			root = merge(left, right);
			delete deletion;
		}

		void moveToFront (int l, int r) {
			Node *left, *middle, *right;
			split(root, l, left, right);
			split(right, r - l + 2, middle, right);
			left = merge(middle, left);
			root = merge(left, right);
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

		void getArray (vector<Data> &array, Node *node = nullptr) const {
			if (node == nullptr) {
				array.clear();
				node = root;
			}

			if (root == nullptr)
				return;


			if (node->left != nullptr)
				getArray(array, node->left);

			array.push_back(node->key);

			if (node->right != nullptr)
				getArray(array, node->right);
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

ostream& operator << (ostream& os, const vector<int> &vec) {
	for (int el: vec)
		cout << el << " ";

	return os; 
}


int main () {
	Treap treap;

	int n;
	cin >> n;

	for (int i = 0; i < n; i ++)
		treap.insert(i, i + 1);


	int m;
	cin >> m;

	for (int i = 0; i < m; i ++) {
		int l, r;
		cin >> l >> r;
		treap.moveToFront(l, r);
	}


	vector<Data> array;
	treap.getArray(array);
	cout << array << endl;

	return 0;
}
