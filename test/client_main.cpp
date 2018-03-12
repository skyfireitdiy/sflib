#include <iostream>
#include "sf_rpcclient.h"


using namespace std;
using namespace skyfire;

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
    auto client = make_shared<sf_rpcclient<>>();
    if (!client->connect("127.0.0.1", 1234))
    {
        cout << "connect error" << endl;
        return -1;
    }
    vector<int> data = {1,5,7,56,4,6,556,65,452,185,0,81,631};
    disp_vec(data);
    client->async_call<vector<int>>("sort"s, disp_vec, data);
    cout<<"hello world"<<endl;
    system("pause");
}
