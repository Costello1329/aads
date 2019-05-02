#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <climits>

typedef unsigned long long ull;
typedef std::vector<std::vector<ull> > Matrix;
using namespace std;


class BigInt {
	private:
		static const size_t maxSize = 20000;
		size_t numSize;
		bool negative;
		char *num;

	public:
		// Constructors:

		explicit BigInt () {
			numSize = 1;
			negative = false;
			num = new char[maxSize];
			num[0] = '0';
		}

		~BigInt () {
			delete num;
			num = nullptr;
		}


		// Copy constructors:

		BigInt (const BigInt &other) {
			num = new char[maxSize];

			negative = other.negative;
			numSize = other.numSize;
			
			for (size_t i = 0; i < numSize; i ++)
				num[i] = other.num[i];
		}

		BigInt (const int &other) {
			num = new char[maxSize];	

			int nonConstOther;

			if (other < 0) {
				negative = true;
				nonConstOther = - other;
			}

			else if (other > 0) {
				negative = false;
				nonConstOther = other;
			}

			else {
				negative = false;
				numSize = 1;
				num[0] = '0';
				return;
			}

			numSize = 0;

			while (nonConstOther) {

				num[numSize ++] = '0' + nonConstOther % 10;
				nonConstOther /= 10;
			}
		}

		BigInt (const string &other) {
			num = new char[maxSize];
			negative = (other[0] == '-');
			numSize = other.size() - negative;

			for (size_t i = 0; i < numSize; i ++)
				num[numSize - 1 - i] = other[i + negative];
		}


		// Copy operators:

		BigInt& operator = (const BigInt &other) {
			negative = other.negative;
			numSize = other.numSize;
			
			for (size_t i = 0; i < numSize; i ++)
				num[i] = other.num[i];

			return *this;
		}

		BigInt& operator = (const int &other) {
			return *this = BigInt(other);
		}

		BigInt& operator = (const string &other) {
			return *this = BigInt(other);
		}


		// Conversion functions:

		explicit operator int() const {
			int out = 0;

			for (size_t i = 0; i < numSize; i ++)
				out += (num[i] - '0') * pow(10, i);

			if (negative)
				out = - out;

			return out;
		}

		explicit operator string() const {
			string out = "";

			if (negative)
				out += '-';

			for (size_t i = 0; i < numSize; i ++)
				out += num[numSize - 1 - i];

			return out;
		}


		// Operators:
		friend bool operator < (const BigInt&, const BigInt&);
		friend bool operator > (const BigInt&, const BigInt&);
		friend bool operator <= (const BigInt&, const BigInt&);
		friend bool operator >= (const BigInt&, const BigInt&);
		friend bool operator == (const BigInt&, const BigInt&);
		friend bool operator != (const BigInt&, const BigInt&);
		friend BigInt operator + (const BigInt&);
		friend BigInt operator - (const BigInt&);
		friend BigInt operator + (const BigInt&, const BigInt&);
		BigInt& operator += (const BigInt&);
		friend BigInt operator - (const BigInt&, const BigInt&);
		BigInt& operator -= (const BigInt&);
		friend BigInt operator * (const BigInt&, const BigInt&);
		BigInt& operator *= (const BigInt&);
		friend BigInt operator / (const BigInt&, const BigInt&);
		BigInt& operator /= (const BigInt&);
		friend BigInt operator % (const BigInt&, const BigInt&);
		BigInt& operator %= (const BigInt&);
		friend istream& operator >> (istream&, BigInt&);
		friend ostream& operator << (ostream&, const BigInt&);
};


bool operator < (const BigInt &one, const BigInt &other) {
	if (one.negative && other.negative)
		return -other < -one;

	else if (one.negative && !other.negative)
		return true;

	else if (!one.negative && other.negative)
		return false;

	if (one.numSize != other.numSize)
		return one.numSize < other.numSize;

	for (size_t i = 0; i < one.numSize; i ++) {
		ushort oneNum = one.num[one.numSize - 1 - i] - '0';
		ushort otherNum = other.num[one.numSize - 1 - i] - '0';

		if (oneNum != otherNum)
			return oneNum < otherNum;
	}

	return false;
}

bool operator > (const BigInt &one, const BigInt &other) {
	return other < one;
}

