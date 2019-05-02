#include <ext/pb_ds/assoc_container.hpp> // Общий файл. 
#include <ext/pb_ds/tree_policy.hpp> // Содержит класс tree_order_statistics_node_update
#include <iostream>

using namespace std;
using namespace __gnu_pbds;

int main () {
	unsigned count;
	cin >> count;

	tree<string, null_type, less<string>, rb_tree_tag, tree_order_statistics_node_update> myCoolStruct;
	
	while (true) {
		string str;

		if (!(cin >> str))
			break;

		if (str[0] >= '0' && str[0] <= '9')
			cout << *myCoolStruct.find_by_order(stoi(str) - 1) << endl;

		else
			myCoolStruct.insert(str);
	}

	return 0;
}
