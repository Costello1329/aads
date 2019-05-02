#include <iostream>
#include <algorithm>
#include <vector>


using namespace std;


template <typename Type>
ostream& operator << (ostream &os, const vector<Type> &vec) {
	for (Type el: vec)
		os << el << " ";

	return os;
}


struct Edge {
	int from;
	int to;
	unsigned id;

	Edge (const unsigned &from = 0, const unsigned &to = 0) {
		this->from = from;
		this->to = to;
	}

	void scan (istream &is = cin) {
		is >> from >> to;
	}

	void print (ostream &os = cout) {
		os << from << " -> " << to;
	}
};

template <class Edge>
class Graph {
	private:
	public:
		vector<vector<Edge> > edges;

		Graph (const unsigned &verticesCount) {
			edges.resize(verticesCount);
		}

		void addEdge (const Edge &edge) {
			edges[edge.from].push_back(edge);
		}

		vector<Edge> const & getOuterEdges (const unsigned &vertex) const {
			return this->edges[vertex];
		}

		unsigned verticesCount () const {
			return edges.size();
		}

		void print (ostream &os = cout) const {
			for (unsigned vertex = 0; vertex < edges.size(); vertex ++) {
				os << vertex << ": ";
		
				for (Edge edge: edges[vertex]) {
					edge.print(os);
					os << " ";
				}
		
				os << endl;
			}
		}
};


/*template <class Edge>
bool dfs (const Graph<Edge> &graph, const vector<unsigned> &leftWeights, const vector<unsigned> &rightWeights, const unsigned &vertex, vector<bool> &used, vector<Edge> &matching) {
	if (used[vertex])
		return false;

	used[vertex] = true;

	for (const Edge &edge: graph.getOuterEdges(vertex)) {
		if (matching[edge.to].from == -1 || dfs(graph, leftWeights, rightWeights, matching[edge.to].from, used, matching)) {
			matching[edge.to] = edge;
			return true;
		}
	}

	return false;
}*/



int dfs (const Graph<Edge> &graph, const vector<unsigned> &leftWeights, const vector<unsigned> &rightWeights, const unsigned &vertex, vector<bool> &used, vector<Edge> &matching, vector<Edge> &parents) {
	if (used[vertex])
		return -1;

	used[vertex] = true;

	int maxRightWeight = -1;
	Edge bestEdge(-1, -1);
	
	for (const Edge &edge: graph.getOuterEdges(vertex)) {
		if (matching[edge.to].from == -1) {
			if (maxRightWeight < (int)rightWeights[edge.to - leftWeights.size()]) {
				maxRightWeight = rightWeights[edge.to - leftWeights.size()];
				bestEdge = edge;
			}
		}

		else {
			int newMaxRightWeight = dfs(graph, leftWeights, rightWeights, matching[edge.to].from, used, matching, parents);

			if (maxRightWeight < (int)newMaxRightWeight) {
				maxRightWeight = newMaxRightWeight;
				bestEdge = edge;
			}
		}
	}

	if (maxRightWeight != -1)
		parents[vertex] = bestEdge;

	return maxRightWeight;
}



inline vector<unsigned> getNewLeftOrder (const vector<unsigned> &leftWeights) {
	vector<pair<unsigned, unsigned> > pairs;

	for (unsigned i = 0; i < leftWeights.size(); i ++)
		pairs.push_back(make_pair(leftWeights[i], i));

	sort(pairs.begin(), pairs.end(), greater<pair<unsigned, unsigned> >());

	vector<unsigned> newLeftOrder;

	for (const pair<unsigned, unsigned> el: pairs)
		newLeftOrder.push_back(el.second);

	return newLeftOrder;
}


template <class Edge>
void findMaxWeightMatchingInBipartiteGraph (const Graph<Edge> &graph, const vector<unsigned> &leftWeights, const vector<unsigned> &rightWeights, vector<Edge> &edges) {
	vector<Edge> matching(graph.verticesCount(), Edge(-1, -1));
	vector<Edge> parents(graph.verticesCount(), Edge(-1, -1));


	vector<unsigned> newLeftOrder = getNewLeftOrder(leftWeights);


	for (const unsigned &vertex: newLeftOrder) {
		vector<bool> used(graph.verticesCount(), false);
		//cout << "NEW COOL DFS FROM: " << vertex << endl;
		dfs(graph, leftWeights, rightWeights, vertex, used, matching, parents);
		
		int current = vertex;

		while (current != -1 && parents[current].to != -1) {
			unsigned carry = matching[parents[current].to].from;
			matching[parents[current].to] = parents[current];
			current = carry;
		}
	}

	for (const Edge edge: matching)
		if (edge.from != -1)
			edges.push_back(edge);
}



int main () {
	unsigned leftCount, rightCount, edgesCount;
	cin >> leftCount >> rightCount >> edgesCount;
	Graph<Edge> graph(leftCount + rightCount);
	vector<unsigned> leftWeights(leftCount);
	vector<unsigned> rightWeights(rightCount);

	for (unsigned &leftWeight: leftWeights)
		cin >> leftWeight;

	for (unsigned &rightWeight: rightWeights)
		cin >> rightWeight;

	unsigned maxId = 0;

	for (unsigned i = 0; i < edgesCount; i ++) {
		Edge edge;
		cin >> edge.from >> edge.to;
		edge.from -= 1;
		edge.to += leftCount - 1;
		edge.id = maxId ++;
		graph.addEdge(edge);
	}


	// -------------------------------------------


	vector<Edge> edges;
	findMaxWeightMatchingInBipartiteGraph(graph, leftWeights, rightWeights, edges);


	// -------------------------------------------


	vector<bool> leftUsed(leftCount, false);
	vector<bool> rightUsed(rightCount, false);
	vector<bool> usedEdges(edgesCount, false);
	unsigned weight = 0;
	unsigned matchingEdgesCount = 0;

	for (Edge edge: edges) {
		if (!leftUsed[edge.from]) {
			leftUsed[edge.from] = true;
			weight += leftWeights[edge.from];
		}

		if (!rightUsed[edge.to - leftCount]) {
			rightUsed[edge.to - leftCount] = true;
			weight += rightWeights[edge.to - leftCount];
		}

		if (!usedEdges[edge.id]) {
			usedEdges[edge.id] = true;
			matchingEdgesCount ++;
		}
	}


	cout << weight << endl << matchingEdgesCount << endl;

	for (unsigned i = 0; i < edgesCount; i ++)
		if (usedEdges[i])
			cout << i + 1 << " ";

	cout << endl;

	return 0;
}
