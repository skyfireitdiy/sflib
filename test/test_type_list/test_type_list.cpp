#include "sf_type_list.h"
#include <iostream>
#include <typeinfo>
#include <string>

using namespace skyfire;

template <typename  T>
void print_type()
{
    std::cout<< typeid(T).name()<<std::endl;
}

int main()
{
    using t1 = sf_type_list<int,double,char,std::string>;
    static_assert(std::is_same_v<sf_type_list<double,int,double ,char ,std::string>, typename tl_push_front<double, t1>::type >);
    static_assert(std::is_same_v<sf_type_list<int,double ,char,std::string,int>, typename tl_push_back<int, t1>::type >);
    static_assert(std::is_same_v<double ,typename tl_get<1, t1>::type>);
    static_assert(std::is_same_v<int ,typename tl_front<t1>::type>);
    static_assert(std::is_same_v<std::string ,typename tl_back<t1>::type>);
    static_assert(std::is_same_v<sf_type_list<int,double>, tl_left<2, t1>::type>);
    static_assert(std::is_same_v<sf_type_list<char,std::string>, tl_right<2, t1>::type>);
    static_assert(std::is_same_v<sf_type_list<double ,char,std::string>, tl_pop_front<t1>::type>);
    static_assert(std::is_same_v<sf_type_list<int, double ,char>, tl_pop_back<t1>::type>);
    static_assert(std::is_same_v<sf_type_list<char,double,int,double ,char,std::string>,typename tl_merge<sf_type_list<char,double>,t1>::type>);
    static_assert(std::is_same_v<sf_type_list<>, typename tl_erase<0,4,t1>::type>);
    static_assert(std::is_same_v<sf_type_list<double ,char>, typename tl_mid<1,2,t1>::type>);
    static_assert(std::is_same_v<sf_type_list<int,double,char,char,std::string>, typename tl_insert<2,char, t1>::type >);
    static_assert(std::is_same_v<sf_type_list<int,double,short,std::string>, typename tl_replace<char,short, t1>::type >);
    static_assert(std::is_same_v<sf_type_list<int,double,short,std::string>, typename tl_replace_index<2, short, t1>::type >);
    static_assert(std::is_same_v<sf_type_list<std::string, char, double , int>,typename tl_reverse<t1>::type >);
    static_assert(std::is_same_v<sf_type_list<std::string,int,char>, typename tl_from_tuple<std::tuple<std::string, int, char>>::type>);
}