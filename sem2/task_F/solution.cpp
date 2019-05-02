#include <algorithm>
#include <iostream>
#include <climits>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <queue>

typedef long long Data;

using std::vector;
using std::pair;
using std::ostream;
using std::istream;
using std::cin;
using std::cout;
using std::swap;
using std::queue;
using std::endl;


/* Utility: */

void printVec (ostream &os, vector<Data> &vec, char sep = ' ', char end = '\n') {
	for (Data el: vec)
		os << el << sep;
	
	if (end != '\0')
		os << end;
}

long long getRand () {
	static unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count ();
	static std::mt19937_64 generator (seed);
	return generator ();
}

/* Treap and Node classes: */

class Treap {
	private:
		struct Node {
			Data data;
			int priority;
		
			size_t weight;
			Data sum;
			Data addition;
			bool isAssigned;
			Data assignment;
			bool reversed;
			size_t increasingPrefixSize;
			size_t decreasingPrefixSize;
			size_t increasingSuffixSize;
			size_t decreasingSuffixSize;
			Data leftElement;
			Data rightElement;
		
			Node *left;
			Node *right;

			explicit Node (Data data) {
				this->data = data;
				this->priority = getRand();
				this->weight = 1;
				this->sum = data;
				this->addition = 0;
				this->isAssigned = false;
				this->assignment = 0;
				this->reversed = false;
				this->increasingPrefixSize = 1;
				this->decreasingPrefixSize = 1;
				this->increasingSuffixSize = 1;
				this->decreasingSuffixSize = 1;
				this->leftElement = data;
				this->rightElement = data;
				left = nullptr;
				right = nullptr;
			}
		} *root;

		static size_t getNodeWeight (Node *node) {
			return (node != nullptr ? node->weight : 0);
		}

		Node *getRight (Node *node) {
			return (node->right != nullptr ? node->right : node);
		}

		Node *getLeft (Node *node) {
			return (node->left != nullptr ? node->left : node);
		}
		
		// Lazy calculations:

		// Push:
		void pushAdditionToTheChildWhenAssigned (Node *parent, Node *child) {
			if (child == nullptr || parent == nullptr)
				return;

			child->isAssigned = true;
			child->assignment = parent->assignment + parent->addition;
			child->data = parent->assignment + parent->addition;
			child->rightElement = parent->assignment + parent->addition;
			child->leftElement = parent->assignment + parent->addition;
			child->sum = (parent->assignment + parent->addition) * getNodeWeight(child);
			child->increasingSuffixSize = getNodeWeight(child);
			child->increasingPrefixSize = getNodeWeight(child);
			child->decreasingSuffixSize = getNodeWeight(child);
			child->decreasingPrefixSize = getNodeWeight(child);
			child->addition = 0;
		}

		void pushAdditionWhenAssigned (Node *node) {
			node->isAssigned = false;
			node->data += node->addition;
			node->leftElement += node->addition;
			node->rightElement += node->addition;
			node->sum += node->weight * node->addition;
			node->addition = 0;
			node->increasingSuffixSize = getNodeWeight(node);
			node->decreasingSuffixSize = getNodeWeight(node);
			node->increasingPrefixSize = getNodeWeight(node);
			node->decreasingPrefixSize = getNodeWeight(node);
		}

		void pushAdditionWhenNotAssigned (Node *node) {
			node->sum = node->sum + node->addition * node->weight;
			node->data += node->addition;
			node->leftElement += node->addition;
			node->rightElement += node->addition;
			
			if(node->left != nullptr)
				node->left->addition += node->addition;
			
			if(node->right != nullptr)
				node->right->addition += node->addition;
			
			node->addition = 0;
		}

		void pushReversed (Node *node) {
			if (node == nullptr || !node->reversed)
				return;

			if (node->left != nullptr)
				node->left->reversed ^= true;
			
			if (node->right != nullptr)
				node->right->reversed ^= true;
			
			node->reversed = false;
			swap(node->left, node->right);
			swap(node->increasingSuffixSize, node->increasingPrefixSize);
			swap(node->decreasingSuffixSize, node->decreasingPrefixSize);
			swap(node->rightElement, node->leftElement);
		}

		void push (Node *node) {
			if (node == nullptr)
				return;
			
			if (node->isAssigned) {
				pushAdditionToTheChildWhenAssigned(node, node->left);
				pushAdditionToTheChildWhenAssigned(node, node->right);
				pushAdditionWhenAssigned(node);
			}
			
			else
				pushAdditionWhenNotAssigned(node);
			
			pushReversed(node);
		}

