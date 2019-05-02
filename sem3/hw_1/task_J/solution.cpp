#include <iostream>
#include <functional>
#include <set>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>

using namespace std;


// Net class:

template <class Edge>
class Net {
	private:
		vector<vector<Edge> > edges;

	public:
		typedef typename Edge::IndexType IndexType;
		typedef typename Edge::FlowType FlowType;
		IndexType source;
		IndexType sink;

		Net (const IndexType &verticesCount = 0) {
			edges.resize(verticesCount);
		}

		void addEdge (const Edge &edge) {
			edges[edge.from].push_back(edge);
			edges[edge.to].push_back(edge.getReversed());
		}

		void increaseFlow (const IndexType &from, const IndexType &to, const FlowType &delta) {
			for (int i = 0; i < edges[from].size(); i ++) {
				if (edges[from][i].to == to) {
					edges[from][i].flow += delta;
					return;
				}
			}
		}

		vector<Edge> const & getEdges (const IndexType &vertex) const {
			return this->edges[vertex];
		}

		IndexType getVerticesCount () const {
			return edges.size();
		}

		void print (ostream &os = cout) const {
			for (IndexType vertex = 0; vertex < edges.size(); vertex ++) {
				for (const Edge &edge: edges[vertex]) {
					edge.print(os);
					os << " ";
				}

				os << endl;
			}
		}
};


// Set max flow:

template <class Edge>
typename Edge::FlowType findIncreasingPath (Net<Edge> &net, const vector<typename Edge::IndexType> &layers, vector<typename Edge::IndexType> &pointers, const typename Edge::IndexType &vertex, const typename Edge::IndexType &end, const typename Edge::FlowType &pushedFlow = numeric_limits<typename Edge::FlowType>::max()) {
	if (vertex == end)
		return pushedFlow;

	const vector<Edge> edges = net.getEdges(vertex);

	for (typename Edge::IndexType i = pointers[vertex]; i < edges.size(); i ++) {
		const typename Edge::IndexType to = edges[i].to;
		const typename Edge::FlowType recidualCapacity = edges[i].recidualCapacity();

		if (layers[to] != layers[vertex] + 1 || recidualCapacity == 0) {
			pointers[vertex] ++;
			continue;
		}

		typename Edge::FlowType newPushedFlow = min<typename Edge::FlowType>(pushedFlow, recidualCapacity);
		typename Edge::FlowType delta = findIncreasingPath(net, layers, pointers, to, end, newPushedFlow);

		if (delta > 0) {;
			net.increaseFlow(vertex, to, delta);
			net.increaseFlow(to, vertex, - delta);
			return delta;
		}

		else
			pointers[vertex] ++;
	}

	return 0;
}

template <class Edge>
bool buildLayeredNet (const Net<Edge> &net, vector<typename Edge::IndexType> &layers) {
	queue<typename Edge::IndexType> verticesQueue;
	verticesQueue.push(net.source);

	layers = vector<typename Edge::IndexType>(net.getVerticesCount(), -1);
	layers[net.source] = 0;

	while (!verticesQueue.empty()) {
		typename Edge::IndexType vertex = verticesQueue.front();
		verticesQueue.pop();

		for (const Edge &edge: net.getEdges(vertex)) {
			if (layers[edge.to] == -1 && edge.recidualCapacity() > 0) {
				layers[edge.to] = layers[vertex] + 1;
				verticesQueue.push(edge.to);
			}
		}
	}

	return (layers[net.sink] != -1);
}

template <class Edge>
typename Edge::FlowType setMaxFlow (Net<Edge> &net) {
	typename Edge::FlowType maxFlow = 0;
	vector<typename Edge::IndexType> layers(net.getVerticesCount(), -1);

	while (buildLayeredNet(net, layers)) {
		typename Edge::FlowType blockingFlow;
		vector<typename Edge::IndexType> pointers(net.getVerticesCount(), 0);
		
		do {
			blockingFlow = findIncreasingPath(net, layers, pointers, net.source, net.sink);
			maxFlow += blockingFlow;
		} while (blockingFlow != 0);
	}

	return maxFlow;
}


// Find min cut:

template <class Edge>
void minCutDFS (const Net<Edge> &net, const typename Edge::IndexType &vertex, vector<bool> &used) {
	used[vertex] = true;

	for (const Edge &edge: net.getEdges(vertex))
		if (!used[edge.to] && edge.recidualCapacity() > 0)
			minCutDFS(net, edge.to, used);
}

template <class Edge>
vector<Edge> findMinCutEdges (const Net<Edge> &net) {
	vector<bool> used(net.getVerticesCount());
	minCutDFS(net, net.source, used);

	enum Sets {Source, Sink};
	vector<Sets> setsCharacteristic(net.getVerticesCount());

	for (typename Edge::IndexType vertex = 0; vertex < net.getVerticesCount(); vertex ++) {
		if (used[vertex] == true)
			setsCharacteristic[vertex] = Source;
		
		else
			setsCharacteristic[vertex] = Sink;
	}


	vector<Edge> edges;

	for (typename Edge::IndexType from = 0; from < net.getVerticesCount(); from ++)
		for (const Edge &edge: net.getEdges(from))
			if (setsCharacteristic[edge.from] == Sets::Source && setsCharacteristic[edge.to] == Sets::Sink)
				edges.push_back(edge);

	return edges;
}


// Construct field:
enum VertexType {black, grey, white};

