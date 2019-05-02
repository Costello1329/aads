#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

using namespace std;


vector<unsigned> prefixArrayFromZArray (const vector<unsigned> &zArray) {
	vector<unsigned> prefixArray(zArray.size(), 0);

	for (unsigned i = 1; i < zArray.size(); i ++)
		if (zArray[i] != 0)
			for (int j = zArray[i] - 1; j >= 0 && prefixArray[i + j] == 0; j --)
				prefixArray[i + j] = j + 1;

	return prefixArray;
}

int main () {
	unsigned size;
	cin >> size;

	vector<unsigned> zArray(size);

	for (unsigned i = 0; i < size; i ++)
		cin >> zArray[i];

	vector<unsigned> prefixArray = prefixArrayFromZArray(zArray);

	for (const unsigned &el: prefixArray)
		cout << el << " ";

	cout << endl;

	return 0;
}
