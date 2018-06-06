#include "sf_any.h"
#include <vector>
#include <iostream>
#include <string>

using namespace skyfire;
using namespace std;

int main()
{
    vector<sf_any> any_v;
    any_v.push_back(5);
    any_v.push_back("hello world"s);
    any_v.push_back(6.1);

    cout << static_cast<int>(any_v[0]) << endl;
    cout << static_cast<string>(any_v[1]) << endl;
    cout << static_cast<double >(any_v[2]) << endl;

    any_v[0] = 689;
    any_v[1] = "1234"s;
    any_v[2] = 99;

}