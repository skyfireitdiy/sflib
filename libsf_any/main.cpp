#include "sf_any.h"
#include <iostream>
#include <vector>
#include <string>


using namespace skyfire;
using namespace std;


struct A
{
    int a;
    int b;
};

void func(int t,A u)
{
    cout<<t<<endl;
    cout<<u.a<<endl;
    cout<<u.b<<endl;
}

int main()
{
    vector<sf_any> arr;
    arr.push_back(5);
    arr.push_back(5.6);
    arr.push_back(A{59,9});

    func(arr[0],arr[2]);
}