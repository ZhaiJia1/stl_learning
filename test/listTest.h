#pragma once
mystl::list<int> x;
for (int i = 0; i < 20; ++i) {
    x.push_front(i);
}
for (auto a = x.begin(); a != x.end(); ++a) {
    cout << a.node->data << " " << endl;
}
x.sort();
for (auto a = x.begin(); a != x.end(); ++a) {
    cout << a.node->data << " " << endl;
}