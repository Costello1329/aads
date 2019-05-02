#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

int main () {
	const unsigned maxN = 100000;
	const unsigned maxK = sqrt(maxN);
	vector<vector<bool> > isWin(maxN + 1, vector<bool>(maxK + 1, false));

	for (int n = 0; n < isWin.size(); n ++) {
		for (int k = 1; k <= n && k < isWin[0].size(); k ++) {
			if (n == k)
				isWin[n][k] = true;
			
			else
				isWin[n][k] = !(isWin[n - k][k] && isWin[n - k - 1][k + 1]);
		}
	}

	
	size_t query = 0;
	
	while (true) {
		unsigned number;
		cin >> number;
			
		if (number == 0)
			break;

		cout << "Case #" << ++query << (!isWin[number - 1][1] ? ": First" : ": Second") << " player wins." << endl;
	}

	return 0;
}