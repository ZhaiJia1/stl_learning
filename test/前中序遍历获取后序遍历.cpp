#include<iostream>
using namespace std;

void dfs(string pre, string in) {
	if (pre.empty()) return;
	char root = pre[0];
	int k = in.find(root);
	dfs(pre.substr(1, k), in.substr(0, k));
	dfs(pre.substr(k + 1), in.substr(k + 1));
	cout << root;
}

//int main() {
//	string pre, in;
//	while (cin >> pre >> in) {
//		dfs(pre, in);
//		cout << endl;
//	}
//	return 0;
//}
