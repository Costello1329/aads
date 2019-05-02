#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

typedef unsigned long long ull;
typedef vector<vector<ull> > Matrix;


ostream& operator << (ostream& os, const vector<ull> &vec) {
	for (ull el: vec)
		os << el << " ";

	return os;  
}


ostream& operator << (ostream& os, const Matrix &matrix) {
	for (vector<ull> row: matrix) {
		for (ull cell: row)
			os << cell << " ";

		os << endl;
	}

	return os;  
}


Matrix multiply (const Matrix &matrix1, const Matrix &matrix2, int MOD) {
	Matrix out(matrix1.size(), vector<ull>(matrix2[0].size(), 0));
	int length = matrix2.size();

	for (int y = 0; y < out.size(); y ++) {
		for (int x = 0; x < out[0].size(); x ++) {
			ull sum = 0;

			for (int i = 0; i < length; i ++) {
				sum += matrix1[y][i] * matrix2[i][x];
				sum %= MOD;
			}

			out[y][x] = sum;
		}
	}

	return out;
}

Matrix pow (const Matrix &matrix, int n, int MOD) {
	if (n <= 1)
		return matrix;

	if (n % 2 == 0) {
		Matrix square = multiply(matrix, matrix, MOD);
		return pow(square, n / 2, MOD);
	}

	else {
		Matrix power = pow(matrix, n - 1, MOD);
		return multiply(matrix, power, MOD);
	}
}

int main () {
	int T;
	cin >> T;

	for (int t = 0; t < T; t ++) {
		int n, l, r, MOD;
		cin >> n >> l >> r >> MOD;

		if (n == 1) {
			cout << 10 % MOD << endl;
			continue;
		}

		Matrix matrix(10, vector<ull>(10));

		for (short num = 0; num <= 9; num ++) {
			for (short adm = 0; adm <= 9; adm ++) {
				if (abs(num - adm) < l || abs(num - adm) > r)
					matrix[num][adm] = 0;

				else
					matrix[num][adm] = 1;
			}
		}

		//cout << matrix << endl;

		Matrix power = pow(matrix, n - 1, MOD);
		//cout << power << endl;

		Matrix col(10, vector<ull>(1, 1));
		col[0][0] = 0;

		Matrix res = multiply(power, col, MOD);

		//cout << res << endl;

		ull answer = 0;

		for (int i = 0; i <= 9; i ++) {
			answer += res[i][0];
			answer %= MOD;
		}

		cout << answer << endl;
	}

	return 0;
}
