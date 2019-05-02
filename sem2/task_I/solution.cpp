#include <iostream>
#include <vector>
#include <cmath>

typedef long long ll;
using namespace std;

const ll lim = 1000000000 + 7;


ll binpow(ll num, ll pow, ll mod) {
	ll res = 1;

	while (pow) {
		if (!(pow % 2)) {
			num *= num;
			num %= mod;
			pow /= 2;
		}

		else {
			res *= num;
			res %= mod;
			pow --;
		}
	}

	return res % mod;
}


int main () {
	int n;
	cin >> n;

	vector<vector<ll> > C;

	for (int i = 0; i <= n; i ++) {
		C.push_back(vector<ll>(i + 1));
		C[i][0] = 1;
		C[i][i] = 1;
	}

	for (int i = 2; i <= n; i ++)
		for (int j = 1; j < C[i].size() - 1; j ++)
			C[i][j] = (C[i - 1][j - 1] + C[i - 1][j]) % lim;

	vector<ll> connected(n + 1);
	vector<ll> G(n + 1);
	G[0] = connected[0] = 0;
	G[1] = connected[1] = 1;

	for (int i = 2; i <= n; i ++)
		G[i] = binpow(2, C[i][2], lim);

	for (int i = 2; i <= n; i ++) {
		ll disconnected = 0;
	
		for (int k = 1; k <= i - 1; k ++)
			disconnected += (((C[i - 1][k - 1] * connected[k]) % lim) * G[i - k]) % lim;
	
		connected[i] = (G[i] - disconnected % lim + lim) % lim;
	}

	cout << connected[n] << endl;
}
