#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

using std::min;
using std::string;
using std::vector;
using std::istream;
using std::ostream;
using std::cin;
using std::cout;
using std::endl;


vector<unsigned> prefixArrayFromString (const string &str) {
	vector<unsigned> prefixArray(str.size(), 0);

	for (unsigned i = 1; i < str.size(); i ++) {
		unsigned j = prefixArray[i - 1];

		while (j > 0 && str[i] != str[j])
			j = prefixArray[j - 1];

		if (str[i] == str[j])
			++ j;

		prefixArray[i] = j;
	}

	return prefixArray;
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


int main () {
	string str;
	cin >> str;
	vector<unsigned> prefixArray = prefixArrayFromString(str);

	for (const unsigned &el: prefixArray)
		cout << el << " ";

	cout << endl;
	return 0;
}
