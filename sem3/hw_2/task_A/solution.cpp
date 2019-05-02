#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <vector>
#include <queue>
#include <list>

using namespace std;



// Graph class: --------------------------------------------------



template <class Edge>
class Graph {
	private:
		vector<vector<Edge> > edges;

	public:
		Graph () {}

		Graph (const typename Edge::IndexType &verticesCount) {
			edges.clear();
			edges.resize(verticesCount);
		}

		void addEdge (const Edge &edge) {
			edges[edge.from].push_back(edge);
		}

		const vector<Edge>& getConstOuterEdges (const typename Edge::IndexType &vertex) const {
			return edges[vertex];
		}

		typename Edge::IndexType getVerticesCount () const {
			return edges.size();
		}
};



// Net class: --------------------------------------------------



template <class Edge>
class Net {
	private:
		typedef Edge* EdgePointer;
		typedef pair<EdgePointer, EdgePointer> Arc;
		typename Edge::IndexType source;
		typename Edge::IndexType sink;
		vector<Arc>* edges;
		vector<vector<Arc>*> innerEdges;
		vector<vector<Arc>*> outerEdges;

		class UniversalEdgeIterator {
			friend class Net;

			private:
				vector<Arc>* edgesList;
				typename vector<Arc>::iterator edgeIterator;

				bool validated;
				enum UniversalIteratorType {inner, outer, all};
				UniversalIteratorType iteratorType;
	
				explicit UniversalEdgeIterator (vector<Arc> *edgesList, const UniversalIteratorType &iteratorType) : edgesList(edgesList), iteratorType(iteratorType), validated(false)  {};

				void validate () {
					if (!validated)
						begin();

					validated = true;
				}

			public:
				typedef UniversalIteratorType IteratorType;

				explicit UniversalEdgeIterator () : edgesList(nullptr), validated(false) {}

				// Iterator methods:
				void begin () {
					edgeIterator = edgesList->begin();
				}

				void move () {
					edgeIterator = next(edgeIterator);
				}
	
				bool end () const {
					return edgesList->size() == 0 || edgeIterator == edgesList->end();
				}

				// Getters:
				Edge getEdge () const {
					return *(edgeIterator->first);
				}

				Edge getReversedEdge () const {
					return *(edgeIterator->second);
				}

				// Setter:
				void increaseFlow (const typename Edge::FlowType &delta) const {
					edgeIterator->first->flow += delta;
					edgeIterator->second->flow -= delta;
				}

				// Other:
				IteratorType getIteratorType () const {
					return iteratorType;
				}
		};

		UniversalEdgeIterator* allEdgesIterator;
		vector<UniversalEdgeIterator*> innerEdgesIterators;
		vector<UniversalEdgeIterator*> outerEdgesIterators;

		void expandIndexation (const typename Edge::IndexType &newVerticesCount) {
			const typename Edge::IndexType &oldVerticesCount = getVerticesCount();

			if (oldVerticesCount >= newVerticesCount)
				return;

			innerEdges.resize(newVerticesCount);
			outerEdges.resize(newVerticesCount);
			innerEdgesIterators.resize(newVerticesCount);
			outerEdgesIterators.resize(newVerticesCount);

			for (typename Edge::IndexType i = oldVerticesCount; i < newVerticesCount; i ++) {
				innerEdges[i] = new vector<Arc>();
				outerEdges[i] = new vector<Arc>();
				innerEdgesIterators[i] = new EdgeIterator(&(*innerEdges[i]), EdgeIterator::IteratorType::inner);
				outerEdgesIterators[i] = new EdgeIterator(&(*outerEdges[i]), EdgeIterator::IteratorType::outer);
			}
		}

	public:
		typedef UniversalEdgeIterator EdgeIterator;
		typedef typename Edge::IndexType IndexType;

		// Constructors, destructors:
		explicit Net (const IndexType &verticesCount = 0) {
			edges = new vector<Arc>();
			allEdgesIterator = new EdgeIterator(&(*edges), EdgeIterator::IteratorType::all);
		}

