#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

using namespace std;


vector<unsigned> prefixArrayFromString (const string &str) {
	vector<unsigned> prefixArray(str.size(), 0);

	for (unsigned i = 1; i < str.size(); i ++) {
		unsigned j = prefixArray[i - 1];

		while (j > 0 && str[i] != str[j])
			j = prefixArray[j - 1];

		if (str[i] == str[j])
			++ j;

		prefixArray[i] = j;
	}

	return prefixArray;
}

vector<unsigned> findSubstrWithPrefixFunction (const string &txt, const string &temp) {
	vector<unsigned> prefixArray = prefixArrayFromString(temp + "#" + txt);

	vector<unsigned> matches;

	for (unsigned i = 0; i < txt.size(); i ++)
		if (prefixArray[temp.size() + 1 + i] == temp.size()) 
			matches.push_back(i + 1 - temp.size());

	return matches;
}


int main () {
	string temp, txt;
	cin >> txt >> temp;
	vector<unsigned> matches = findSubstrWithPrefixFunction(txt, temp);

	cout << matches.size() << endl;

	for (const unsigned &match: matches)
		cout << (match + 1) << " ";

	cout << endl;

	return 0;
}
