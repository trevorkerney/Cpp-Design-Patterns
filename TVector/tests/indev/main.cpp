#include "../../inc/TVector.h"

#include <iostream>
#include <vector>

int main()
{
    TVector<int> tv;

    tv.reserve(5);

    tv.push(4);
    tv.push(7);
    tv.push(52);

    int num = tv.erase(0);

    tv.insert(11, 1);
    tv.insert(32, 0);

    int num2 = tv.pull();

    tv.shrink();

    for (int _i = 0; _i < tv.size(); _i++)
    {
        std::cout << tv[_i] << std::endl;
    }
    std::cout << "-----------" << std::endl;

    TVector<int> tv2;
    tv2 = tv;

    tv.pull();
    tv.pull();
    tv.pull();

    for (int _i = 0; _i < tv.size(); _i++)
    {
        std::cout << tv[_i] << std::endl;
    }
    std::cout << "-----------" << std::endl;

    for (int _i = 0; _i < tv2.size(); _i++)
    {
        std::cout << tv2[_i] << std::endl;
    }
    std::cout << "-----------" << std::endl;

    std::cout << "size: " << tv.size() << std::endl;
}