		// Update:
		void updateSumWeightEdges (Node *node) {
			node->weight = getNodeWeight(node->left) + getNodeWeight(node->right) + 1;
			node->sum = node->data;
			node->leftElement = node->data;
			node->rightElement = node->data;

			if (node->left != nullptr) {
				node->sum += node->left->sum + node->left->addition * getNodeWeight(node->left);
				node->leftElement = node->left->leftElement;
			}

			if (node->right != nullptr) {
				node->sum += node->right->sum + node->right->addition * getNodeWeight(node->right);
				node->rightElement = node->right->rightElement;
			}
		}

		void preUpdateFixes (Node *node) {
			if (node == nullptr)
				return;

			if (node->right) {
				node->increasingSuffixSize = node->right->increasingSuffixSize;
				node->decreasingSuffixSize = node->right->decreasingSuffixSize;
			}

			else {
				node->increasingSuffixSize = 0;
				node->decreasingSuffixSize = 0;
			}

			if (node->left) {
				node->increasingPrefixSize = node->left->increasingPrefixSize;
				node->decreasingPrefixSize = node->left->decreasingPrefixSize;
			}

			else {
				node->increasingPrefixSize = 0;
				node->decreasingPrefixSize = 0;
			}
		}

		template <class Comparator>
		void updateFixes (size_t &nodeFix, Node *&child, Node *&otherChild, Comparator compare, Data nodeData, Data element, Data otherElement, Data addition) {
			if (nodeFix != getNodeWeight(child))
				return;

			if (child == nullptr || compare(nodeData, element)) {
				nodeFix ++;
				
				if (otherChild != nullptr && compare(otherElement, nodeData))
					nodeFix += addition;
			}
		}

		void update (Node *node) {
				if (node == nullptr)
					return;

				push(node->left);
				push(node->right);
			
				updateSumWeightEdges(node);
				preUpdateFixes(node);

				updateFixes(node->increasingSuffixSize, node->right, node->left, std::greater_equal<Data>(), node->data, (node->right ? node->right->leftElement : 0), (node->left ? node->left->rightElement : 0), (node->left ? static_cast<size_t>(node->left->increasingSuffixSize) : 0));
				updateFixes(node->decreasingSuffixSize, node->right, node->left, std::less_equal<Data>(), node->data, (node->right ? node->right->leftElement : 0), (node->left ? node->left->rightElement : 0), (node->left ? static_cast<size_t>(node->left->decreasingSuffixSize) : 0));
				updateFixes(node->increasingPrefixSize, node->left, node->right, std::greater_equal<Data>(), node->data, (node->left ? node->left->rightElement : 0), (node->right ? node->right->leftElement : 0), (node->right ? static_cast<size_t>(node->right->increasingPrefixSize) : 0));
				updateFixes(node->decreasingPrefixSize, node->left, node->right, std::less_equal<Data>(), node->data, (node->left ? node->left->rightElement : 0), (node->right ? node->right->leftElement : 0), (node->right ? static_cast<size_t>(node->right->decreasingPrefixSize) : 0));
		}

		// Main non-client treap operations:
		pair<Node*, Node*> split (Node *node, size_t implicitKey) {			
			if (node == nullptr)
				return {nullptr, nullptr};

			push(node);

			if (getNodeWeight(node->left) >= implicitKey) {
				pair<Node*, Node*> treap = split(node->left, implicitKey);
				node->left = treap.second;
				update(node);
				return {treap.first, node};
			}

			else {
				pair<Node*, Node*> treap = split(node->right, implicitKey - getNodeWeight(node->left) - 1);
				node->right = treap.first;
				update(node);
				return {node, treap.second};
			}
		}