bool operator <= (const BigInt &one, const BigInt &other) {
	return !(one > other);
}

bool operator >= (const BigInt &one, const BigInt &other) {
	return !(one < other);
}

bool operator == (const BigInt &one, const BigInt &other) {
	return one <= other && one >= other;
}

bool operator != (const BigInt &one, const BigInt &other) {
	return !(one == other);
}


BigInt operator + (const BigInt &one) {
	return one;
}


BigInt operator - (const BigInt &one) {
	BigInt newBigInt(one);
	newBigInt.negative ^= true;
	return newBigInt;
}


BigInt operator + (const BigInt &one, const BigInt &other) {
	if (one.negative && other.negative)
		return -one + -other;

	else if (one.negative && !other.negative)
		return other - -one;

	else if (!one.negative && other.negative)
		return one - -other;

	BigInt out;
	out.numSize = one.numSize > other.numSize ? one.numSize : other.numSize;

	ushort delta = 0;

	for (size_t i = 0; i < out.numSize; i ++) {
		ushort oneNum = (i < one.numSize ? one.num[i] - '0' : 0);
		ushort otherNum = (i < other.numSize ? other.num[i] - '0' : 0);
		ushort sum = oneNum + otherNum + delta;
		out.num[i] = (sum % 10) + '0';
		delta = sum / 10;
	}

	if (delta != 0) {
		out.num[out.numSize] = delta + '0';
		out.numSize ++;
	}

	return out;
}

BigInt& BigInt::operator += (const BigInt &other) {
	return *this = *this + other;
}


BigInt operator - (const BigInt &one, const BigInt &other) {
	if (one.negative && other.negative)
		return -one - -other;

	else if (one.negative && !other.negative)
		return -(-one + other);

	else if (!one.negative && other.negative)
		return -(one + -other);

	if (one < other)
		return -(other - one);

	BigInt out;
	out.numSize = one.numSize > other.numSize ? one.numSize : other.numSize;

	ushort delta = 0;

	for (size_t i = 0; i < out.numSize; i ++) {
		ushort oneNum = (i < one.numSize ? one.num[i] - '0' : 0);
		ushort otherNum = (i < other.numSize ? other.num[i] - '0' : 0);
		short difference = oneNum - otherNum - delta;
		delta = 0;

		if (difference < 0) {
			delta = 1;
			difference += 10;
		}

		out.num[i] = (difference % 10) + '0';
	}

	size_t zeroCount;
	for (zeroCount = 0; zeroCount < out.numSize && out.num[out.numSize - 1 - zeroCount] == '0' && zeroCount < out.numSize - 1; zeroCount ++);
	out.numSize -= zeroCount;

	return out;
}

BigInt& BigInt::operator -= (const BigInt &other) {
	return *this = *this - other;
}


BigInt operator * (const BigInt &one, const BigInt &other) {
	if (one.negative && other.negative)
		return -one * -other;
	
	else if (one.negative && !other.negative)
		return - (-one * other);
	
	else if (!one.negative && other.negative)
		return - (one * - other);
	
	if (one < other)
		return other * one;

	BigInt out;

	for (size_t i = 0; i < other.numSize; i ++) {
		ushort delta = 0;
		BigInt current;
		current.numSize = one.numSize;

		for (size_t j = 0; j < one.numSize; j ++) {
			ushort oneNum = one.num[j] - '0';
			ushort otherNum = other.num[i] - '0';
			ushort multiplication = oneNum * otherNum + delta;
			current.num[j] = multiplication % 10 + '0';
			delta = multiplication / 10;
		}

		if (delta > 0)
			current.num[current.numSize ++] = delta + '0';

		string currentWithAddedDigits = (string)current;

		for (size_t z = 0; z < i; z ++)
			currentWithAddedDigits += "0";

		out += current = currentWithAddedDigits;
	}

	size_t zeroCount;
	for (zeroCount = 0; zeroCount < out.numSize && out.num[out.numSize - 1 - zeroCount] == '0' && zeroCount < out.numSize - 1; zeroCount ++);
	out.numSize -= zeroCount;

	return out;
}

BigInt& BigInt::operator *= (const BigInt &other) {
	return *this = *this * other;
}


