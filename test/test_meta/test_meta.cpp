#include "sf_meta.h"
#include <typeinfo>
#include <iostream>

using namespace std;
using namespace skyfire;

int func(const int&)
{
    int a;
    return a;
}

int main()
{
    using ret = typename sf_function_type_helper<decltype(function(func))>::return_type ;
    using param = typename sf_function_type_helper<decltype(function(func))>::param_type ;

    cout<< typeid(ret).name()<<endl;
    cout<< typeid(param).name()<<endl;

}