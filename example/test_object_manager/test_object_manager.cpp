#include "sf_object.hpp"
#include "sf_object_manager.hpp"
#include <vector>


class work : public skyfire::sf_object
{
private:
    sf_reg_class(work)
    m_base(std::string,name);
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
    m_base(std::string, name)
    m_base(int, age)
    m_value(work,works)
    m_pointer(work,works2)
    mc_base(std::vector,int,vi)
    mc_value(std::list, work, wlist)
    mc_pointer(std::list, work, wlist2)
    mac_bb(std::map,int, std::string,mis_value)
    mac_bv(std::map,int,work,miw_value)
    mac_bp(std::map,int,work,miw_pointer)
    mac_vb(std::map,work,int,mwi_value)
    mac_vv(std::map, work, work, ww_map)
    mac_vp(std::map,work,work,ww_pointer);
    mac_pb(std::map,work,std::string,mwi_pv);
    mac_pv(std::map, work,work , ww_pr);
    mac_pp(std::map, work,work , ww_pp);
};

sf_class(student)


int main()
{
    // 1.创建一个manager
    skyfire::sf_object_manager manager;
    // 2.加载配置文件
    auto ret = manager.load_config("/home/skyfire/CLionProjects/sflib/example/test_object_manager/config.json");

    if(!ret){
	    std::cout<<"加载配置文件失败";
        return 0;
    }

    // 3. 获取my_work对象，类型是work
    auto wk = manager.get_object<work>("my_work");

    // 4.将my_work序列化为json
    std::cout<<wk->to_json()<< std::endl;

    // 5. 修改my_work的属性
    wk->set_name("skyfire");


    std::cout<<wk->to_json()<< std::endl;

    // 6. 再次获取my_work对象，此时获取到的与上一次一致
    wk = manager.get_object<work>("my_work");
    std::cout<<wk->to_json()<< std::endl;

    // 7. 获取student对象，此时student中的works属性与上面的my_work一致
    auto st = manager.get_object<student>("my_student");
    std::cout<<st->to_json()<< std::endl;

}
