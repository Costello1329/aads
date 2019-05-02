#include <iostream>
#include <vector>
#include <cmath>
#include <string>

typedef unsigned long long ull;
using namespace std;


template <typename Type>
ostream &operator << (ostream &os, const vector<Type> &vec) {
	for (Type el: vec)
		os << el;
	return os;
}


bool checkProfiles (ull first, ull second, size_t size) {
	vector<bool> binaryFirst(size, false);
	vector<bool> binarySecond(size, false);

	for (int i = 0; i < size; i ++) {
		binaryFirst[size - i - 1] = first % 2;
		first /= 2;
	}

	for (int i = 0; i < size; i ++) {
		binarySecond[size - i - 1] = second % 2;
		second /= 2;
	}

	for (int i = 0; i < size - 1; i ++)
		if (binaryFirst[i] == binaryFirst[i + 1])
			if (binaryFirst[i] == binarySecond[i])
				if (binaryFirst[i] == binarySecond[i + 1])
					return false;

	return true;
}


int main () {
	int n, m;
	cin >> n >> m;

	if (n > m)
		swap(n, m);


	vector<vector<bool> > admissible(exp2(n), vector<bool>(exp2(n)));

	for (ull i = 0; i < exp2(n); i ++)
		for (ull j = i; j < exp2(n); j ++)
			admissible[i][j] = admissible[j][i] = checkProfiles(i, j, n);


	vector<vector<ull> > count(m, vector<ull>(exp2(n), 0));
	count[0] = vector<ull>(exp2(n), 1);

	for (int depth = 1; depth < m; depth ++)
		for (int i = 0; i < exp2(n); i ++)
			for (int j = 0; j < exp2(n); j ++)
				count[depth][i] += count[depth - 1][j] * admissible[i][j];


	ull answer = 0;

	for (int i = 0; i < exp2(n); i ++)
		answer += count[m - 1][i];

	cout << answer << endl;

	return 0;
}
