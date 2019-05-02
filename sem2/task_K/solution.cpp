#include <iostream>
#include <fstream>
#include <vector>


typedef unsigned int ul;
using namespace std;


inline bool bit (const ul &num, const ushort &index) {
	return (bool)((num & (1 << index)) >> index);
}

inline void printBin (const ul &num) {
	for (int i = sizeof(num) * 8 - 1; i >= 0; i --)
		cout << bit(num, i);
}

inline ul exp2(ushort pow, ul mod) {
	if (pow == 0)
		return 1;

	if (mod % 2 == 0)
		return exp2(pow / 2, mod) * exp2(pow / 2, mod) % mod;

	else
		return (exp2(pow - 1, mod) << 1) % mod;
}

inline ul solve (ushort n, ushort m) {
	const ul mod = (1 << 30) | 1;

	if (n > m)
		swap(n, m);

	if (n == 1)
		return exp2(m, mod);

	const ushort size = n + 1;
	const ul sizeCount = 1 << size;


	// Start:

	vector<ul> count(sizeCount, 1);

	// Step:

	for (ushort d = 0; d < n*m - size; d ++) {
		vector<ul> newCount(sizeCount, 0);
		ushort i = d % n;

		for (ul p = 0; p < sizeCount; p ++) {
			if (count[p] == 0)
				continue;

			ul p1, p2;
			bool newRow = false;
		
			if (i != n - 1) {
				p1 = p ^ (1 << (size - i - 2));
				p2 = p;

				if (p1 > p2)
					swap(p1, p2);
			}
		
			else {
				p1 = p >> 1;
				p2 = (p >> 1) | (1 << (size - 1));
				newRow = true;
			}
		
			bool b1 = bit(p, size - i - 3);
			bool b2 = bit(p, size - i - 2);
			bool b3 = bit(p, size - i - 1);


			if (b1 == b2 && b2 == b3 && !newRow) {
				newCount[b2 ? p1 : p2] += count[p] % mod;
				newCount[b2 ? p1 : p2] %= mod;
			}

			else {
				newCount[p1] += count[p] % mod;
				newCount[p1] %= mod;
				newCount[p2] += count[p] % mod;
				newCount[p2] %= mod;
			}
		}
		
		count = newCount;
	}


	// End:

	ul out = 0;

	for (ul i = 0; i < sizeCount; i ++) {
		out += count[i];
		out %= mod;
	}

	return out;
}


int main () {
	ushort n, m;
	cin >> n >> m;
	//ofstream fout("out2.txt");

	//for (int i = 1; i < 300; i ++)
		//for (int j = 1; j*i <= 300; j ++)
			cout << solve(n, m) << endl;

	return 0;
}
