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


Matrix multiply (const Matrix &matrix1, const Matrix &matrix2, ull MOD) {
	Matrix out(matrix1.size(), vector<ull>(matrix2[0].size(), 0));
	ull length = matrix2.size();

	for (ull y = 0; y < out.size(); y ++) {
		for (ull x = 0; x < out[0].size(); x ++) {
			ull sum = 0;

			for (ull i = 0; i < length; i ++) {
				sum += (matrix1[y][i] * matrix2[i][x]) % MOD;
				sum %= MOD;
			}

			out[y][x] = sum;
		}
	}

	return out;
}

Matrix pow (const Matrix &matrix, ull n, ull MOD) {
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
	ull T;
	cin >> T;

	for (ull t = 0; t < T; t ++) {
		ull n, l, r, MOD;

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


		Matrix power = pow(matrix, n - 1, MOD);

		Matrix col(10, vector<ull>(1, 1));
		col[0][0] = 0;

		Matrix res = multiply(power, col, MOD);


		ull answer = 0;

		for (ull i = 0; i <= 9; i ++) {
			answer += res[i][0];
			answer %= MOD;
		}

		cout << answer << endl;
	}

	return 0;
}
