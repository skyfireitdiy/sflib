#include "sf_meta.hpp"
#include <typeinfo>
#include <iostream>

using namespace skyfire;

int func(const int&)
{
    int a;
    return a;
}

int main()
{
    using ret = typename sf_function_type_helper<decltype(std::function(func))>::return_type ;
    using param = typename sf_function_type_helper<decltype(std::function(func))>::param_type ;

    std::cout<< typeid(ret).name()<<std::endl;
    std::cout<< typeid(param).name()<<std::endl;

}