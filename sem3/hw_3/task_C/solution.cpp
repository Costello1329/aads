#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

using namespace std;


vector<unsigned> buildLCP (const string &str, const vector<unsigned> &suffixArray) {
	vector<unsigned> lcp(str.size() - 1);
	vector<unsigned> suffixPositionInSuffixArray(str.size());

	for (unsigned i = 0; i < str.size(); i ++)
		suffixPositionInSuffixArray[suffixArray[i]] = i;

	unsigned prefixMatchLength = 0;

	for (unsigned currentSuffix = 0; currentSuffix < str.size(); currentSuffix ++) {
		if (prefixMatchLength > 0)
			prefixMatchLength --;

		if (suffixPositionInSuffixArray[currentSuffix] == str.size() - 1)
			prefixMatchLength = 0;

		else {
			unsigned nextSuffix = suffixArray[suffixPositionInSuffixArray[currentSuffix] + 1];
			
			while (max<unsigned>(currentSuffix, nextSuffix) + prefixMatchLength < str.size() && str[currentSuffix + prefixMatchLength] == str[nextSuffix + prefixMatchLength])
				prefixMatchLength ++;

			lcp[suffixPositionInSuffixArray[currentSuffix]] = prefixMatchLength;
		}

	}

	return lcp;
}

int main () {
	unsigned size = 0;
	cin >> size;
	string str;
	cin >> str;
	vector<unsigned> suffixArray(str.size());

	for (unsigned &el: suffixArray) {
		cin >> el;
		el --;
	}

	vector<unsigned> lcp = buildLCP(str, suffixArray);

	for (const unsigned &el: lcp)
		cout << el << " ";
	cout << endl;
}
