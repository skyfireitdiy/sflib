#include "sf_rpcclient.h"
#include <iostream>

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
    cout << "5+6=" << client->call<int>("add", 5, 6) << endl;
    cout << "9+15=" << client->call<int>("add", 9, 15) << endl;
    cout << "skyfire->" << client->call<string>("add_welcome", string("skyfire")) << endl;
    cout << "hello world->" << client->call<string>("add_welcome", string("hello world")) << endl;
    vector<int> vec{94, 34, 34, 6, 21, 94, 31, 64, 31, 6, 43, 1, 64, 643, 1, 4, 31, 6, 3, 43, 164, 6, 1};
    cout << "old: " << flush;
    disp_vec(vec);
    vec = client->call<vector<int>>("sort", vec);
    cout << "new: " << flush;
    disp_vec(vec);
    system("pause");
}
