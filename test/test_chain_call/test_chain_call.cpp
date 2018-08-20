#include <iostream>
#include "sf_chain.h"


using namespace skyfire;

int sum(int a, int b)
{
    std::cout<<a<<" "<<b<<std::endl;
    return a+b;
}

int append(int a)
{
    std::cout<<a<<std::endl;
    return a+1;
}

void append_n(int a, int n)
{
    std::cout<<"append "<<n<<std::endl;
}


int main()
{
    auto p = make_chain_call(sum, 1, 1).then(append).then(append).then(append_n, 25);
    p.call();

    make_chain_async_call(sum,1,1).then(append).then(append).then(append_n, 25);

    std::cout<<"hello world"<<std::endl;
}