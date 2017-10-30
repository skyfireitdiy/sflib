#include "sf_serialize.h"
#include <iostream>


using namespace skyfire;
using namespace std;

struct myType
{
    int t;
    double u;
    char str[1024];
};

int main()
{
    int a{5};
    double b{9.6};
    float c{6.54f};
    string d{"hello world"};
    vector<string> e{"nihao","hahaha","the end"};
    tuple<string,double,myType> f{"113assd",6.87,myType{6,5.6,"yyyyyy"}};
    auto t=sf_serialize(a,b,c,d,e,f);
    int aa;
    double bb;
    float cc;
    string dd;
    vector<string> ee;
    tuple<string,double,myType> ff;
    size_t pos=0;
    pos=sf_deserialize(t,aa,pos);
    pos=sf_deserialize(t,bb,pos);
    pos=sf_deserialize(t,cc,pos);
    pos=sf_deserialize(t,dd,pos);
    pos=sf_deserialize(t,ee,pos);
    pos=sf_deserialize(t,ff,pos);
    cout<<aa<<endl;
    cout<<bb<<endl;
    cout<<cc<<endl;
    cout<<dd<<endl;
    for(auto p:ee)
    {
        cout<<p<<endl;
    }
    cout<<get<0>(ff)<<endl;
    cout<<get<1>(ff)<<endl;
    cout<<get<2>(ff).t<<endl;
    cout<<get<2>(ff).u<<endl;
    cout<<get<2>(ff).str<<endl;
    
}