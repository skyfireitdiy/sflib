#include <iostream>
#include "sf_rpcclient.h"


using namespace std;
using namespace skyfire;

void disp_vec(vector<int> data)
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
    if (!client->connect("127.0.0.1", 10001))
    {
        cout << "connect error" << endl;
        return -1;
    }
    client->call<>("print"s);
    vector<int> data = {9,5,6,7,41,23,4,5,7};
    disp_vec(data);
    data = *client->call<vector<int>>("add_one"s, data);
    disp_vec(data);
    cout<<"---------"<<endl;
    client->async_call<vector<int>>("add_one"s, disp_vec, data);
    system("pause");
}