		~Net () {
			for (const Arc &arc: *edges)
				delete arc.first;

			delete edges;
			delete allEdgesIterator;

			for (typename Edge::IndexType vertex = 0; vertex < getVerticesCount(); vertex ++) {
				delete innerEdges[vertex];
				delete outerEdges[vertex];
				delete innerEdgesIterators[vertex];
				delete outerEdgesIterators[vertex];
			}
		}

		// Setters:
		void setSource (const typename Edge::IndexType &source) {
			expandIndexation(source + 1);
			this->source = source;
		}

		void setSink (const typename Edge::IndexType &sink) {
			expandIndexation(sink + 1);
			this->sink = sink;
		}

		void addEdge (const Edge &edge) {
			expandIndexation(max<typename Edge::IndexType>(edge.from, edge.to) + 1);

			EdgePointer edgePointer = new Edge(edge);
			EdgePointer reversedEdgePointer = new Edge(edge.reversed());

			edges->push_back(make_pair(edgePointer, reversedEdgePointer));
			outerEdges[edgePointer->from]->push_back(make_pair(edgePointer, reversedEdgePointer));
			innerEdges[edgePointer->to]->push_back(make_pair(edgePointer, reversedEdgePointer));

			edges->push_back(make_pair(reversedEdgePointer, edgePointer));
			outerEdges[reversedEdgePointer->from]->push_back(make_pair(reversedEdgePointer, edgePointer));
			innerEdges[reversedEdgePointer->to]->push_back(make_pair(reversedEdgePointer, edgePointer));

			allEdgesIterator->validate();
			outerEdgesIterators[edgePointer->from]->validate();
			outerEdgesIterators[edgePointer->to]->validate();
			innerEdgesIterators[edgePointer->from]->validate();
			innerEdgesIterators[edgePointer->to]->validate();
		}

		// Getters:
		const typename Edge::IndexType &getSource () const {
			return source;
		}

		const typename Edge::IndexType &getSink () const {
			return sink;
		}

		EdgeIterator& getAllEdgesIterator () {
			return *allEdgesIterator;
		}

		EdgeIterator getConstAllEdgesIterator () const {
			return *allEdgesIterator;
		}

		EdgeIterator& getInnerEdgeIterator (const IndexType &vertex) {
			return *innerEdgesIterators[vertex];
		}

		EdgeIterator getConstInnerEdgeIterator (const IndexType &vertex) const {
			return *innerEdgesIterators[vertex];
		}

		EdgeIterator& getOuterEdgeIterator (const IndexType &vertex) {
			return *outerEdgesIterators[vertex];
		}

		EdgeIterator getConstOuterEdgeIterator (const IndexType &vertex) const {
			return *outerEdgesIterators[vertex];
		}

		// Info:
		IndexType getVerticesCount () const {
			return innerEdges.size();
		}

		IndexType getEdgesCount () const {
			return edges->size();
		}
};



// Flow addons: --------------------------------------------------



template <class Edge>
const typename Edge::FlowType getFlowFromOrToVertex (typename Net<Edge>::EdgeIterator iter) {
	typename Edge::FlowType flow = 0;

	for (iter.begin(); !iter.end(); iter.move())
		flow += iter.getEdge().flow;

	return flow;
}

template <class Edge>
bool isFlowValid (const Net<Edge> &net) {
	typename Net<Edge>::EdgeIterator iter = net.getConstAllEdgesIterator();

	for (iter.begin(); !iter.end(); iter.move())
		if (iter.getEdge().flow > iter.getEdge().capacity || iter.getEdge().flow != -iter.getReversedEdge().flow)
			return false;

	for (typename Edge::IndexType vertex = 0; vertex < net.getVerticesCount(); vertex ++) {
		if (vertex == net.getSource() || vertex == net.getSink())
			break;

		typename Edge::FlowType innerFlow = getFlowFromOrToVertex<Edge>(net.getConstInnerEdgeIterator(vertex));
		typename Edge::FlowType outerFlow = getFlowFromOrToVertex<Edge>(net.getConstOuterEdgeIterator(vertex));

		if (innerFlow != outerFlow)
			return false;
	}

	return true;
}

