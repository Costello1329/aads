#include <algorithm>
#include <iomanip>
#include <fstream>
#include <limits>
#include <vector>
#include <queue>
#include <set>

using namespace std;
const size_t maxSizeOfArray = 700;


// Net class: --------------------------------------------------


template <class Edge>
struct Net {
	typedef typename Edge::IndexType IndexType;
	typedef Edge* EdgePointer;
	typedef pair<EdgePointer, EdgePointer> Arc;

	typename Edge::IndexType source;
	typename Edge::IndexType sink;
	Arc edges[maxSizeOfArray][maxSizeOfArray];
	IndexType outerEdgesCount[maxSizeOfArray];
	typename Edge::IndexType verticesCount;

	explicit Net (const IndexType &verticesCount = 0) : verticesCount(verticesCount) {
		for (IndexType i = 0; i < verticesCount; i ++)
			outerEdgesCount[i] = 0;
	}

	void addEdge (const Edge &edge) {
		EdgePointer edgePointer = new Edge(edge);
		EdgePointer reversedEdgePointer = new Edge(edge.reversed());
		edges[edgePointer->from][outerEdgesCount[edgePointer->from] ++] = make_pair(edgePointer, reversedEdgePointer);
		edges[reversedEdgePointer->from][outerEdgesCount[reversedEdgePointer->from] ++] = make_pair(reversedEdgePointer, edgePointer);
	}
};


// Min Cost Max Flow algorithm: --------------------------------------------------

template <class Edge>
void findMinCostPathsToAllVerticesFordBellman (const Net<Edge> &net, const typename Edge::IndexType &vertex, typename Edge::CostType (&costs)[maxSizeOfArray]) {
	for (typename Edge::IndexType i = 0; i < net.verticesCount; i ++)
		costs[i] = numeric_limits<typename Edge::CostType>::max();
	
	costs[vertex] = 0;
	
	for (typename Edge::IndexType iteration = 0; iteration < net.verticesCount - 1; iteration ++) {
		bool changed = false;

		for (typename Edge::IndexType from = 0; from < net.verticesCount; from ++) {
			for (typename Edge::IndexType i = 0; i < net.outerEdgesCount[from]; i ++) {
				typename Net<Edge>::Arc arc = net.edges[from][i];
				Edge edge = *arc.first;

				if (edge.residualCapacity() > 0 && costs[edge.from] < numeric_limits<typename Edge::CostType>::max()) {
					changed = true;
					costs[edge.to] = min<typename Edge::CostType>(costs[edge.to], costs[edge.from] + edge.cost);
				}
			}
		}
		
		if (!changed)
			break;		
	}
}

template <class Edge>
void findMinCostPathsToAllVerticesDijkstra (const Net<Edge> &net, const typename Edge::CostType (&potentials)[maxSizeOfArray], const typename Edge::IndexType &start, typename Edge::CostType (&minDistances)[maxSizeOfArray]) {
	for (typename Edge::IndexType i = 0; i < net.verticesCount; i ++)
		minDistances[i] = numeric_limits<typename Edge::CostType>::max();
	
	minDistances[start] = 0;


	bool used[maxSizeOfArray];

	for (typename Edge::IndexType i = 0; i < net.verticesCount; i ++)
		used[i] = false;
	

	for (typename Edge::IndexType i = 0; i < net.verticesCount; i ++) {
		typename Edge::CostType currentMinDistance = numeric_limits<typename Edge::CostType>::max();
		typename Edge::IndexType minVertex;

		for (typename Edge::IndexType j = 0; j < net.verticesCount; j ++) {
			if (!used[j] && minDistances[j] < currentMinDistance) {
				currentMinDistance = minDistances[j];
				minVertex = j;
			}
		}

		used[minVertex] = true;

		if (currentMinDistance == numeric_limits<typename Edge::CostType>::max())
			break;

		for (typename Edge::IndexType i = 0; i < net.outerEdgesCount[minVertex]; i ++) {
			typename Net<Edge>::Arc arc = net.edges[minVertex][i];
			Edge edge = *arc.first;

			if (edge.residualCapacity() == 0)
				continue;

			if (potentials[edge.from] != numeric_limits<typename Edge::CostType>::max()) {
				edge.cost += potentials[edge.from];
				edge.cost -= potentials[edge.to];
			}
			
			else
				edge.cost = numeric_limits<typename Edge::CostType>::max();

			if (minDistances[edge.from] + edge.cost < minDistances[edge.to])
				minDistances[edge.to] = minDistances[edge.from] + edge.cost;
		}
	}
}

