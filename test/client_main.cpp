#include "sf_rpcclient.h"
#include <iostream>

using namespace std;
using namespace skyfire;

class rpcclient : public sf_rpcclient<>, public std::enable_shared_from_this<rpcclient>
{
public:
    SF_BEGIN_REG_RPC_FUNC
    SF_REG_RPC_FUNC("add",add,,int,int,int);
    SF_REG_RPC_FUNC("add_welcome",add_welcome,, std::string,std::string);
    SF_REG_RPC_FUNC("sort_vec",sort_vec,, vector<int>, vector<int>);
    SF_END_REG_RPC_FUNC
};

void disp_vec(const vector<int> &data)
{
    for(auto p: data)
    {
        cout<<p<<" "<<flush;
    }
    cout<<endl;
}

int main()
{
    auto client = make_shared<rpcclient>();
    if(!client->connect("127.0.0.1",1234))
    {
        cout<<"connect error"<<endl;
        return -1;
    }
    cout<<"5+6="<<client->add(5,6)<<endl;
    cout<<"9+15="<<client->add(9,15)<<endl;
    cout<<"skyfire->"<<client->add_welcome(string("skyfire"))<<endl;
    cout<<"hello world->"<<client->add_welcome(string("hello world"))<<endl;
    vector<int> vec{94,34,34,6,21,94,31,64,31,6,43,1,64,643,1,4,31,6,3,43,164,6,1};
    cout<<"old: "<<flush;
    disp_vec(vec);
    vec = client->sort_vec(vec);
    cout<<"new: "<<flush;
    disp_vec(vec);
    system("pause");
}