template <class Edge>
typename Edge::FlowType getFlowWithoutCheck (const Net<Edge> &net) {
	return getFlowFromOrToVertex<Edge>(net.getConstOuterEdgeIterator(net.getSource()));
}

template <class Edge>
typename Edge::FlowType getFlowWithCheck (const Net<Edge> &net) {
	if (isFlowValid(net))
		return getFlowWithoutCheck(net);

	else
		throw ("Net exeption: invalid flow!");
}

template <class Edge>
void printEdge (const Edge &edge, ostream &os = cout) {
	os << edge.from << " -> " << edge.to << " (" << edge.flow << "/" << edge.capacity << ")";
}

template <class Edge>
void printNet (Net<Edge> &net, ostream &os = cout) {
	for (typename Edge::IndexType vertex = 0; vertex < net.getVerticesCount(); vertex ++) {
		typename Net<Edge>::EdgeIterator iter = net.getConstOuterEdgeIterator(vertex);

		for (iter.begin(); !iter.end(); iter.move()) {
			printEdge(iter.getEdge(), os);
			os << " ";
		}

		os << endl;
	}
}



// Min Cost Max Flow algorithm: --------------------------------------------------


template <class Edge>
vector<typename Edge::CostType> findMinCostPathsToAllVertices (const Graph<Edge> &graph, const typename Edge::IndexType &vertex) {
	vector<typename Edge::CostType> costs(graph.getVerticesCount(), numeric_limits<typename Edge::CostType>::max());
	costs[vertex] = 0;
	
	for (typename Edge::IndexType iteration = 0; iteration < graph.getVerticesCount() - 1; iteration ++)
		for (typename Edge::IndexType from = 0; from < graph.getVerticesCount(); from ++)
			for (const Edge &edge: graph.getConstOuterEdges(from))
				if (costs[edge.from] < numeric_limits<typename Edge::CostType>::max())
					costs[edge.to] = min<typename Edge::CostType>(costs[edge.to], costs[edge.from] + edge.cost);

	return costs;
}

template <class Edge>
bool buildMinCostPathDFS (const Net<Edge> &net, const vector<typename Edge::CostType> &costs, const typename Edge::IndexType &vertex, const typename Edge::IndexType &destination, vector<bool> &used, list<Edge> &path, typename Edge::CostType &currentPathCost) {
	if (used[vertex])
		return false;

	used[vertex] = true;

	if (vertex == destination)
		return true;

	typename Net<Edge>::EdgeIterator iter = net.getConstOuterEdgeIterator(vertex);
	for (iter.begin(); !iter.end(); iter.move()) {
		if (iter.getEdge().residualCapacity() <= 0)
			continue;

		if (currentPathCost + iter.getEdge().cost == costs[iter.getEdge().to]) {
			path.push_back(iter.getEdge());
			currentPathCost = costs[iter.getEdge().to];

			if (buildMinCostPathDFS(net, costs, iter.getEdge().to, destination, used, path, currentPathCost))
				return true;

			else {
				path.pop_back();
				currentPathCost = costs[iter.getEdge().from];
			}
		}
	}

	return false;
}

template <class Edge>
bool setFlowOnPathDFS (Net<Edge> &net, const typename Edge::IndexType &vertex, const typename Edge::FlowType &augmentingFlow, list<Edge> &path, vector<bool> &used) {
	if (used[vertex])
		return false;

	used[vertex] = true;

	if (path.empty())
		return true;

	typename Net<Edge>::EdgeIterator iter = net.getConstOuterEdgeIterator(vertex);
	for (iter.begin(); !iter.end(); iter.move()) {
		if (iter.getEdge().residualCapacity() <= 0)
			continue;

		if (iter.getEdge() == path.front()) {
			path.pop_front();
			iter.increaseFlow(augmentingFlow);

			if (setFlowOnPathDFS(net, iter.getEdge().to, augmentingFlow, path, used))
				return true;

			else {
				path.push_front(iter.getEdge());
				iter.increaseFlow(- augmentingFlow);
			}
		}
	}

	return false;
}

