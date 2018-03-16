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

void finished(int a)
{
    cout<<"finished"<<endl;
    cout<<a<<endl;
}

int main()
{
    auto client = make_shared<sf_rpcclient<>>();
    if (!client->connect("127.0.0.1", 1234))
    {
        cout << "connect error" << endl;
        return -1;
    }

    client->async_call<int>("print"s, finished, 500);
    system("pause");
}
