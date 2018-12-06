#define SF_DEBUG

#include "sf_object.hpp"

using namespace skyfire;
using namespace std;

class work : public sf_object
{
public:
    sf_member(string,name)
    sf_member(int,time)
};

sf_class(work)

//class student : public sf_object
//{
//    sf_member(string, name)
//    sf_member(int, age)
//    sf_member(work,works)
//};

//sf_class(student)


int main()
{
    sf_object_manager manager;
    auto ret = manager.load_config("/home/skyfire/code/sflib/example/test_object_manager/config.json");
    cout<<ret<<endl;

    auto wk = manager.get_object<work>("my_work");

    cout<<wk->name<<endl;
    cout<<wk->time<<endl;
}
