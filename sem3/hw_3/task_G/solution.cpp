#include <iostream>
#include <unordered_map>
#include <limits>
#include <string>
#include <vector>

using namespace std;


vector<unsigned> buildCyclicSuffixArray (const string &str, const vector<char> &alphabet) {	
	vector<unsigned> permutation(str.size(), 0);
	vector<unsigned> classes(str.size(), 0);


	// Запоминаем индексы каждого символа в алфавитном массиве.
	unordered_map<char, unsigned> symbolIndexInAlphabet;

	for (unsigned i = 0; i < alphabet.size(); i ++)
		symbolIndexInAlphabet[alphabet[i]] = i;

	// Считаем кол-во каждого символа в индексации алфавитного вектора.
	vector<unsigned> symbolCount(alphabet.size(), 0);

	for (unsigned i = 0; i < str.size(); i ++)
		symbolCount[symbolIndexInAlphabet[str[i]]] ++;

	// Считаем кол-во символов, индекс которых (в афавите) меньше или равен i.
	for (unsigned i = 1; i < alphabet.size(); i ++)
		symbolCount[i] += symbolCount[i - 1];

	// Находим требуемую перестановку символов.
	for (unsigned i = 0; i < str.size(); i ++)
		permutation[-- symbolCount[symbolIndexInAlphabet[str[i]]]] = i;


	// Вычисляем принадлежность символов их классам эквивалентности.
	unsigned classesCount = 1;

	for (unsigned i = 1; i < str.size(); i ++) {
		if (str[permutation[i]] != str[permutation[i - 1]])
			classesCount ++;

		classes[permutation[i]] = classesCount - 1;
	}


	// Основные O(logN) фазы алгоритма.
	for (unsigned k = 0; (1 << k) < str.size(); k ++) {
		// Подготовка - вычисление перестановки в порядке сортировки вторых половин новых строк.
		vector<unsigned> secondHalfPermutation(str.size(), 0);

		for (unsigned i = 0; i < str.size(); i ++) {
			if (permutation[i] < (1 << k))
				secondHalfPermutation[i] = str.size() + permutation[i] - (1 << k);

			else
				secondHalfPermutation[i] = permutation[i] - (1 << k);
		}


		// Сортировка подсчетом и вычисление перестановки.
		vector<unsigned> count(str.size());

		for (unsigned i = 0; i < str.size(); i ++)
			count[classes[secondHalfPermutation[i]]] ++;

		for (unsigned i = 1; i < str.size(); i ++)
			count[i] += count[i - 1];

		for (int i = str.size() - 1; i >= 0; i --)
			permutation[-- count[classes[secondHalfPermutation[i]]]] = secondHalfPermutation[i];


		// Вычисление новых классов эквивалентности.
		classesCount = 1;
		vector<unsigned> newClasses(str.size(), 0);

		for (unsigned i = 1; i < str.size(); i ++) {
			unsigned firstHalf1 = (permutation[i] + (1 << k)) % str.size();
			unsigned firstHalf2 = (permutation[i - 1] + (1 << k)) % str.size();

			if (classes[permutation[i]] != classes[permutation[i - 1]] || classes[firstHalf1] != classes[firstHalf2])
				classesCount ++;

			newClasses[permutation[i]] = classesCount - 1;
		}

		classes = newClasses;
	}


	return permutation;
}

vector<unsigned> buildSuffixArray (const string &str, const vector<char> &alphabet) {
	string cyclicString = str + '!';
	vector<char> newAlphabet(alphabet.size() + 1);

	newAlphabet[0] = '!';

	for (unsigned i = 0; i < alphabet.size(); i ++)
		newAlphabet[i + 1] = alphabet[i];

	vector<unsigned> cyclicSuffixArray = buildCyclicSuffixArray(cyclicString, newAlphabet);
	vector<unsigned> suffixArray;

	for (unsigned el: cyclicSuffixArray)
		if (el != cyclicString.size() - 1)
			suffixArray.push_back(el);

	return suffixArray;
}

vector<unsigned> buildLCP (const string &str, const vector<unsigned> &suffixArray) {
	vector<unsigned> lcp(str.size(), numeric_limits<unsigned>::max());
	vector<unsigned> suffixPositionInSuffixArray(str.size());

	for (unsigned i = 0; i < str.size(); i ++)
		suffixPositionInSuffixArray[suffixArray[i]] = i;

	unsigned prefixMatchLength = 0;

	for (unsigned currentSuffix = 0; currentSuffix < str.size(); currentSuffix ++) {
		if (prefixMatchLength > 0)
			prefixMatchLength --;

		if (suffixPositionInSuffixArray[currentSuffix] == str.size() - 1)
			prefixMatchLength = 0;

		else {
			unsigned nextSuffix = suffixArray[suffixPositionInSuffixArray[currentSuffix] + 1];
			
			while (max<unsigned>(currentSuffix, nextSuffix) + prefixMatchLength < str.size() && str[currentSuffix + prefixMatchLength] == str[nextSuffix + prefixMatchLength])
				prefixMatchLength ++;

			lcp[suffixPositionInSuffixArray[currentSuffix]] = prefixMatchLength;
		}

	}

	return lcp;
}

 
string getKthSubstring (const string &str1, const string &str2, const unsigned long long &k) {
	string str = str1 + '#' + str2 + '$';
	vector<char> alphabet = {'#', '$', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
	vector<unsigned> suffixArray = buildSuffixArray(str, alphabet);
	vector<unsigned> lcp = buildLCP(str, suffixArray);

	/*for (const unsigned &el: suffixArray)
		cout << el << " ";
	cout << endl;

	for (const unsigned &el: lcp)
		cout << el << " ";
	cout << endl;*/

	unsigned long long prev = 0, count = 0;
	
	for (unsigned long long i = 2; i < str1.size() + str2.size() + 1; i ++) {
		bool flag1 = (suffixArray[i] < str1.size() + 1);
		bool flag2 = (suffixArray[i + 1] < str1.size() + 1);
		
		if (flag1 != flag2) {
			if (lcp[i] > prev)
				count += lcp[i] - prev;

			if (count >= k)
				return str.substr(suffixArray[i], k - count + lcp[i]);

			prev = lcp[i];
		}
		
		else
			prev = min<unsigned>(prev, lcp[i]);
	}

	return "-1";
}
 
int main() {
	string str1, str2;
	unsigned long long k;
	cin >> str1 >> str2 >> k;
	cout << getKthSubstring(str1, str2, k) << endl;
	return 0;
}