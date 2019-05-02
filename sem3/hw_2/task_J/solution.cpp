#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>

using namespace std;

/******************************************************************************/

// Segment tree with operations of taking maximum and increasing a segment
class SegmentTree{
public:
    // Constructs the tree
    SegmentTree(const std::vector<unsigned>& sequence);

    // Returns max on the segment [a, b]
    unsigned getSegmentMax(size_t a, size_t b);
    // Increases segment [a, b] by delta
    void increaseSegment(size_t a, size_t b, unsigned delta);

private:
    // Recursive function for taking max on a segment
    // node - root of current subtree,
    // left - left bound of current subtree in sequence,
    // right - right bound of current subtree
    // a - left bound of segment to increase in current subtree
    // b - right bound of segment to increase in current subtree
    unsigned getSegmentMax(size_t node, size_t left, size_t right, size_t a, size_t b);
    // Recursive function for increasing segment by delta
    // Arguments same as for max
    void increaseSegment(size_t node, size_t left, size_t right, size_t a, size_t b, unsigned delta);

private:
    // Buffer storing the tree like a heap
    // Tree is stored as a binary tree with full lower level. All unused leafs are initialized with 0
    std::vector<std::pair<unsigned, unsigned>> _tree;
    // Number of tree leafs
    const size_t _n;
};


/******************************************************************************/

SegmentTree::SegmentTree(const std::vector<unsigned>& sequence)
        : _n(1ull << (size_t)ceil(log2(sequence.size())))
{
    // Reserving memory for storing tree
    // _tree[i].first - current minimum in relevant segment
    // _tree[i].second - number to increase relevant segment
    _tree.assign(2 * _n - 1, std::pair<unsigned, unsigned>(0, 0));
    // Initialising leafs with sequence members
    for (size_t i = 0; i < sequence.size(); i++)
        _tree[_tree.size() / 2 + i].first = sequence[i];
    // Counting min for every other node
    for (long i = _tree.size() / 2 - 1; i >= 0; i--)
        _tree[i].first = std::max(_tree[2*i + 1].first, _tree[2*i + 2].first);
}



unsigned SegmentTree::getSegmentMax(size_t a, size_t b)
{
    return getSegmentMax(0, 0, _n - 1, a, b);
}



unsigned SegmentTree::getSegmentMax(size_t node, size_t left, size_t right, size_t a, size_t b)
{
    // If DFS is in obviously wrong place, stop searching
    if (a > b || left > right)
        return 0;
    // If relevant segment is included in segment to find max, return current max
    if (a == left && b == right)
        return _tree[node].first + _tree[node].second;

    // Visit child nodes
    size_t med = (left + right) / 2;
    unsigned leftMax = getSegmentMax(2 * node + 1, left, med, a, std::min(b, med));
    unsigned rightMax = getSegmentMax(2 * node + 2, med + 1, right, std::max(a, med + 1), b);
    // Return max increased by subtree delta
    return std::max(rightMax, leftMax) + _tree[node].second;
}



void SegmentTree::increaseSegment(size_t a, size_t b, unsigned delta)
{
    increaseSegment(0, 0, _n - 1, a, b, delta);
}



void SegmentTree::increaseSegment(size_t node,
                                  size_t left,
                                  size_t right,
                                  size_t a,
                                  size_t b,
                                  unsigned delta)
{
    // If DFS is in obviously wrong place, stop searching
    if (a > b || left > right)
        return;
    // If relevant segment is included in segment to increase, increase
    if (a == left && b == right) {
        _tree[node].second += delta;
        return;
    }

    // Visit child nodes
    size_t med = (left + right) / 2;
    increaseSegment(2 * node + 1, left, med, a, std::min(b, med), delta);
    increaseSegment(2 * node + 2, med + 1, right, std::max(a, med + 1), b, delta);
    // Fix max
    _tree[node].first = std::max(_tree[2*node + 1].first + _tree[2*node + 1].second,
                                 _tree[2*node + 2].first + _tree[2*node + 2].second);
}

vector<unsigned> zArrayFromString (const string &str) {
	vector<unsigned> zArray(str.size());
	zArray[0] = str.size();

	unsigned l = 0, r = 0;

	for (unsigned i = 1; i < str.size(); i ++) {
		if (i <= r)
			zArray[i] = min<unsigned>(r - i + 1, zArray[i - l]);

		for (unsigned j = i + zArray[i]; j < str.size() && str[j] == str[j - i]; j ++)
			zArray[i] ++;

		if (i + zArray[i] - 1 > r) {
			l = i;
			r = i + zArray[i] - 1;
		}
	}

	return zArray;
}

// ababababa

int main () {
	string str;
	cin >> str;
	vector<unsigned> zArray = zArrayFromString(str);

	vector<unsigned> arr = vector<unsigned>(zArray.size(), 0);
	SegmentTree segmentTree(arr);

	for (unsigned i = 1; i < str.size(); i ++)
		if (zArray[i] > 0)
			segmentTree.increaseSegment(i, i + zArray[i] - 1, 1);

	for (unsigned i = 0; i < str.size(); i ++)
		arr[i] = segmentTree.getSegmentMax(i, i);


	unsigned maxIndex = 0, curMax = 0;

	for (unsigned i = 1; i < str.size(); i ++) {
		if (arr[i] > curMax) {
			maxIndex = i;
			curMax = arr[i];
		}
	}


	for (unsigned i = 0; i <= maxIndex; i ++)
		cout << str[i];

	cout << endl;

	return 0;
}
