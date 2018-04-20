#include "sf_type_list.h"
#include <iostream>
#include <typeinfo>
#include <string>

using namespace skyfire;
using namespace std;

template <typename  T>
void print_type()
{
    cout<< typeid(T).name()<<endl;
}

int main()
{
    using t1 = sf_type_list<int,double,char,string>;
    static_assert(is_same_v<sf_type_list<double,int,double ,char ,std::string>, typename tl_push_front<double, t1>::type >);
    static_assert(is_same_v<sf_type_list<int,double ,char,string,int>, typename tl_push_back<int, t1>::type >);
    static_assert(is_same_v<double ,typename tl_get<1, t1>::type>);
    static_assert(is_same_v<int ,typename tl_front<t1>::type>);
    static_assert(is_same_v<string ,typename tl_back<t1>::type>);
    static_assert(is_same_v<sf_type_list<int,double>, tl_left<2, t1>::type>);
    static_assert(is_same_v<sf_type_list<char,string>, tl_right<2, t1>::type>);
    static_assert(is_same_v<sf_type_list<double ,char,string>, tl_pop_front<t1>::type>);
    static_assert(is_same_v<sf_type_list<int, double ,char>, tl_pop_back<t1>::type>);
    static_assert(is_same_v<sf_type_list<char,double,int,double ,char,string>,typename tl_merge<sf_type_list<char,double>,t1>::type>);
    static_assert(is_same_v<sf_type_list<>, typename tl_erase<0,4,t1>::type>);
    static_assert(is_same_v<sf_type_list<double ,char>, typename tl_mid<1,2,t1>::type>);
    static_assert(is_same_v<sf_type_list<int,double,char,char,string>, typename tl_insert<2,char, t1>::type >);
    static_assert(is_same_v<sf_type_list<int,double,short,string>, typename tl_replace<char,short, t1>::type >);
    static_assert(is_same_v<sf_type_list<int,double,short,string>, typename tl_replace_index<2, short, t1>::type >);
    static_assert(is_same_v<sf_type_list<string, char, double , int>,typename tl_reverse<t1>::type >);
}