void constructField (istream &is, vector<VertexType> &field, unsigned &width, unsigned &height) {
	is >> width >> height;

	field = vector<VertexType>(width * height, VertexType::grey);

	unsigned blackCount, whiteCount;
	is >> blackCount >> whiteCount;

	for (unsigned i = 0; i < blackCount; i ++) {
		unsigned x, y;
		is >> x >> y;
		field[(y - 1) * width + x - 1] = VertexType::black;
	}

	for (unsigned i = 0; i < whiteCount; i ++) {
		unsigned x, y;
		is >> x >> y;
		field[(y - 1) * width + x - 1] = VertexType::white;
	}
}

void printField (const unsigned &width, const unsigned &height, const vector<VertexType> &field) {
	for (unsigned i = 0; i < height; i ++) {
		for (unsigned j = 0; j < width; j ++) {
			if (field[i * width + j] == VertexType::black)
				cout << "x";

			else if (field[i * width + j] == VertexType::grey)
				cout << ".";

			else
				cout << "_";
		}

		cout << endl;
	}
}


// Construct net:

struct Vertex {
	unsigned vertexIndex;
	VertexType vertexType;

	Vertex (const unsigned &vertexIndex, const VertexType &vertexType) {
		this->vertexIndex = vertexIndex;
		this->vertexType = vertexType;
	}
};

template <typename FirstType = unsigned, typename SecondType = long long>
struct Edge {
	typedef FirstType IndexType;
	typedef SecondType FlowType;
	IndexType from;
	IndexType to;
	FlowType flow;
	FlowType capacity;

	Edge (const IndexType &from, const IndexType &to, const FlowType &capacity) {
		this->from = from;
		this->to = to;
		this->flow = 0;
		this->capacity = capacity;
	}

	FlowType recidualCapacity () const {
		return capacity - flow;
	}

	Edge getReversed () const {
		return Edge(to, from, 0);
	}

	void print (ostream &os = cout) const {
		os << from << " -> " << to << " (" << flow << "/" << capacity << ")";
	}
};

long long maxCapacity = 10000;

template <typename FirstType = unsigned, typename SecondType = long long>
bool operator == (const Edge<FirstType, SecondType> &edge, const Edge<FirstType, SecondType> &otherEdge) {
	return edge.from == otherEdge.from && edge.to == otherEdge.to && edge.flow == otherEdge.flow && edge.capacity == otherEdge.capacity;
}

template <class Edge>
void addEdgeBetweenCells (Net<Edge> &net, const Vertex &from, const Vertex &to) {
	const typename Edge::IndexType indexationDelta = net.getVerticesCount() / 2;

	typename Edge::FlowType capacity;

	if (from.vertexType == VertexType::black || to.vertexType == VertexType::black)
		return;

	net.addEdge(Edge(from.vertexIndex + indexationDelta, to.vertexIndex, maxCapacity));
}

template <class Edge>
Net<Edge> constructNet (istream &is, Net<Edge> &net, const unsigned &width, const unsigned &height, const vector<VertexType> &field) {
	net = Net<Edge>(width * height * 2);

	unsigned x, y;
	is >> x >> y;
	net.source = (y - 1)*width + (x - 1);

	is >> x >> y;
	net.sink = (y - 1)*width + (x - 1);
	net.sink += width*height;


	// Adding edges between cells:
	for (unsigned i = 0; i < height; i ++) {
		for (unsigned j = 0; j < width; j ++) {
			VertexType vertexType = field[i * width + j];

			if (i != 0) {
				VertexType upVertexType = field[(i - 1)*width + j];
				addEdgeBetweenCells(net, Vertex((i - 1)*width + j, upVertexType),  Vertex(i*width + j, vertexType));
			}

			if (j != 0) {
				VertexType leftVertexType = field[i*width + j - 1];
				addEdgeBetweenCells(net, Vertex(i*width + j - 1, leftVertexType),  Vertex(i*width + j, vertexType));
			}

			if (i != height - 1) {
				VertexType downVertexType = field[(i + 1)*width + j];
				addEdgeBetweenCells(net, Vertex((i + 1)*width + j, downVertexType),  Vertex(i*width + j, vertexType));
			}

			if (j != width - 1) {
				VertexType rightVertexType = field[i*width + j + 1];
				addEdgeBetweenCells(net, Vertex(i*width + j + 1, rightVertexType),  Vertex(i*width + j, vertexType));
			}
		}
	}

	// Adding edges in the cells:
	for (unsigned i = 0; i < field.size(); i ++) {
		if (field[i] == VertexType::black)
			continue;

		typename Edge::FlowType capacity = (field[i] == VertexType::white ? 1 : maxCapacity);
		net.addEdge(Edge(i, i + field.size(), capacity));
	}

	return net;
}


// MAIN:

int main () {
	unsigned width, height;
	vector<VertexType> field;
	constructField(cin, field, width, height);

	Net<Edge<unsigned, long long> > net;
	constructNet(cin, net, width, height, field);


	if (setMaxFlow(net) >= maxCapacity)
		cout << "-1" << endl;
	
	else {
		vector<Edge<unsigned, long long> > minCutEdges = findMinCutEdges(net);

		vector<bool> answerCharacteristic(field.size(), false);
		vector<unsigned> answer;

		for (const Edge<unsigned, long long> &edge: minCutEdges)
			if (edge.from % field.size() == edge.to % field.size())
				answerCharacteristic[edge.from % field.size()] = true;	

		for (unsigned vertex = 0; vertex < answerCharacteristic.size(); vertex ++)
			if (answerCharacteristic[vertex] && field[vertex] == VertexType::white)
				answer.push_back(vertex);

		cout << answer.size() << endl;

		for (const unsigned &vertex: answer)
			cout << (vertex % width + 1) << " " << (vertex / width + 1) << endl;
	}
		
	return 0;
}