template <class Edge>
void setMaxFlowMinCost (Net<Edge> &net) {
	typename Net<Edge>::EdgeIterator iter = net.getConstAllEdgesIterator();
	for (iter.begin(); !iter.end(); iter.move())
		iter.increaseFlow(- iter.getEdge().flow);

	// O(|F|)
	while (true) {
		Graph<Edge> residualGraph(net.getVerticesCount());

		typename Net<Edge>::EdgeIterator iter = net.getConstAllEdgesIterator();
		for (iter.begin(); !iter.end(); iter.move())
			if (iter.getEdge().residualCapacity() > 0)
				residualGraph.addEdge(iter.getEdge());

		// O(|E|*|V|)
		vector<typename Edge::CostType> costs = findMinCostPathsToAllVertices(residualGraph, net.getSource());

		// O(E)
		vector<bool> used(net.getVerticesCount(), false);
		list<Edge> path;
		typename Edge::CostType currentPathCost = 0;
		
		if (!buildMinCostPathDFS(net, costs, net.getSource(), net.getSink(), used, path, currentPathCost))
			break;

		typename Edge::FlowType augmentingFlow = numeric_limits<typename Edge::FlowType>::max();
		
		for (const Edge &edge: path)
			augmentingFlow = min<typename Edge::FlowType>(augmentingFlow, edge.residualCapacity());

		used.assign(net.getVerticesCount(), false);
		setFlowOnPathDFS(net, net.getSource(), augmentingFlow, path, used);
	}
}




// My non-lib code: --------------------------------------------------




// My custom Edge:
template <typename FirstType = unsigned, typename SecondType = int, typename ThirdType = long long>
struct CostedFlowedEdge {
	typedef FirstType IndexType;
	typedef SecondType FlowType;
	typedef ThirdType CostType;
	IndexType from;
	IndexType to;
	FlowType flow;
	FlowType capacity;
	CostType cost;
	unsigned index;

	explicit CostedFlowedEdge (const IndexType &from, const IndexType &to, const FlowType &capacity, const CostType &cost, const unsigned index = 0) : from(from), to(to), flow(0), capacity(capacity), cost(cost), index(index) {}

	FlowType residualCapacity () const {
		return capacity - flow;
	}

	CostedFlowedEdge reversed () const {
		return CostedFlowedEdge(to, from, 0, - cost);
	}

	bool operator == (const CostedFlowedEdge &otherEdge) const {
		return from == otherEdge.from && to == otherEdge.to && flow == otherEdge.flow && capacity == otherEdge.capacity && cost == otherEdge.cost;
	}
};

// Kingdom:
template <typename Edge>
void inputKingdom (Net<Edge> &net, typename Edge::FlowType &neededFlow, istream &is) {
	typename Edge::IndexType verticesCount, edgesCount;
	is >> verticesCount >> edgesCount >> neededFlow;

	net.setSource(0);
	net.setSink(verticesCount);

	net.addEdge(Edge(0, 1, neededFlow, 0));

	for (unsigned edge = 0; edge < edgesCount; edge ++) {
		typename Edge::IndexType from, to;
		typename Edge::CostType cost;
		is >> from >> to >> cost;
		net.addEdge(Edge(from, to, 1, cost, edge + 1));
		net.addEdge(Edge(to, from, 1, cost, edge + 1));
	}
}

