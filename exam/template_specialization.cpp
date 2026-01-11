#include <iostream>

template<typename T>
struct Printer {
    void print(T value) { std::cout << value << "\n"; }
};

template<>
struct Printer<const char*> {
    void print(const char* value) { std::cout << "String: " << value << "\n"; }
};

int main() {
    Printer<int> p1;
    p1.print(42);

    Printer<const char*> p2;
    p2.print("Hello"); 
}
