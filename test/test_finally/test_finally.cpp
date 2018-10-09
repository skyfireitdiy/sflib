#include "sf_finally.hpp"
#include <iostream>

using namespace skyfire;
using namespace std;

int main()
{
    int *p = new int;
    sf_finally del_p([&](){
        delete p;
        cout<<"Delete p"<<endl;
    });
    {
        int *q = new int;
        sf_finally del_q([&](){
            delete q;
            cout<<"Delete q"<<endl;
        });
    }
}