#include "sf_object.hpp"
#include "sf_object_manager.hpp"
#include <vector>

using namespace std;

class work : public skyfire::sf_object
{
private:
    sf_reg_class(work)
    m_base(string,name);
    m_base(int,time);
    bool operator<(const work& other) const{
        return time<other.time;
    }
};

sf_class(work)

class student : public skyfire::sf_object
{
private:
    sf_reg_class(student)
    m_base(string, name)
    m_base(int, age)
    m_value(work,works)
    m_pointer(work,works2)
    mc_base(vector,int,vi)
    mc_value(list, work, wlist)
    mc_pointer(list, work, wlist2)
    mac_bb(map,int,string,mis_value)
    mac_bv(map,int,work,miw_value)
    mac_bp(map,int,work,miw_pointer)
    mac_vb(map,work,int,mwi_value)
    mac_vv(map, work, work, ww_map)
    mac_vp(map,work,work,ww_pointer);
    mac_pb(map,work,string,mwi_pv);
    mac_pv(map, work,work , ww_pr);
    mac_pp(map, work,work , ww_pp);
};

sf_class(student)


int main()
{
    // 1.创建一个manager
    skyfire::sf_object_manager manager;
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
