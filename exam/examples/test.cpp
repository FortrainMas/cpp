#include<iostream>

void bar(int& x) {std::cout << "lvalue" << std::endl;}
void bar(int&& x) {std::cout << "rvalue" << std::endl;}

template<class T>
void foo(T&& x) {
    bar(std::forward<T>(x));
}


int main() {
    int x =0;
    foo(x);
    foo(0);
    return 0;
}
