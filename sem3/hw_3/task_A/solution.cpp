#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>


using namespace std;


vector<unsigned long long> countOddPalindroms (const string &str) {
	vector<unsigned long long> oddCount(str.size(), 0);
	unsigned long long left = 0, right = 0;

	for (unsigned long long i = 0; i < str.size(); i ++) {
		unsigned long long count = (i >= right) ? 1 : min<unsigned long long>(oddCount[left + (right - i - 1)], right - i);
		
		for (unsigned long long j = count; i + j < str.size() && i >= j && str[i + j] == str[i - j]; j ++)
			count ++;

		oddCount[i] = count;

		if (i + count > right) {
			left = i - count + 1;
			right = i + count;
		}
	}

	return oddCount;
}

vector<unsigned long long> countEvenPalindroms (const string &str) {
	vector<unsigned long long> evenCount(str.size(), 0);
	unsigned long long left = 0, right = 0;

	for (unsigned long long i = 0; i < str.size() - 1; i ++) {
		unsigned long long count = (i + 1 >= right) ? 0 : min<unsigned long long>(evenCount[left + (right - i - 1) - 1], right - i - 1);

		for (unsigned long long j = count; i + j + 1 < str.size() && i >= j && str[i + j + 1] == str[i - j]; j ++)
			count ++;

		// baaaaab

		evenCount[i] = count;

		if (count > 0 && i + count + 1 >= right) {
			left = i - count + 1;
			right = i + count + 1;
		}
	}

	return evenCount;
}


/******************************************************************************/

class SegmentTree {
	public:
		SegmentTree (const vector<unsigned long long> &sequence);
	
		unsigned long long getSegmentMax (size_t a, size_t b);
		void increaseSegment (size_t a, size_t b, unsigned long long delta);
	
	private:
		unsigned long long getSegmentMax (size_t node, size_t left, size_t right, size_t a, size_t b);
		void increaseSegment (size_t node, size_t left, size_t right, size_t a, size_t b, unsigned long long delta);
	
	private:
		vector<pair<unsigned long long, unsigned long long> > _tree;
		const size_t _n;
};

SegmentTree::SegmentTree(const vector<unsigned long long>& sequence) : _n(1ull << (size_t)ceil(log2(sequence.size()))) {
	_tree.assign(2 * _n - 1, pair<unsigned long long, unsigned long long>(0, 0));
	
	for (size_t i = 0; i < sequence.size(); i++)
		_tree[_tree.size() / 2 + i].first = sequence[i];
	
	for (long i = _tree.size() / 2 - 1; i >= 0; i--)
		_tree[i].first = max(_tree[2*i + 1].first, _tree[2*i + 2].first);
}

unsigned long long SegmentTree::getSegmentMax(size_t a, size_t b) {
	return getSegmentMax(0, 0, _n - 1, a, b);
}

unsigned long long SegmentTree::getSegmentMax(size_t node, size_t left, size_t right, size_t a, size_t b) {
	if (a > b || left > right)
		return 0;

	if (a == left && b == right)
		return _tree[node].first + _tree[node].second;

	size_t med = (left + right) / 2;
	unsigned long long leftMax = getSegmentMax(2 * node + 1, left, med, a, min(b, med));
	unsigned long long rightMax = getSegmentMax(2 * node + 2, med + 1, right, max(a, med + 1), b);
	return max(rightMax, leftMax) + _tree[node].second;
}

void SegmentTree::increaseSegment(size_t a, size_t b, unsigned long long delta) {
	increaseSegment(0, 0, _n - 1, a, b, delta);
}

void SegmentTree::increaseSegment(size_t node, size_t left, size_t right, size_t a, size_t b, unsigned long long delta) {
	if (a > b || left > right)
		return;

	if (a == left && b == right) {
		_tree[node].second += delta;
		return;
	}

	size_t med = (left + right) / 2;
	increaseSegment(2 * node + 1, left, med, a, min(b, med), delta);
	increaseSegment(2 * node + 2, med + 1, right, max(a, med + 1), b, delta);
	_tree[node].first = max(_tree[2*node + 1].first + _tree[2*node + 1].second, _tree[2*node + 2].first + _tree[2*node + 2].second);
}

/******************************************************************************/


int main () {
	string str;
	cin >> str;

	vector<unsigned long long> oddCount = countOddPalindroms(str);
	vector<unsigned long long> evenCount = countEvenPalindroms(str);

	SegmentTree start(vector<unsigned long long>(str.size(), 0));
	SegmentTree end(vector<unsigned long long>(str.size(), 0));


	for (unsigned long long i = 0; i < str.size(); i ++) {
		start.increaseSegment(i + 1 - oddCount[i], i, 1);

		if (evenCount[i] != 0)
			start.increaseSegment(i + 1 - evenCount[i], i, 1);

		end.increaseSegment(i, i + oddCount[i] - 1, 1);

		if (evenCount[i] != 0)
			end.increaseSegment(i + 1, i + evenCount[i], 1);
	}


	unsigned long long ans = 0;





	for (unsigned long long i = 0; i < str.size() - 1; i ++) {
		ans += end.getSegmentMax(i, i)*start.getSegmentMax(i + 1, i + 1);
	}

	cout << ans << endl;


	return 0;
}