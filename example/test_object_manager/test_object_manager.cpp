#include "sf_object_manager.hpp"

using namespace skyfire;
using namespace std;

int main()
{
    sf_object_manager manager;
    auto ret = manager.load_config("/home/skyfire/CLionProjects/sflib/example/test_object_manager/config.json");
    cout<<ret<<endl;
}