BigInt operator / (const BigInt &one, const BigInt &other) {
	if (one.negative && other.negative)
		return -one / -other;
	
	else if (one.negative && !other.negative)
		return - (-one / other);
	
	else if (!one.negative && other.negative)
		return - (one / - other);
	
	if (one < other)
		return BigInt(0);

	BigInt out, currentSlice;
	out.numSize = one.numSize;

	for (size_t i = 0; i < one.numSize; i ++) {
		ushort oneNum = one.num[one.numSize - 1 - i] - '0';
		currentSlice *= 10;
		currentSlice += oneNum;

		if (currentSlice < other) {
			out.num[one.numSize - 1 - i] = '0';
			continue;
		}

		BigInt edge = other;

		size_t digit = 1;
		for (digit = 1; digit <= 9; digit ++, edge += other)
			if (edge > currentSlice)
				break;

		digit -= 1;
		edge -= other;
		currentSlice -= edge;
		out.num[one.numSize - 1 - i] = digit + '0';
	}

	size_t zeroCount;
	for (zeroCount = 0; zeroCount < out.numSize && out.num[out.numSize - 1 - zeroCount] == '0' && zeroCount < out.numSize - 1; zeroCount ++);
	out.numSize -= zeroCount;

	return out;
}

BigInt& BigInt::operator /= (const BigInt &other) {
	return *this = *this / other;
}


BigInt operator % (const BigInt &one, const BigInt &other) {
	if (one < 0 || other < 0 || other == 0)
		return BigInt(-1);

	BigInt p = one / other;
	return one - p * other;
}

BigInt& BigInt::operator %= (const BigInt &other) {
	return *this = *this % other;
}


istream& operator >> (istream &is, BigInt &bigInt) {
	string str;
	is >> str;
	BigInt newBigInt(str);
	bigInt = newBigInt;
	return is;
}

ostream& operator << (ostream &os, const BigInt &bigInt) {
	return os << (string)bigInt;
}


template <typename Type>
ostream& operator << (ostream& os, const vector<Type> &vec) {
	for (Type el: vec)
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

	for (ull y = 0; y < out.size(); y ++)
		for (ull x = 0; x < out[0].size(); x ++)
			for (ull i = 0; i < length; i ++)
				(out[y][x] += (matrix1[y][i] * matrix2[i][x]) % MOD) %= MOD;

	return out;
}

Matrix binPow (const Matrix &matrix, BigInt pow, ull MOD) {
	if (pow == 0) {
		Matrix out(matrix.size(), vector<ull>(matrix.size(), 0));

		for (int i = 0; i < out.size(); i ++)
			out[i][i] = 1;

		return out;
	}

	if (pow % 2 == 0) {
		Matrix square = multiply(matrix, matrix, MOD);
		return binPow(square, pow / 2, MOD);
	}

	else {
		Matrix power = binPow(matrix, pow - 1, MOD);
		return multiply(matrix, power, MOD);
	}
}


bool checkProfiles (ull first, ull second, ull size) {
	vector<bool> binaryFirst(size, false);
	vector<bool> binarySecond(size, false);

	for (ull i = 0; i < size; i ++) {
		binaryFirst[size - i - 1] = first % 2;
		first /= 2;
	}

	for (ull i = 0; i < size; i ++) {
		binarySecond[size - i - 1] = second % 2;
		second /= 2;
	}

	for (ull i = 0; i < size - 1; i ++)
		if (binaryFirst[i] == binaryFirst[i + 1])
			if (binaryFirst[i] == binarySecond[i])
				if (binaryFirst[i] == binarySecond[i + 1])
					return false;

	return true;
}


int main () {
	BigInt n, m;
	ushort mod;
	cin >> n >> m >> mod;


	Matrix admissible(exp2(int(m)), vector<ull>(exp2(int(m))));

	for (ull i = 0; i < exp2(int(m)); i ++)
		for (ull j = i; j < exp2(int(m)); j ++)
			admissible[i][j] = admissible[j][i] = checkProfiles(i, j, int(m));


	Matrix count(1, vector<ull>(exp2(int(m)), 1));
	Matrix power = binPow(admissible, n - 1, mod);
	count = multiply(count, power, mod);


	ull answer = 0;

	for (ull i = 0; i < exp2(int(m)); i ++) {
		answer += (count[0][i] % mod);
		answer %= mod;
	}

	cout << answer << endl;

	return 0;
}
