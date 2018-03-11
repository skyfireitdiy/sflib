#include "sf_rpcclient.h"
#include <iostream>


using namespace std;
using namespace skyfire;


class rpcclient : public sf_rpcclient<>, public std::enable_shared_from_this<rpcclient>
{
public:
#include "rpc_declare.h"
};

int main()
{
    auto client = make_shared<rpcclient>();
    if(!client->connect("127.0.0.1",1234))
    {
        cout<<"connect error"<<endl;
        return -1;
    }
    cout<<client->add(5,6)<<endl;
    this_thread::sleep_for(std::chrono::seconds(5));
    cout<<client->add_welcome(string("skyfire"))<<endl;
    this_thread::sleep_for(std::chrono::seconds(5));
    system("pause");
}
