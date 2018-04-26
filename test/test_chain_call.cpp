#include <iostream>
#include "sf_chain.h"


using namespace skyfire;
using namespace std;

int sum(int a, int b)
{
    cout<<a<<" "<<b<<endl;
    return a+b;
}

int append(int a)
{
    cout<<a<<endl;
    return a+1;
}

void append_n(int a, int n)
{
    cout<<"append "<<n<<endl;
}


int main()
{
    auto p = make_chain_call(sum, 1, 1).then(append).then(append).then(append_n, 25);
    p.call();

    make_chain_async_call(sum,1,1).then(append).then(append).then(append_n, 25);

    cout<<"hello world"<<endl;
}