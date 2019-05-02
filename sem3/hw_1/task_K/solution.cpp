#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>
#include <vector>
#include <queue>
#include <list>

using std::vector;
using std::queue;
using std::list;
using std::pair;
using std::make_pair;
using std::istream;
using std::ostream;
using std::cin;
using std::cout;
using std::endl;
using std::numeric_limits;
using std::min;
using std::max;



// Net class: --------------------------------------------------



template <class Edge>
class Net {
	private:
		typedef Edge* EdgePointer;
		typedef pair<EdgePointer, EdgePointer> Arc;
		typename Edge::IndexType source;
		typename Edge::IndexType sink;
		list<Arc>* edges;
		vector<list<Arc>*> innerEdges;
		vector<list<Arc>*> outerEdges;

		class UniversalEdgeIterator {
			friend class Net;

			private:
				list<Arc>* edgesList;
				typename list<Arc>::iterator edgeIterator;

				bool validated;
				enum UniversalIteratorType {inner, outer, all};
				UniversalIteratorType iteratorType;
	
				explicit UniversalEdgeIterator (list<Arc> *edgesList, const UniversalIteratorType &iteratorType) : edgesList(edgesList), iteratorType(iteratorType), validated(false)  {};

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

		UniversalEdgeIterator *allEdgesIterator;
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
				innerEdges[i] = new list<Arc>();
				outerEdges[i] = new list<Arc>();
				innerEdgesIterators[i] = new EdgeIterator(&(*innerEdges[i]), EdgeIterator::IteratorType::inner);
				outerEdgesIterators[i] = new EdgeIterator(&(*outerEdges[i]), EdgeIterator::IteratorType::outer);
			}
		}

	public:
		typedef UniversalEdgeIterator EdgeIterator;
		typedef typename Edge::IndexType IndexType;

		// Constructors, destructors:
		explicit Net (const IndexType &verticesCount = 0) {
			edges = new list<Arc>();
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



// Global Preflow-Push algorithm: --------------------------------------------------



template <class Edge>
struct PreflowPushProcessor {
	vector<typename Edge::FlowType> excess;
	vector<typename Edge::IndexType> height;

	typename Edge::IndexType verticesCount;
	typename Edge::IndexType source;
	typename Edge::IndexType sink;

	explicit PreflowPushProcessor (const Net<Edge> &net) : excess(vector<typename Edge::FlowType>(net.getVerticesCount(), 0)), height(vector<typename Edge::IndexType>(net.getVerticesCount(), 0)), verticesCount(net.getVerticesCount()), source(net.getSource()), sink(net.getSink()) {}

	virtual void push (const typename Net<Edge>::EdgeIterator &iter, const typename Edge::FlowType &pushingFlow) {
		iter.increaseFlow(pushingFlow);
		Edge edge = iter.getEdge();
		excess[edge.from] -= pushingFlow;
		excess[edge.to] += pushingFlow;
	}

	virtual void lift (typename Net<Edge>::EdgeIterator iter) {
		typename Edge::IndexType minHeight = numeric_limits<typename Edge::IndexType>::max();
		typename Edge::IndexType vertex;
	
		for (iter.begin(), vertex = iter.getEdge().from; !iter.end(); iter.move()) {
			Edge edge = iter.getEdge();
	
			if (edge.residualCapacity() > 0)
				minHeight = min<typename Edge::IndexType>(minHeight, height[edge.to]);
		}
	
		height[vertex] = minHeight + 1;
	}
};

template <class Edge>
void setMaxFlowWithPreflowPush (Net<Edge> &net, void (*preflowPushOperations)(Net<Edge>&, PreflowPushProcessor<Edge>*)) {
	PreflowPushProcessor<Edge> *processor = new PreflowPushProcessor<Edge>(net);
	processor->height[net.getSource()] = net.getVerticesCount();

	// Preflow-push setting:
	typename Net<Edge>::EdgeIterator iter = net.getConstOuterEdgeIterator(net.getSource());

	for (iter.begin(); !iter.end(); iter.move())
		processor->push(iter, iter.getEdge().residualCapacity());

	// Preflow-push operations:
	preflowPushOperations(net, processor);
}


// discharge preflow-push operations: --------------------------------------------------


template <class Edge>
inline void discharge (Net<Edge> &net, const typename Edge::IndexType &vertex, PreflowPushProcessor<Edge> *processor) {
	typename Net<Edge>::EdgeIterator &iter = net.getOuterEdgeIterator(vertex);

	while (processor->excess[vertex] > 0) {
		if (iter.end()) {
			iter.begin();
			processor->lift(iter);
		}

		else {
			Edge edge = iter.getEdge();

			if (edge.residualCapacity() > 0 && processor->height[vertex] == processor->height[edge.to] + 1) {
				typename Edge::FlowType pushingFlow = min<typename Edge::FlowType>(processor->excess[edge.from], edge.residualCapacity());
				processor->push(iter, pushingFlow);
			}

			else
				iter.move();
		}
	}
}

template <class Edge>
void dischargePreflowPushOperations (Net<Edge> &net, PreflowPushProcessor<Edge> *processor) {
	while (true) {
		bool discharged = false;

		for (typename Edge::IndexType vertex = 0; vertex < net.getVerticesCount(); vertex ++) {
			if (processor->excess[vertex] && vertex != net.getSource() && vertex != net.getSink()) {
				discharge(net, vertex, processor);
				discharged = true;
			}
		}

		if (!discharged)
			break;
	}
}


// Discharge highest preflow-push operations: --------------------------------------------------


template <class Edge>
struct DischargeHighestProcessor : public PreflowPushProcessor<Edge> {
	typename Edge::IndexType activeVerticesCount;
	vector<list<typename Edge::IndexType> > activeFloors;
	typename Edge::IndexType maxActiveFloor;

	typename Edge::FlowType maxActivePushedHeight;
	bool lastPushWasToSource;

	explicit DischargeHighestProcessor (PreflowPushProcessor<Edge> *processor) : PreflowPushProcessor<Edge>(*processor), activeVerticesCount(0), activeFloors(vector<list<typename Edge::IndexType> >(2 * processor->verticesCount)), maxActiveFloor(0), maxActivePushedHeight(0), lastPushWasToSource(0) {}

	void push (const typename Net<Edge>::EdgeIterator &iter, const typename Edge::FlowType &pushingFlow) override {
		Edge edge = iter.getEdge();
	
		if (edge.to != this->source && edge.to != this->sink) {
			if (this->excess[edge.to] == 0) {
				activeVerticesCount ++;
				activeFloors[this->height[edge.to]].push_back(edge.to);
			}
	
			maxActivePushedHeight = this->height[edge.to];
		}
	
		lastPushWasToSource = (edge.to == this->source);
	
		iter.increaseFlow(pushingFlow);
		this->excess[edge.from] -= pushingFlow;
		this->excess[edge.to] += pushingFlow;
	}

	void lift (typename Net<Edge>::EdgeIterator iter) override {
		typename Edge::IndexType minHeight = numeric_limits<typename Edge::IndexType>::max();
		typename Edge::IndexType vertex;
	
		for (iter.begin(), vertex = iter.getEdge().from; !iter.end(); iter.move()) {
			Edge edge = iter.getEdge();
	
			if (edge.residualCapacity() > 0)
				minHeight = min<typename Edge::IndexType>(minHeight, this->height[edge.to]);
		}
	
		this->height[vertex] = minHeight + 1;
	
		activeFloors[maxActiveFloor].pop_back();
		maxActiveFloor = this->height[vertex];
		activeFloors[maxActiveFloor].push_back(vertex);
	}
	
	void recalculateMaxActiveFloor () {
		activeVerticesCount --;
		activeFloors[maxActiveFloor].pop_back();
	
		if (activeFloors[maxActiveFloor].size() == 0) {
			if (!lastPushWasToSource)
				maxActiveFloor = maxActivePushedHeight;
	
			else {
				for (typename Edge::IndexType i = maxActiveFloor - 1;; i --) {
					if (activeFloors[i].size() != 0 || i == 0) {
						maxActiveFloor = i;
						break;
					}
				}
			}
		}
	
		lastPushWasToSource = false;
		maxActivePushedHeight = 0;
	}
};

template <class Edge>
void dischargeHighestPreflowPushOperations (Net<Edge> &net, PreflowPushProcessor<Edge> *processor) {
	DischargeHighestProcessor<Edge> *dischargeHighestProcessor = new DischargeHighestProcessor<Edge>(processor);

	// Setting up dischargeHighestProcessor:
	for (typename Edge::IndexType vertex = 0; vertex < net.getVerticesCount(); vertex ++) {
		if (dischargeHighestProcessor->excess[vertex] > 0 && vertex != net.getSource() && vertex != net.getSink()) {
			dischargeHighestProcessor->activeVerticesCount ++;
			dischargeHighestProcessor->activeFloors[dischargeHighestProcessor->height[vertex]].push_back(vertex);
			dischargeHighestProcessor->maxActiveFloor = max<typename Edge::IndexType>(dischargeHighestProcessor->maxActiveFloor, dischargeHighestProcessor->height[vertex]);
		}
	}

	// Main cycle:
	while (dischargeHighestProcessor->activeVerticesCount > 0) {
		const typename Edge::IndexType vertex = dischargeHighestProcessor->activeFloors[dischargeHighestProcessor->maxActiveFloor].back();
		discharge(net, vertex, dischargeHighestProcessor);
		dischargeHighestProcessor->recalculateMaxActiveFloor();
	}
}



// Global augmenting paths algorithm: --------------------------------------------------



template <class Edge>
Net<Edge> buildLayeredNetwork (const Net<Edge> &net) {
	// BFS init:
	queue<typename Edge::IndexType> verticesQueue;
	vector<bool> used(net.getVerticesCount(), false);
	verticesQueue.push(net.getSource());

	// Layered Net init:
	Net<Edge> layeredNet(net.getVerticesCount());
	layeredNet.setSource(net.getSource());
	layeredNet.setSink(net.getSink());
	vector<typename Edge::IndexType> layers(net.getVerticesCount(), numeric_limits<typename Edge::IndexType>::max());
	layers[net.getSource()] = 0;

	// BFS:
	while (!verticesQueue.empty()) {
		typename Edge::IndexType vertex = verticesQueue.front();
		verticesQueue.pop();
	
		if (used[vertex])
			continue;

		used[vertex] = true;
		
		typename Net<Edge>::EdgeIterator iter = net.getConstOuterEdgeIterator(vertex);

		for (iter.begin(); !iter.end(); iter.move()) {
			Edge edge = iter.getEdge();

			if (edge.residualCapacity() > 0 && layers[edge.from] + 1 <= layers[edge.to]) {
				verticesQueue.push(edge.to);
				
				Edge residualEdge = edge;
				residualEdge.capacity = edge.residualCapacity();
				residualEdge.flow = 0;
				layeredNet.addEdge(residualEdge);
				layers[edge.to] = layers[edge.from] + 1;
			}
		}
	}

	return layeredNet;
}

template <class Edge>
void transferFlowFromLayeredNetworkToNetwork (Net<Edge> &layeredNet, Net<Edge> &net) {
	// BFS init:
	queue<typename Edge::IndexType> verticesQueue;
	vector<bool> used(net.getVerticesCount(), false);
	verticesQueue.push(net.getSource());

	// Layered Net init:
	vector<typename Edge::IndexType> layers(net.getVerticesCount(), numeric_limits<typename Edge::IndexType>::max());
	layers[net.getSource()] = 0;

	// BFS:
	while (!verticesQueue.empty()) {
		typename Edge::IndexType vertex = verticesQueue.front();
		verticesQueue.pop();

		if (used[vertex])
			continue;

		used[vertex] = true;
		
		typename Net<Edge>::EdgeIterator iter = net.getConstOuterEdgeIterator(vertex);
		typename Net<Edge>::EdgeIterator &layeredNetIter = layeredNet.getOuterEdgeIterator(vertex);
		layeredNetIter.begin();

		for (iter.begin(); !iter.end() && !layeredNetIter.end(); iter.move()) {
			Edge edge = iter.getEdge();

			if (edge.residualCapacity() > 0 && layers[edge.from] + 1 <= layers[edge.to]) {
				verticesQueue.push(edge.to);

				layers[edge.to] = layers[edge.from] + 1;

				while (layeredNetIter.getEdge().capacity == 0)
					layeredNetIter.move();
				
				iter.increaseFlow(layeredNetIter.getEdge().flow);
				layeredNetIter.move();	
			}
		}
	}
}

template <class Edge>
void setMaxFlowWithAugmentingPaths (Net<Edge> &net, void (*setBlockingFlow)(Net<Edge>&)) {
	while (true) {
		Net<Edge> layeredNet = buildLayeredNetwork(net);

		setBlockingFlow(layeredNet);

		if (getFlowWithCheck(layeredNet) == 0)
			break;

		else
			transferFlowFromLayeredNetworkToNetwork(layeredNet, net);
	}
}


// Dinic Blocking flow algorithm: --------------------------------------------------


template <class Edge>
typename Edge::FlowType tryPushFlow (Net<Edge> &layeredNet, const typename Edge::IndexType &vertex, const typename Edge::FlowType &pushingFlow = numeric_limits<typename Edge::FlowType>::max()) {
	if (vertex == layeredNet.getSink() || pushingFlow == 0)
		return pushingFlow;

	for (typename Net<Edge>::EdgeIterator iter = layeredNet.getConstOuterEdgeIterator(vertex); !iter.end(); iter.move()) {
		Edge edge = iter.getEdge();

		if (edge.capacity == 0)
			continue;

		typename Edge::FlowType newPushingFlow = min<typename Edge::FlowType>(pushingFlow, edge.residualCapacity());
		typename Edge::FlowType pushedFlow = tryPushFlow(layeredNet, edge.to, newPushingFlow);

		if (pushedFlow != 0) {
			iter.increaseFlow(pushedFlow);
			return pushedFlow;
		}
	}

	if (!layeredNet.getOuterEdgeIterator(vertex).end())
		layeredNet.getOuterEdgeIterator(vertex).move();
	
	return 0;
}

template <class Edge>
void setBlockingFlowWithDinic (Net<Edge> &layeredNet) {
	while (tryPushFlow(layeredNet, layeredNet.getSource()) != 0) {};
}


// Malhotra-Kumar-Maheshwari Blocking flow algorithm: --------------------------------------------------


template <class Edge>
struct MalhotraKumarMaheshwariProcessor {
	vector<typename Edge::FlowType> innerPotentials;
	vector<typename Edge::FlowType> outerPotentials;
	vector<bool> isDeleted;

	enum pushingFlowDirection {fromSource, toSink};

	explicit MalhotraKumarMaheshwariProcessor (const typename Edge::IndexType &verticesCount) {
		innerPotentials.resize(verticesCount, 0);
		outerPotentials.resize(verticesCount, 0);
		isDeleted.resize(verticesCount, false);
	}
};

template <class Edge>
inline typename Edge::FlowType setPotentialOneSide (typename Net<Edge>::EdgeIterator iter) {
	typename Edge::FlowType potential = 0;

	for (iter.begin(); !iter.end(); iter.move())
		potential += iter.getEdge().residualCapacity();

	return potential;
}

template <class Edge>
inline typename Edge::FlowType getPotential (const typename Edge::IndexType &vertex, MalhotraKumarMaheshwariProcessor<Edge> *processor) {
	return min<typename Edge::FlowType>(processor->outerPotentials[vertex], processor->innerPotentials[vertex]);
}

template <class Edge>
inline void deleteVertexOneSide (const Net<Edge> &layeredNet, typename Net<Edge>::EdgeIterator iter, MalhotraKumarMaheshwariProcessor<Edge> *processor) {
	for (iter.begin(); !iter.end(); iter.move()) {
		Edge edge = iter.getEdge();

		if (edge.capacity == 0)
			continue;

		typename Edge::IndexType nextVertex = (iter.getIteratorType() == Net<Edge>::EdgeIterator::IteratorType::outer ? edge.to : edge.from);
		(iter.getIteratorType() == Net<Edge>::EdgeIterator::IteratorType::outer ? processor->innerPotentials[nextVertex] : processor->outerPotentials[nextVertex]) -= edge.residualCapacity();

		if (getPotential(nextVertex, processor) == 0)
			deleteVertex(layeredNet, nextVertex, processor);
	}
}

template <class Edge>
void deleteVertex (const Net<Edge> &layeredNet, const typename Edge::IndexType &vertex, MalhotraKumarMaheshwariProcessor<Edge> *processor) {
	if (processor->isDeleted[vertex])
		return;

	processor->isDeleted[vertex] = true;
	deleteVertexOneSide<Edge>(layeredNet, layeredNet.getConstOuterEdgeIterator(vertex), processor);
	deleteVertexOneSide<Edge>(layeredNet, layeredNet.getConstInnerEdgeIterator(vertex), processor);
}

template <typename Edge>
void pushFlowFromRoot (Net<Edge> &layeredNet, const typename Edge::IndexType &root, const typename Edge::FlowType &pushingFlow, const typename MalhotraKumarMaheshwariProcessor<Edge>::pushingFlowDirection &pushingFlowDirection, MalhotraKumarMaheshwariProcessor<Edge> *processor) {
	// BFS init:
	queue<typename Edge::IndexType> verticesQueue;
	vector<bool> used(layeredNet.getVerticesCount(), false);
	verticesQueue.push(root);
	used[root] = true;

	// Other data init:
	vector<typename Edge::FlowType> excessOrLack(layeredNet.getVerticesCount(), 0);
	excessOrLack[root] = pushingFlow;
	bool pushingToTheSink = (pushingFlowDirection == MalhotraKumarMaheshwariProcessor<Edge>::pushingFlowDirection::toSink);

	// BFS:
	while (!verticesQueue.empty()) {
		typename Edge::IndexType vertex = verticesQueue.front();
		verticesQueue.pop();

		if (vertex == (pushingToTheSink ? layeredNet.getSink() : layeredNet.getSource()))
			break;

		for (typename Net<Edge>::EdgeIterator &iter = (pushingToTheSink ? layeredNet.getOuterEdgeIterator(vertex) : layeredNet.getInnerEdgeIterator(vertex)); !iter.end(); iter.move()) {
			Edge edge = iter.getEdge();
			typename Edge::IndexType nextVertex = (pushingToTheSink ? edge.to : edge.from);

			if (edge.capacity > 0 && !processor->isDeleted[nextVertex]) {
				if (!used[nextVertex]) {
					verticesQueue.push(nextVertex);
					used[nextVertex] = true;
				}
				
				typename Edge::FlowType pushedFlow = min<typename Edge::FlowType>(excessOrLack[vertex], edge.residualCapacity());
				excessOrLack[vertex] -= pushedFlow;
				excessOrLack[nextVertex] += pushedFlow;
				iter.increaseFlow(pushedFlow);
				processor->outerPotentials[edge.from] -= pushedFlow;
				processor->innerPotentials[edge.to] -= pushedFlow;
			}

			if (excessOrLack[vertex] == 0)
				break;
		}
	}
}

template <class Edge>
void setBlockingFlowWithMalhotraKumarMaheshwari (Net<Edge> &layeredNet) {
	// Initialize processor:
	MalhotraKumarMaheshwariProcessor<Edge> *processor = new MalhotraKumarMaheshwariProcessor<Edge>(layeredNet.getVerticesCount());

	// SetPotentials:
	for (typename Edge::IndexType vertex = 0; vertex < layeredNet.getVerticesCount(); vertex ++) {
		processor->innerPotentials[vertex] = setPotentialOneSide<Edge>(layeredNet.getConstInnerEdgeIterator(vertex));
		processor->outerPotentials[vertex] = setPotentialOneSide<Edge>(layeredNet.getConstOuterEdgeIterator(vertex));
	}

	processor->innerPotentials[layeredNet.getSource()] = numeric_limits<typename Edge::FlowType>::max();
	processor->outerPotentials[layeredNet.getSink()] = numeric_limits<typename Edge::FlowType>::max();


	// Main cycle:
	while (true) {
		// Deleting traverse:
		for (typename Edge::IndexType vertex = 0; vertex < layeredNet.getVerticesCount(); vertex ++)
			if (!processor->isDeleted[vertex] && getPotential(vertex, processor) == 0)
				deleteVertex(layeredNet, vertex, processor);

		if (processor->isDeleted[layeredNet.getSource()] || processor->isDeleted[layeredNet.getSink()])
			break;

		// Finding root:
		typename Edge::IndexType root = layeredNet.getSource();

		for (typename Edge::IndexType vertex = 0; vertex < layeredNet.getVerticesCount(); vertex ++)
			if (!processor->isDeleted[vertex] && getPotential(vertex, processor) <= getPotential(root, processor))
				root = vertex;
		
		// Pushing flow through the root:
		typename Edge::FlowType pushingFlow = getPotential(root, processor);
		pushFlowFromRoot(layeredNet, root, pushingFlow, MalhotraKumarMaheshwariProcessor<Edge>::pushingFlowDirection::toSink, processor);
		pushFlowFromRoot(layeredNet, root, pushingFlow, MalhotraKumarMaheshwariProcessor<Edge>::pushingFlowDirection::fromSource, processor);
	}
}





// My non-lib code: --------------------------------------------------





// My custom Edge:
template <typename FirstType = unsigned, typename SecondType = long long>
struct SimpleEdge {
	typedef FirstType IndexType;
	typedef SecondType FlowType;
	IndexType from;
	IndexType to;
	FlowType flow;
	FlowType capacity;

	explicit SimpleEdge (const IndexType &from, const IndexType &to, const FlowType &capacity) : from(from), to(to), flow(0), capacity(capacity) {}

	FlowType residualCapacity () const {
		return capacity - flow;
	}

	SimpleEdge reversed () const {
		return SimpleEdge(to, from, 0);
	}
};


// Solving Matan:
Net<SimpleEdge<unsigned, long long> > inputMatan (istream &is) {
	Net<SimpleEdge<unsigned, long long> > net;

	unsigned verticesCount;
	is >> verticesCount;

	net.setSource(verticesCount);
	net.setSink(verticesCount + 1);

	for (unsigned vertex = 0; vertex < verticesCount; vertex ++) {
		long long usability;
		is >> usability;

		if (usability > 0)
			net.addEdge(SimpleEdge<unsigned, long long>(net.getSource(), vertex, usability));

		else
			net.addEdge(SimpleEdge<unsigned, long long>(vertex, net.getSink(), - usability));
	}


	const long long maxCapacity = verticesCount * 1000 + 1;

	for (unsigned from = 0; from < verticesCount; from ++) {
		unsigned outerEdgesCount;
		is >> outerEdgesCount;

		for (unsigned edgeIndex = 0; edgeIndex < outerEdgesCount; edgeIndex ++) {
			unsigned to;
			is >> to;
			-- to;
			net.addEdge(SimpleEdge<unsigned, long long>(from, to, maxCapacity));
		}
	}

	return net;
}

long long solveMatan (Net<SimpleEdge<unsigned, long long> > &net) {
	long long positiveSum = 0;

	typename Net<SimpleEdge<unsigned, long long> >::EdgeIterator iter = net.getConstOuterEdgeIterator(net.getSource());

	for (iter.begin(); !iter.end(); iter.move())
		positiveSum += iter.getEdge().capacity;
	
	setMaxFlowWithPreflowPush(net, dischargeHighestPreflowPushOperations);
	long long maxFlow = getFlowWithoutCheck(net);
	
	return positiveSum - maxFlow;
}

void outputMatan (const long long &answer, ostream &os) {
	os << answer << endl;
}


// Solving Max Flow:
Net<SimpleEdge<unsigned, long long> > inputMaxFlow (istream &is) {
	Net<SimpleEdge<unsigned, long long> > net;

	unsigned verticesCount, edgesCount;
	is >> verticesCount >> edgesCount;

	net.setSource(0);
	net.setSink(verticesCount - 1);

	for (unsigned edge = 0; edge < edgesCount; edge ++) {
		unsigned from, to;
		long long capacity;
		is >> from >> to >> capacity;
		net.addEdge(SimpleEdge<unsigned, long long>(from - 1, to - 1, capacity));
	}

	return net;
}

long long solveMaxFlow (Net<SimpleEdge<unsigned, long long> > &net) {
	setMaxFlowWithPreflowPush(net, dischargeHighestPreflowPushOperations);
	return getFlowWithoutCheck(net);
}

void outputMaxFlow (const long long &maxFlow, ostream &os) {
	os << maxFlow << endl;
}


// Main:
int main () {
	Net<SimpleEdge<unsigned, long long> > net = inputMatan(cin);
	long long answer = solveMatan(net);
	outputMatan(answer, cout);
	return 0;
}
