#include <iostream>
#include <vector>
#include <list>

using namespace std;


void debug (vector<list<ushort> > &boxes) {
	for (ushort b = 0; b < boxes.size(); b ++) {
		for (auto i = boxes[b].begin(); i != boxes[b].end(); ++ i)
			cout << *i << " ";

		cout << endl;
	}
}

int main () {
	ushort N, T, B;
	cin >> N >> T >> B;

	vector<list<ushort> > boxes(B + 1);

	for (ushort b = 0; b < B; b ++) {
		ushort boxSize;
		cin >> boxSize;

		for (ushort i = 0; i < boxSize; i ++) {
			ushort elType;
			cin >> elType;
			boxes[b].push_back(elType);
		}
	}


	// DEBUG:
	//debug(boxes);

	ushort Q;
	cin >> Q;

	for (int q = 0; q < Q; q ++) {
		ushort command;
		cin >> command;

		if (command == 1) {
			ushort type, boxNum, ans = 0;
			cin >> type >> boxNum;
			boxNum --;

			for (list<ushort>::iterator i = boxes[boxNum].begin(); i != boxes[boxNum].end(); ++ i)
				if (*i == type)
					ans ++;

			cout << ans << endl;
		}

		else if (command == 2) {
			ushort type, boxNum, size, newBoxNum, count = 0;
			cin >> type >> boxNum >> size >> newBoxNum;
			boxNum --;
			newBoxNum --;

			list<ushort>::iterator iter = prev(boxes[boxNum].end());

			//cout << "YA PIDOR: ";

			for (ushort i = 0; i < size; i ++) {
				//cout << i << ", ";
				list<ushort>::iterator prevIter = std::prev(iter, 1);

				if (*iter == type) {
					count ++;
					boxes[boxNum].erase(iter);
				}

				iter = prevIter;
			}

			//cout << endl;

			for (ushort i = 0; i < count; i ++)
				boxes[newBoxNum].push_back(type);
		}

		//debug(boxes);
	}

	return 0;
}
