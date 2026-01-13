#include <iostream>

#include "Array.h"

int main()
{
    Array<int> a;
    a.add(5);
    a.add(3);
    a.add(5398);

    a.remove(0);
    for (int i = 0; i < a.getSize(); i++) {
        std::cout << a[i] << std::endl;
    }

    return 0;
}