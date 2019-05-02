#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

using namespace std;


vector<unsigned> zArrayFromPrefixArray (const vector<unsigned> &prefixArray) {
	vector<unsigned> zArray(prefixArray.size(), 0);

	for (unsigned i = 1; i < prefixArray.size(); i ++)
		if (prefixArray[i] != 0)
				zArray[i - prefixArray[i] + 1] = prefixArray[i];
 
	zArray[0] = prefixArray.size();
		
	if (zArray[1])
		for (unsigned i = 1; i < zArray[1]; i ++)
			zArray[i + 1] = zArray[1] - i;
	 
	int temp;
	
	for (unsigned i = zArray[1] + 1; i < prefixArray.size() - 1; i ++) {
		temp = i;
	   
		if (zArray[i] && !zArray[i + 1]) {
			for (unsigned j = 1; j < zArray[i] && zArray[i + j] <= zArray[j]; j ++) {
				zArray[i + j] = min(zArray[j], zArray[i] - j);
				temp = i + j;
			}
		}
			
		i = temp;  
	}

	return zArray;
}

int main () {
	unsigned size;
	cin >> size;

	vector<unsigned> prefixArray(size);

	for (unsigned i = 0; i < size; i ++)
		cin >> prefixArray[i];

	vector<unsigned> zArray = zArrayFromPrefixArray(prefixArray);

	for (const unsigned &el: zArray)
		cout << el << " ";

	cout << endl;

	return 0;
}
