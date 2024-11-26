#include<iostream>
#include<vector>
using namespace std;

struct ListNode {
	int val;
	ListNode* next;
	ListNode():val(-1),next(nullptr){}
	ListNode(int value):val(value),next(nullptr){}
};

ListNode* vecToList(const vector<int>& nums) {
	ListNode* head = new ListNode(nums[0]);
	int pre = nums[0];
	ListNode* cur = head;
	for (int i = 1; i < nums.size(); ++i) {
		if (nums[i] != pre) {
			pre = nums[i];
			ListNode* node = new ListNode(nums[i]);
			cur->next = node;
			cur = node;
		}
	}
	return head;
}

//int main() {
//	int n;
//	while (cin >> n) {
//		if (n == 0) {
//			cout << "list is empty" << endl;
//		}
//		else {
//			int i;
//			vector<int> list;
//			while (n--) {
//				cin >> i;
//				list.emplace_back(i);
//			}
//			for (int i = 0; i < list.size()-1; ++i) {
//				cout << list[i] << " ";
//			}
//			cout << list[list.size() - 1] << endl;
//
//			ListNode* ans = vecToList(list);
//			ListNode* val = ans;
//			while (val->next) {
//				cout << val->val << " ";
//				val = val->next;
//			}
//			cout << val->val<<endl;
//		}
//	}
//	return 0;
//}