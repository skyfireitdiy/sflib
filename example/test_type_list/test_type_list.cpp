
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_type_list.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include "tools/sf_type_list.h"
using namespace skyfire;

template <typename  T>
void print_type()
{
    std::cout<< typeid(T).name()<<std::endl;
}

int main()
{
    using t1 = sf_type_list<int,double,char,std::string>;
    static_assert(std::is_same_v<sf_type_list<double,int,double ,char ,std::string>, typename sf_tl_push_front<double, t1>::type >);
    static_assert(std::is_same_v<sf_type_list<int,double ,char,std::string,int>, typename sf_tl_push_back<int, t1>::type >);
    static_assert(std::is_same_v<double ,typename sf_tl_get<1, t1>::type>);
    static_assert(std::is_same_v<int ,typename sf_tl_front<t1>::type>);
    static_assert(std::is_same_v<std::string ,typename sf_tl_back<t1>::type>);
    static_assert(std::is_same_v<sf_type_list<int,double>, sf_tl_left<2, t1>::type>);
    static_assert(std::is_same_v<sf_type_list<char,std::string>, sf_tl_right<2, t1>::type>);
    static_assert(std::is_same_v<sf_type_list<double ,char,std::string>, sf_tl_pop_front<t1>::type>);
    static_assert(std::is_same_v<sf_type_list<int, double ,char>, sf_tl_pop_back<t1>::type>);
    static_assert(std::is_same_v<sf_type_list<char,double,int,double ,char,std::string>,typename sf_tl_merge<sf_type_list<char,double>,t1>::type>);
    static_assert(std::is_same_v<sf_type_list<>, typename sf_tl_erase<0,4,t1>::type>);
    static_assert(std::is_same_v<sf_type_list<double ,char>, typename sf_tl_mid<1,2,t1>::type>);
    static_assert(std::is_same_v<sf_type_list<int,double,char,char,std::string>, typename sf_tl_insert<2,char, t1>::type >);
    static_assert(std::is_same_v<sf_type_list<int,double,short,std::string>, typename sf_tl_replace<char,short, t1>::type >);
    static_assert(std::is_same_v<sf_type_list<int,double,short,std::string>, typename sf_tl_replace_index<2, short, t1>::type >);
    static_assert(std::is_same_v<sf_type_list<std::string, char, double , int>,typename sf_tl_reverse<t1>::type >);
    static_assert(std::is_same_v<sf_type_list<std::string,int,char>, typename sf_tl_from_tuple<std::tuple<std::string, int, char>>::type>);
}