		template<typename Comparator = std::less_equal<Data> >
		pair<Node*, Node*> splitByData (Node *node, Data data, Comparator compare = std::less_equal<Data>()) {
			if (node == nullptr)
				return {nullptr, nullptr};

			push(node);

			if (compare(node->data, data)) {
				pair<Node*, Node*> treap = splitByData(node->right, data, compare);
				node->right = treap.first;
				update(node);
				return {node, treap.second};
			}

			else {
				pair<Node*, Node*> treap = splitByData(node->left, data, compare);
				node->left = treap.second;
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
		
			if (left->priority >= right->priority) {
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

		void reverseOnSubsegment (size_t l, size_t r, Node *node) {
			if (node == nullptr)
				return;

			pair<Node*, Node*> treap1 = split(node, l - 1);
			pair<Node*, Node*> treap2 = split(treap1.second, r - l + 1);
			treap2.first->reversed ^= true;
			treap1.second = merge(treap2.first, treap2.second);
			node = merge(treap1.first, treap1.second);
		}

		template<typename Method>
		void operation(size_t l, size_t r, Method method) {
			if (root == nullptr)
				return;

			pair<Node*, Node*> treap1 = split(root, l - 1);
			pair<Node*, Node*> treap2 = split(treap1.second, r - l + 1);
			method(treap2.first);
			root = merge(treap1.first, treap2.first);
			root = merge(root, treap2.second);
		}

		template <class Comparator = std::less_equal<Data> >
		void permutationOnSubsegment (size_t l, size_t r, bool flag, Comparator compare = std::less_equal<Data>()) {
			operation(l, r, [this, flag, compare](Node *innerNode) {
				push(innerNode);
	
				size_t fixSize = flag ? innerNode->increasingSuffixSize : innerNode->decreasingSuffixSize;
	
				if (fixSize == getNodeWeight(innerNode)) {
					reverseOnSubsegment(1, getNodeWeight(innerNode), innerNode);
					return;
				}
			
				pair<Node*, Node*> t1 = split(innerNode, getNodeWeight(innerNode) - fixSize - 1);
				pair<Node*, Node*> t2 = split(t1.second, 1);
				reverseOnSubsegment(1, getNodeWeight(t2.second), t2.second);
				pair<Node*, Node*> t3 = splitByData(t2.second, t2.first->data, compare);
				pair<Node*, Node*> t4 = split(t3.second, 1);
				t1.first = merge(t1.first, t4.first);
				t1.first = merge(t1.first, t3.first);
				t1.first = merge(t1.first, t2.first);
				t1.first = merge(t1.first, t4.second);
				innerNode = t1.first;
			});
		}

	public:
		// Constructors and Destructors:

		Treap (Node *root = nullptr) {
			this->root = root;
		}

		Treap (Data data) {
			root = new Node(data);
		}

		~Treap () {
			clear();
		}

		void clear () {
			queue<Node*> que;
			que.push(root);
		
			while (!que.empty()) {
				Node *current = que.front();
				que.pop();
		
				if (current == nullptr)
					continue;

				que.push(current->left);
				que.push(current->right);

				delete current;
			}
		}

		// Getters:

		Data getAt (size_t index) {
			Data data;

			operation(index, index, [&data](Node *&innerNode) {
				data = innerNode->data;
			});

			return data;
		}

		void getArray (vector<Data> &array, Node *node = nullptr) {
			update(root);

			for (int i = 1; i <= getNodeWeight(root); i ++)
				array.push_back(getAt(i));
		}

		ostream &print (ostream &os) const {
			queue<Node*> que;
			que.push(root);
		
			while (!que.empty()) {
				Node *current = que.front();
				que.pop();
		
				if (current == nullptr)
					continue;
		
				os << current->data << " (sum: " << current->sum << ", addition: " << current->addition << "): left – ";
				
				if (current->left == nullptr)
					cout << "x";

				else
					cout << current->left->data;

				cout << " right – ";

				if (current->right == nullptr)
					cout << "x" << endl;

				else
					cout << current->right->data << endl;

				que.push(current->left);
				que.push(current->right);
			}

			return os;
		}

		// Main methods:

		void insert (Data data, size_t pos = INT_MAX - 1) {
			Node *insertion = new Node(data);
			Node *left, *right;
			pair<Node*, Node*> treap = split(root, pos - 1);
			Node *newLeft = merge(treap.first, insertion);
			root = merge(newLeft, treap.second);
		}

		void remove (size_t pos) {
			operation(pos, pos, [](Node *&innerNode) {
				delete innerNode;
				innerNode = nullptr;
			});
		}

		// Subsegments' methods:

		Data sumOnSubsegment (size_t l, size_t r) {
			Data sum;

			operation(l, r, [&sum](Node *innerNode) {
				sum = innerNode->sum;
			});

			return sum;
		}

		void addOnSubsegment (size_t l, size_t r, Data addition) {
			operation(l, r, [addition](Node *innerNode) {
				innerNode->addition += addition;
			});
		}

		void attachOnSubsegment (size_t l, size_t r, Data assignment) {
			operation(l, r, [l, r, assignment](Node *innerNode) {
				innerNode->leftElement = assignment;
				innerNode->rightElement = assignment;
				innerNode->data = assignment;
				innerNode->sum = (r - l + 1) * assignment;
				innerNode->isAssigned = true;
				innerNode->assignment = assignment;
				innerNode->increasingPrefixSize = innerNode->weight;
				innerNode->decreasingPrefixSize = innerNode->weight;
				innerNode->increasingSuffixSize = innerNode->weight;
				innerNode->decreasingSuffixSize = innerNode->weight;
				innerNode->addition = 0;
			});
		}

		void nextPermutationOnSubsegment(size_t l, size_t r) {
			permutationOnSubsegment(l, r, true, std::less_equal<Data>());
		}

		void prevPermutationOnSubsegment(size_t l, size_t r) {
			permutationOnSubsegment(l, r, false, std::greater_equal<Data>());
		}
};


/* Querry class, and it's children: */

enum {
	sumOnSubsegment = 1,
	insertElement = 2,
	removeElement = 3,
	attachOnSubsegment = 4,
	addOnSubsegment = 5,
	nextPermutationOnSubsegment = 6,
	prevPermutationOnSubsegment = 7
};

class Query {
	public:
		virtual void makeQuery (Treap&) = 0;
};

class SumOnSubsegment: public Query {
	private:
		size_t l, r;
		Data out;

	public:
		// Constructor:
		SumOnSubsegment(size_t l, size_t r) : l(l), r(r), out(0) {};

		// Getter:
		Data get () const {
			return out;
		}

		void makeQuery (Treap &treap) override {
			out = treap.sumOnSubsegment(l, r);
		}
};

class Insert: public Query {
	private:
		Data insertion;
		size_t pos;

	public:
		// Constructor:
		Insert(Data insertion, size_t pos) : insertion(insertion), pos(pos) {};

		void makeQuery (Treap &treap) override {
			treap.insert(insertion, pos);
		}
};

class Remove: public Query {
	private:
		size_t pos;

	public:
		// Constructor:
		Remove(size_t pos) : pos(pos) {};

		void makeQuery (Treap &treap) override {
			treap.remove(pos);
		}
};

class AttachOnSubsegment: public Query {
	private:
		size_t l, r;
		Data attachment;

	public:
		// Constructor:
		AttachOnSubsegment(size_t l, size_t r, Data attachment) : l(l), r(r), attachment(attachment) {};

		void makeQuery (Treap &treap) override {
			treap.attachOnSubsegment(l, r, attachment);
		}
};

class AddOnSubsegment: public Query {
	private:
		size_t l, r;
		Data addition;

	public:
		// Constructor:
		AddOnSubsegment(size_t l, size_t r, Data addition) : l(l), r(r), addition(addition) {};

		void makeQuery (Treap &treap) override {
			treap.addOnSubsegment(l, r, addition);
		}
};

class NextPermutationOnSubsegment: public Query {
	private:
		size_t l, r;

	public:
		// Constructor:
		NextPermutationOnSubsegment(size_t l, size_t r) : l(l), r(r) {};

		void makeQuery (Treap &treap) override {
			treap.nextPermutationOnSubsegment(l, r);
		}
};

class PrevPermutationOnSubsegment: public Query {
	private:
		size_t l, r;

	public:
		// Constructor:
		PrevPermutationOnSubsegment(size_t l, size_t r) : l(l), r(r) {};

		void makeQuery (Treap &treap) override {
			treap.prevPermutationOnSubsegment(l, r);
		}
};

Query *ConstructQuery (istream &is, unsigned short commandType) {
	switch (commandType) {
		case sumOnSubsegment: {
			size_t l, r;
			is >> l >> r;
			l ++;
			r ++;
			SumOnSubsegment *query = new SumOnSubsegment(l, r);
			return query;
			break;
		}

		case insertElement: {
			Data el;
			int pos;
			is >> el >> pos;
			pos ++;
			Insert *query = new Insert(el, pos);
			return query;
			break;
		}

		case removeElement: {
			size_t pos;
			is >> pos;
			pos ++;
			Remove *query = new Remove(pos);
			return query;
			break;
		}

		case attachOnSubsegment: {
			size_t l, r;
			Data attachment;
			is >> attachment >> l >> r;
			l ++;
			r ++;
			AttachOnSubsegment *query = new AttachOnSubsegment(l, r, attachment);
			return query;
			break;
		}

		case addOnSubsegment: {
			size_t l, r;
			Data addition;
			cin >> addition >> l >> r;
			l ++;
			r ++;
			AddOnSubsegment *query = new AddOnSubsegment(l, r, addition);
			return query;
			break;
		}

		case nextPermutationOnSubsegment: {
			size_t l, r;
			cin >> l >> r;
			l ++;
			r ++;
			NextPermutationOnSubsegment *query = new NextPermutationOnSubsegment(l, r);
			return query;
			break;
		}

		case prevPermutationOnSubsegment: {
			size_t l, r;
			cin >> l >> r;
			l ++;
			r ++;
			PrevPermutationOnSubsegment *query = new PrevPermutationOnSubsegment(l, r);
			return query;
			break;
		}

		default: {
			Data el;
			int pos;
			is >> el >> pos;
			pos ++;
			Insert *query = new Insert(el, pos);
			return query;
			break;
		}
	}
}

class QueryStack {
	private:
		vector<Query*> queries;

	public:
		//Constructors:
		QueryStack (const QueryStack &other) {
			vector<Query*> newQueries;

			for (int i = 0; i < other.size(); i ++) {
				Query *newQueryCasted = dynamic_cast<SumOnSubsegment*>(other.get(i));

				if (newQueryCasted == nullptr)
					newQueryCasted = dynamic_cast<Insert*>(other.get(i));

				if (newQueryCasted == nullptr)
					newQueryCasted = dynamic_cast<Remove*>(other.get(i));

				if (newQueryCasted == nullptr)
					newQueryCasted = dynamic_cast<AddOnSubsegment*>(other.get(i));

				if (newQueryCasted == nullptr)
					newQueryCasted = dynamic_cast<AttachOnSubsegment*>(other.get(i));

				if (newQueryCasted == nullptr)
					newQueryCasted = dynamic_cast<NextPermutationOnSubsegment*>(other.get(i));

				if (newQueryCasted == nullptr)
					newQueryCasted = dynamic_cast<PrevPermutationOnSubsegment*>(other.get(i));

				newQueries.push_back(newQueryCasted);
			}

			queries = newQueries;
		}

		QueryStack () {};


		// Getters:
		Query *get (size_t i) const {
			return queries[i];
		}
	
		size_t size () const {
			return queries.size();
		}
	
		// Setter:
		void push (Query *query) {
			queries.push_back(query);
		}
};

QueryStack Input(istream& is) {
	QueryStack queryStack;
	size_t n;
	is >> n;

	for (size_t i = 1; i <= n; ++i) {
		Data data;
		is >> data;
		Query* query = static_cast<Query*>(new Insert(data, i));
		queryStack.push(query);
	}

	size_t m;
	is >> m;

	for (size_t i = 0; i < m; i ++) {
		int operation;
		is >> operation;
		Query *query = ConstructQuery(is, operation);
		queryStack.push(query);
	}

	return queryStack;
}

/* Querry manager class and it's methods: */

template <class Structure>
class QueryManager {
	Structure structure;
	QueryStack queryStack;
	vector<Data> sumOutput;
	vector<Data> dataOutput;

	public:
		QueryManager(const QueryStack &constQueryStack) {
			queryStack = constQueryStack;

			for (size_t i = 0; i < queryStack.size(); ++i) {

				queryStack.get(i)->makeQuery(structure);
				SumOnSubsegment *checker = dynamic_cast<SumOnSubsegment*>(queryStack.get(i));
				
				if (checker != nullptr) {
					sumOutput.push_back(checker->get());
				}
			}

			structure.getArray(dataOutput);
		}
	
		const vector<Data> &getSumOutput () const {
			return sumOutput;
		}
	
		const vector<Data> &getValueOutput () const {
			return dataOutput;
		}
};

template <class Structure>
void Output(const QueryManager<Structure>& manager, std::ostream& os) {
	vector<Data> outputSums = manager.getSumOutput();
	vector<Data> outputValues = manager.getValueOutput();

	printVec(cout, outputSums, '\n', '\0');
	printVec(cout, outputValues);
}

/* Problem solving: */

void solveProblem() {
	QueryManager<Treap> manager(Input(cin));
	Output(manager, cout);
}

int main () {
	solveProblem();
	return 0;
}
