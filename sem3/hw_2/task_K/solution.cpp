#include <iostream>
#include <string>
#include <vector>

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

bool isSubstr (const string &txt, const string &temp) {
	vector<unsigned> prefixArray = prefixArrayFromString(temp + "#" + txt);

	for (unsigned i = 0; i < txt.size(); i ++)
		if (prefixArray[temp.size() + 1 + i] == temp.size()) 
			return true;

	return false;
}

string findAnswer (const string &str) {
	vector<unsigned> prefixArray = prefixArrayFromString(str);
	return str.substr(0, str.size() - prefixArray[str.size() - 1]);
}

string compressTwoStrings (const string &str1, const string &str2) {
	unsigned size = 0;

	string str = str2 + char(0) + str1;
	size = prefixArrayFromString(str)[str.size() - 1];

	return str1.substr(0, str1.size() - size) + str2;
}


int main () {
	string str1, str2;
	cin >> str1 >> str2;

	if (!(str1.size() >= str2.size()))
		swap(str1, str2);

	if (isSubstr(str1, str2))
		cout << findAnswer(str1) << endl;

	else {
		string comp1 = findAnswer(compressTwoStrings(str1, str2));
		string comp2 = findAnswer(compressTwoStrings(str2, str1));
		cout << (comp1.size() < comp2.size() ? comp1 : comp2) << endl;
	}

	return 0;
}