template <class Edge>
typename Edge::FlowType decreaseOnPath (Net<Edge> &net, const typename Edge::IndexType &vertex, const typename Edge::IndexType &destination, list<Edge> &path, vector<bool> &used, const typename Edge::FlowType &decreasing = numeric_limits<typename Edge::FlowType>::max()) {
	if (used[vertex])
		return -1;

	used[vertex] = true;

	if (vertex == destination)
		return decreasing;

	typename Net<Edge>::EdgeIterator iter = net.getConstOuterEdgeIterator(vertex);
	for (iter.begin(); !iter.end(); iter.move()) {
		if (iter.getEdge().flow > 0) {
			const typename Edge::FlowType newDecreasing = min<typename Edge::FlowType>(decreasing, iter.getEdge().flow);
			const typename Edge::FlowType finalDecreasing = decreaseOnPath(net, iter.getEdge().to, destination, path, used, newDecreasing);

			if (finalDecreasing > 0) {
				path.push_front(iter.getEdge());
				iter.increaseFlow(- finalDecreasing);
				return finalDecreasing;
			}
		}
	}

	return -1;
}

template <typename Edge>
typename Edge::CostType solveKingdom (Net<Edge> &net, const typename Edge::FlowType &neededFlow, vector<list<Edge> > &paths) {
	setMaxFlowMinCost(net);
	typename Edge::CostType minCost = 0;

	typename Net<Edge>::EdgeIterator iter = net.getConstAllEdgesIterator();
	for (iter.begin(); !iter.end(); iter.move())
		if (iter.getEdge().flow > 0)
			minCost += iter.getEdge().flow * iter.getEdge().cost;

	if (getFlowWithoutCheck(net) != neededFlow)
		return -1;

	else {
		while (true) {
			list<Edge> path;
			vector<bool> used(net.getVerticesCount(), false);
			
			if (decreaseOnPath(net, net.getSource(), net.getSink(), path, used) == -1)
				break;

			else
				paths.push_back(path);
		}
	}

	return minCost;
}

void outputKingdom (const long long &minCost, const int &neededFlow, const vector<list<CostedFlowedEdge<unsigned, int, long long> > > &paths, ostream &os) {
	if (minCost != -1) {
		os << fixed << setprecision(10) << (double)minCost/(double)neededFlow << endl;

		for (const list<CostedFlowedEdge<unsigned, int, long long> > &path: paths) {
			os << path.size() - 1 << " ";

			for (const CostedFlowedEdge<unsigned, int, long long> &edge: path)
				if (edge.index != 0)
					os << edge.index << " ";

			os << endl;
		}
	}

	else
		os << -1 << endl;
}


// MinCostMaxFlow:
template <typename Edge>
Net<Edge> inputMinCostMaxFlow (istream &is) {
	unsigned verticesCount, edgesCount;
	is >> verticesCount >> edgesCount;

	Net<Edge> net(verticesCount);
	net.setSource(0);
	net.setSink(verticesCount - 1);

	for (unsigned edge = 0; edge < edgesCount; edge ++) {
		typename Edge::IndexType from, to;
		typename Edge::FlowType capacity;
		typename Edge::CostType cost;
		is >> from >> to >> capacity >> cost;
		net.addEdge(Edge(from - 1, to - 1, capacity, cost));
	}

	return net;
}

template <typename Edge>
typename Edge::CostType solveMinCostMaxFlow (Net<Edge> &net) {
	setMaxFlowMinCost(net);
	typename Edge::CostType minCost = 0;

	typename Net<Edge>::EdgeIterator iter = net.getConstAllEdgesIterator();
	for (iter.begin(); !iter.end(); iter.move())
		if (iter.getEdge().flow > 0)
			minCost += iter.getEdge().flow * iter.getEdge().cost;

	return minCost;
}

void outputMinCostMaxFlow (const long long &minCost, ostream &os) {
	os << minCost << endl;
}



int main () {
	Net<CostedFlowedEdge<unsigned, int, long long> > net;
	int neededFlow;
	inputKingdom(net, neededFlow, cin);
	vector<list<CostedFlowedEdge<unsigned, int, long long > > > paths;
	long long minCost = solveKingdom(net, neededFlow, paths);
	outputKingdom(minCost, neededFlow, paths, cout);
	return 0;
}
