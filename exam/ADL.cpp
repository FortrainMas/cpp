#include <iostream>

namespace adl {
    struct adl_sruct {
        int x;
    };

    void foo(adl_sruct adl) { std::cout << adl.x << std::endl; }
}

int main() {
    adl::adl_sruct adl;
    adl.x = 10;
    foo(adl);

    return 0;
}
