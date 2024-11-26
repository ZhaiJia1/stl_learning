#pragma once
mystl::vector<int> test;
mystl::vector<int> test2;
/*cout << "here" << endl;
auto x = test.begin();
auto y = test.end();
std::cout << test.size() << std::endl;


test.push_back(5);
std::cout << *(test.end()-1) << std::endl;
test.emplace_back(6);
std::cout << *(test.end() - 1) << std::endl;*/
DWORD start_time1 = GetTickCount();

cout << test.size() << endl;

for (int i = 0; i < 500000000; ++i) {
	test.push_back(i);
}
std::cout << test.size() << std::endl;
DWORD end_time1 = GetTickCount();

cout << "push_back()10000times: " << (end_time1 - start_time1) << endl;
DWORD start_time2 = GetTickCount();
for (int i = 0; i < 500000000; ++i) {
	test2.emplace_back(i);
}
DWORD end_time2 = GetTickCount();

cout << "emplace_back()10000times: " << end_time2 - start_time2 << endl;