template <class Edge>
bool setFlowDFS (Net<Edge> &net, const typename Edge::CostType (&potentials)[maxSizeOfArray], const typename Edge::IndexType &vertex, const typename Edge::IndexType &destination, bool (&used)[maxSizeOfArray], typename Edge::FlowType &augmentingFlow) {
	if (used[vertex])
		return false;

	used[vertex] = true;

	if (vertex == destination)
		return true;

	const typename Edge::FlowType temp = augmentingFlow;

	for (typename Edge::IndexType i = 0; i < net.outerEdgesCount[vertex]; i ++) {
		typename Net<Edge>::Arc arc = net.edges[vertex][i];
		Edge edge = *arc.first;

		if (edge.residualCapacity() > 0 && edge.cost == potentials[edge.to] - potentials[edge.from]) {
			augmentingFlow = min<typename Edge::FlowType>(augmentingFlow, edge.residualCapacity());

			if (setFlowDFS(net, potentials, edge.to, destination, used, augmentingFlow)) {
				arc.first->flow += augmentingFlow;
				arc.second->flow -= augmentingFlow;
				return true;
			}

			else
				augmentingFlow = temp;
		}
	}

	return false;
}

template <class Edge>
void setMaxFlowMinCost (Net<Edge> &net) {
	typename Edge::CostType potentials[maxSizeOfArray];
	findMinCostPathsToAllVerticesFordBellman(net, net.source, potentials);

	while (true) {
		// Building minCost path:
		bool used[maxSizeOfArray];
		
		for (bool &isVertexUsed: used)
			isVertexUsed = false;

		typename Edge::FlowType augmentingFlow = numeric_limits<typename Edge::FlowType>::max();
		if (!setFlowDFS(net, potentials, net.source, net.sink, used, augmentingFlow))
			break;

		// Counting costs:
		typename Edge::CostType costs[maxSizeOfArray];
		findMinCostPathsToAllVerticesDijkstra(net, potentials, net.source, costs);

		// Recouting potentials:
		for (typename Edge::IndexType i = 0; i < net.verticesCount; i ++) {
			if (costs[i] != numeric_limits<typename Edge::CostType>::max())
				potentials[i] += costs[i];	

			else
				potentials[i] = costs[i];
		}
	}
}


// My non-lib code: --------------------------------------------------


// My custom Edge:
template <typename FirstType, typename SecondType, typename ThirdType>
struct CostedFlowedEdge {
	typedef FirstType IndexType;
	typedef SecondType FlowType;
	typedef ThirdType CostType;
	IndexType from;
	IndexType to;
	FlowType flow;
	FlowType capacity;
	CostType cost;

	explicit CostedFlowedEdge (const IndexType &from, const IndexType &to, const FlowType &capacity, const CostType &cost) : from(from), to(to), flow(0), capacity(capacity), cost(cost) {}

	FlowType residualCapacity () const {
		return capacity - flow;
	}

	CostedFlowedEdge reversed () const {
		return CostedFlowedEdge(to, from, 0, - cost);
	}

	bool operator == (const CostedFlowedEdge &otherEdge) const {
		return from == otherEdge.from && to == otherEdge.to && flow == otherEdge.flow && capacity == otherEdge.capacity;
	}
};

// Matching:
int main () {
	typedef CostedFlowedEdge<unsigned, int, int> Edge;
	typename Edge::IndexType verticesCount;
	scanf("%u", &verticesCount);

	Net<Edge> net(2 * verticesCount + 2);
	net.source = 0;
	net.sink = 2 * verticesCount + 1;

	for (typename Edge::IndexType from = 0; from < verticesCount; from ++) {
		net.addEdge(Edge(0, from + 1, 1, 0));
		net.addEdge(Edge(from + 1 + verticesCount, 2 * verticesCount + 1, 1, 0));

		for (typename Edge::IndexType to = 0; to < verticesCount; to ++) {
			typename Edge::CostType cost;
			scanf("%u", &cost);
			net.addEdge(Edge(from + 1, to + 1 + verticesCount, 1, cost));
		}
	}




	setMaxFlowMinCost(net);

	typename Edge::CostType minCost = 0;

	for (typename Edge::IndexType from = 0; from < net.verticesCount; from ++) {
		for (typename Edge::IndexType i = 0; i < net.outerEdgesCount[from]; i ++) {
			typename Net<Edge>::Arc arc = net.edges[from][i];
			Edge edge = *arc.first;

			if (edge.flow > 0)
				minCost += edge.flow * edge.cost;
		}
	}

	printf("%u\n", minCost);

	for (typename Edge::IndexType from = 0; from < net.verticesCount; from ++) {
		for (typename Edge::IndexType i = 0; i < net.outerEdgesCount[from]; i ++) {
			typename Net<Edge>::Arc arc = net.edges[from][i];
			Edge edge = *arc.first;

			if (edge.flow != 0 && edge.from != net.source && edge.to != net.source && edge.from != net.sink && edge.to != net.sink)
				if (edge.flow > 0)
					printf("%u %u\n", edge.from, edge.to - (unsigned)net.verticesCount/2 + 1);
		}
	}

	return 0;
}
