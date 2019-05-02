#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


struct Data {
	short l, r;

	Data () {
		l = r = -1;
	}
};


bool solveForSubsegments (const vector<ushort> &suffixes, const ushort length) {
	if (length == 0)
		return false;

	else if (length == 1)
		return suffixes.size() > 1;

	Data data;

	for (const ushort &suffix: suffixes) {
		const ushort l = suffix;
		const ushort r = suffix + length - 1;

		if (data.l >= 0 && data.r >= 0) {
			if (data.l < 2*l - r || data.r > 2*r - l)
				return true;

			else {
				data.r = max<short>(data.r, r);
				data.l = min<short>(data.l, l);
			}
		}

		else {
			data.l = l;
			data.r = r;
		}
	}

	return false;
}


unsigned dfs (char *str, const ushort &stringLength, const vector<ushort> &suffixes, const ushort &length = 0) {
	vector<vector<ushort> > nextLists(26);

	for (const ushort &suffix: suffixes) {
		if (suffix + length >= stringLength)
			continue;

		else
			nextLists[str[suffix + length] - 'a'].push_back(suffix);
	}

	unsigned count = solveForSubsegments(suffixes, length);

	for (const vector<ushort> &nextList: nextLists)
		if (nextList.size() > 1)
			count += dfs(str, stringLength, nextList, length + 1);

	return count;
}


int main () {
	char *str = new char[100000];
	scanf("%s", str);
	ushort stringLength = strlen(str);

	vector<ushort> allSuffixes;

	for (unsigned i = 0; i < stringLength; i ++)
		allSuffixes.push_back(i);	

	printf("%u\n", dfs(str, stringLength, allSuffixes));

	return 0;
}
