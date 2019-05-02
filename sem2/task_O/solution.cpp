#include <iostream>
#include <vector>

using namespace std;


vector<unsigned> getPrimeNumbers (unsigned maxPrimeNumber) {
	vector<bool> isPrime(maxPrimeNumber, true);

	for (size_t i = 2; i < maxPrimeNumber; i ++)
		if (isPrime[i])
			for (size_t j = i * i; j < maxPrimeNumber; j += i)
				isPrime[j] = false;

	vector<unsigned> primeNumbers(1, 1);

	for (size_t i = 2; i < maxPrimeNumber; i ++)
		if (isPrime[i])
			primeNumbers.push_back(i);

	return primeNumbers;
}

int main () {
	const unsigned maxPrimeNumber = 1000000;
	vector<unsigned> primeNumbers = getPrimeNumbers(maxPrimeNumber);

	unsigned long long number;
	cin >> number;

	unsigned xorSum = 0;
	bool isPrime = true;


	for (size_t i = 1; i < primeNumbers.size(); i ++) {
		while (number % primeNumbers[i] == 0) {
			isPrime = false;
			xorSum ^= i;
			number /= primeNumbers[i];
		}
	}

	if (isPrime && number > maxPrimeNumber)
		cout << "David" << endl;

	else
		cout << (xorSum ? "David" : "Vasya") << endl;

	return 0;
}