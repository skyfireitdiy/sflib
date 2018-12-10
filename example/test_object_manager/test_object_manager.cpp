#include "sf_object.hpp"

#include <vector>

using namespace skyfire;
using namespace std;

class work : public sf_object
{
private:
    sf_reg_class(work)
    sf_meta_value(string,name);
    sf_meta_value(int,time);
};

sf_class(work)

class student : public sf_object
{
private:
    sf_reg_class(student)
    sf_meta_value(string, name)
    sf_meta_value(int, age)
    sf_meta_ref(work,works)
    sf_meta_pointer(work,works2)
    sf_meta_container_value(vector,int,vi)
    sf_meta_container_ref(list, work, wlist)
    sf_meta_container_pointer(list, work, wlist2)
    sf_meta_associated_container_value_value(map,int,string,mis_value)
};

sf_class(student)


int main()
{
    // 1.创建一个manager
    sf_object_manager manager;
    // 2.加载配置文件
    auto ret = manager.load_config("/home/skyfire/CLionProjects/sflib/example/test_object_manager/config.json");

    if(!ret){
        cout<<"加载配置文件失败";
        return 0;
    }

    // 3. 获取my_work对象，类型是work
    auto wk = manager.get_object<work>("my_work");

    // 4.将my_work序列化为json
    cout<<wk->to_json()<<endl;

    // 5. 修改my_work的属性
    wk->set_name("skyfire");


    cout<<wk->to_json()<<endl;

    // 6. 再次获取my_work对象，此时获取到的与上一次一致
    wk = manager.get_object<work>("my_work");
    cout<<wk->to_json()<<endl;

    // 7. 获取student对象，此时student中的works属性与上面的my_work一致
    auto st = manager.get_object<student>("my_student");
    cout<<st->to_json()<<endl;

}
