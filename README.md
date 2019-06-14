## sflib 工具库

### 简介

`sflib`是一个集合本人在工作学习过程中常用的工具合集，主要包括以下组件：

* 根对象`sf_object`

  `提供基于观察者模式的事件循环机制，附加包括sf_eventloop（事件循环）、sf_timer（定时器）、sf_event_waiter（事件等待）,并且支持侵入式面向切面编程。`
  
* RAII管理类`sf_finally`

    `在作用域结束执行某些特定操作。`
    
* 日志类`sf_logger`

    `提供线程安全的基础日志输出类，日志分级，多目的地输出。`
    
* tcp异步网络库`sf_tcp_server`、`sf_tcp_client`

    `提供一个异步网络通信框架，在Windows下的实现为IOCP，linux下的实现为epoll。`
    
* 线程池简单实现`sf_thread_pool`

    `提供接口易用的线程池实现。`
    
* RPC（远程过程调用）框架`sf_rpc_server`、`sf_rpc_client`

    `提供简单的rpc框架，支持同步调用和异步调用。`
    
* 基于网络的消息总线（订阅模式）框架`sf_msg_bus`

    `提供一个基于tcp的跨主机消息总线模型。`
    
* 内网NAT网络穿透框架`sf_tcp_nat_traversal_server`、`sf_tcp_nat_traversal_client`（因缺乏测试条件，目前未作详细测试）

    `提供用于内网NAT穿透的通信框架。`
    
* HTTP服务器框架`sf_http_server`，包含基础框架`sf_http_base_server`

    `提供HTTP/1.1服务器端实现，包括WebSocket服务器实现。`

* 词法分析器`sf_lex`，语法分析器`sf_yacc`。

    `提供词法分析与语法分析，作为基础的编译器前端。`

* `json`库`sf_json`。

    `提供对json数据的解析，生成，合并等操作，接口友好。`

* 对象工厂`sf_object_factory`

    `提供工厂模式的简单实现。`

    
### 特点

* Header-Only
* 接口现代化、人性化
* 跨平台（目前在Windows和Linux测试通过）
    
### 快速使用

#### `sf_object`基于信号的通信方式使用

```cpp
#include "sf_object.hpp"

using namespace skyfire;

// 1. 类继承自sf_object
class A: public sf_object
{
    // 2.注册一个信号，信号名称为s1，参数列表为int
    SF_REG_SIGNAL(s1, int)
};

// 3.定义一个槽函数，参数列表与信号参数列表一致
void slot1(int a)
{
    std::cout<<std::this_thread::get_id()<<" "<<a<<std::endl;
}

int main()
{
    A a;
    // 4.绑定信号与槽， 对象指针（可以是智能指针），信号名称，槽，是否使用消息队列
    sf_bind_signal(&a, s1, slot1, false);
    sf_bind_signal(&a, s1, slot1, true);
    a.s1(56);

    sf_eventloop e;
    // 5.启动事件循环
    e.exec();
}
```

使用步骤：

1. 类继承自`sf_object`

2. 类内部可以使用`SF_REG_SIGNAL`宏注册信号，

    ``如要注册一个`void func(int)`类型的信号，使用`SF_REG_SIGNAL(func,int)`，信号没有返回值，所以宏中没有提供返回类型声明。`SF_REG_SIGNAL`的时用格式为SF_REG_SIGNAL(信号名称,参数1,参数2,...)``
    
3. 定义一个回调函数（槽函数）。其参数列表与注册的信号一致。

4. 使用`sf_bind_signal`绑定信号与槽函数。
    
    ``参数列表为`sf_bind_signal(信号发出对象指针,信号名称,槽函数,是否使用消息队列)`，其中`是否使用消息队列`为true时，当信号发出会发生直接调用（多用于同一线程中），会等待调用返回，不需要事件循环即可完成。为false时，使用消息队列，这种情况下信号发出时，会将此信号发送至消息队列，由事件循环调用槽函数（多用于不同线程），信号发出端不会阻塞。``
    
5. 启动事件循环。（如果不使用消息队列，这一步可省略）

#### 面向切面编程（AOP）使用

```cpp
#include <sf_object.hpp>
using namespace skyfire;

// 1.对象继承自sf_object
class A : public sf_object
{
public:
    // 2. 注册aop成员函数，会生成aop_xxx函数
    SF_REG_AOP(func, int , int)
    int func(int a, int b) const
    {
        std::cout<<a+b<<std::endl;
        return a+b;
    }
};

// 3.定义一个函数，使其插入到函数调用前，参数列表与要注入的函数相同
void before_call(int a,int b)
{
    std::cout<<"a="<<a<<",b="<<b<<std::endl;
}

// 4.定义一个函数，使其插入到函数调用后，参数列表与要注入的函数返回类型
void after_call()
{
    std::cout<<"call finished"<<std::endl;
}

int main()
{
    A a;
    // 5.注入before_call，参数为 对象指针、成员函数名称、注入函数名称, 返回注入id，可用此id撤销注入
    auto id = sf_aop_before_bind(&a, func, before_call);
    // 6. 同样也支持lambda
    sf_aop_before_bind(&a, func, [](int a,int b){
        std::cout<<"this is lambda, a="<<a<<",b="<<b<<std::endl;
    });
    // 7. 注入after_call
    sf_aop_after_bind(&a, func, after_call);
    // 8. 调用函数
    a.aop_func(5,10);

    // 输出:
    //    this is lambda, a=5,b=10
    //    a=5,b=10
    //    15
    //    call finished

    // 9.撤销注入
    sf_aop_before_unbind(&a, func, id);
    a.aop_func(5,10);

    // 输出：
    //    this is lambda, a=5,b=10
    //    15
    //    call finished
}
```

使用步骤：

1. 对象继承自`sf_object`

2. 定义要切入的成员函数，并使用`SF_REG_AOP`宏生成before和after切入面。`SF_REG_AOP`宏的使用方法与`SF_REG_SIGNAL`宏基本相同，只是`SF_REG_AOP`的第一个参数必须为要切入函数的名字。`SF_REG_AOP`会生成 `aop_函数名`的函数。

3. 定义切入函数。before面切入函数的参数列表与要切入函数的参数列表一致。

4. 定义切入函数。after面切入函数参数列表为空。

5. 使用`sf_aop_before_bind`切入before切面，`sf_aop_before_bind`的使用类似`sf_bind_signal`（没有了消息队列选项）.

6. 也可以切入lambda

7. 使用`sf_aop_after_bind`切入before切面

8. 调用函数。**（注意：调用的是`aop_func`，而不是`func`）**

9. 可以通过`sf_aop_before_unbind`和`sf_aop_after_unbind`撤销切入。

#### 事件等待和timer使用

```cpp
#include "sf_object.hpp"
#include "sf_event_waiter.hpp"
#include "sf_timer.hpp"

using namespace skyfire;


sf_timer timer;

int main()
{
    std::cout<<"Start"<<std::endl;
    timer.start(5000, true);
    // 1. 等待超时
    SF_WAIT(&timer, timeout);
    std::cout<<"End"<<std::endl;
}
```

使用步骤：

1. `sf_timer`有`start`成员函数，接收一个延时（毫秒）和一个是否一次性触发的参数。如`t.start(1000,true)`表示1000ms后触发`t.timeout()`信号，然后定时器停止。

2. `sf_wait`用于等待事件触发，当等待的事件没有触发时，会一直阻塞。调用为`sf_wait(对象指针,事件)`。

#### RAII管理使用

```cpp
#include "sf_finally.hpp"

using namespace skyfire;

int main()
{
    int *p = new int;
    // 1. 注册一个删除函数，在作用域结束的时候执行（后执行）
    sf_finally del_p([&](){
        delete p;
        std::cout<<"Delete p"<< std::endl;
    });
    {
        int *q = new int;
        // 2. 注册一个删除函数，在作用域结束的时候执行（先执行）
        sf_finally del_q([&](){
            delete q;
            std::cout<<"Delete q"<< std::endl;
        });
    }
}
```

使用步骤：

1. 在资源分配后，生成一个清理函数，传入一个`sf_finally`对象，该对象在析构时自动调用指定的清理函数。

#### 线程池使用

```cpp
#include "sf_thread_pool.hpp"
#include <iostream>

using namespace skyfire;


long long sum(long long from, long long n)
{
    std::cout<<"add from "<<from<<std::endl;
    long long ret;
    for(long long i =0;i<n;++i)
    {
        ret += from + i;
    }
    return ret;
}


int main()
{
    // 1. 创建线程池对象
    sf_thread_pool pool4(4);
    // 2. 用于存储线程的返回值
    std::vector<std::future<long long>> item;
    // 3. 增加任务
    for(long long i=0;i<10;++i)
    {
        item.emplace_back(pool4.add_task(sum, 10000*i, 10000));
    }
    long long ret = 0;
    // 4. 合并（使用返回值）
    for(auto &p:item)
    {
        ret += p.get();
    }
    std::cout<<ret<<std::endl;
}
```

使用步骤：

1. 创建一个线程池，构造函数传入线程数量

2. 使用`add_task`添加任务，可接受任意可调用对象，与`thread`的参数列表一致。

3. 使用`wait_all_task_finished`等待任务执行完成。（线程依然存在）

#### tcp服务器使用

```cpp
#include "sf_tcp_server.h"

using namespace skyfire;



int main()
{
    // 1.创建服务器
    auto server = sf_tcp_server::make_server(true);
    // 2.监听
    if(!server->listen("0.0.0.0",9988))
    {
        std::cout << "listen on 9988 error" << std::endl;
        return -1;
    }
    // 3.设置数据到来事件响应
    sf_bind_signal(server,raw_data_coming,[=](SOCKET sock, const byte_array& data){
        std::cout << "recv:" << to_string(data) << std::endl;
    },true);
    // 4. 启动消息循环
    sf_eventloop loop;
    loop.exec();
}
```

使用步骤：

1. 创建一个`sf_tcp_server`对象。传入参数为是否为原始数据。`sf_tcp_server`和`sf_tcp_client`内部会有一套封包、解包、校验流程，推荐使用，但是如果要实现一些原始协议（如`http`、`websocket`等），需要原始数据。

2. 监听。传入本机监听的`ip`和`port`。

3. 绑定信号（信号可参照API文档）

4. 启动消息循环

#### 内网穿透框架使用

穿透服务器

```cpp
#include "sf_tcp_nat_traversal_server.hpp"

using namespace skyfire;

int main(){
    g_logger->add_level_stream(SF_DEBUG_LEVEL,&std::cout);
    // 1. 创建nat穿透server
    auto pserver = sf_tcp_nat_traversal_server::make_server();
    std::cout<<"port:"<<std::flush;
    unsigned short port;
    std::cin>>port;
    // 2.监听端口
    auto ret = pserver->listen("0.0.0.0",port);
    if(ret){
        std::cout<<"监听成功"<<std::endl;
    }else{
        std::cout<<"监听失败"<<std::endl;
    }

    // 3. 启动事件循环
    sf_eventloop loop;
    loop.exec();
}
```

客户端
```cpp
#include "sf_tcp_nat_traversal_client.hpp"

using namespace skyfire;

void connect(std::shared_ptr<sf_tcp_nat_traversal_client> client){
    std::string ip;
    unsigned short port;
    std::cout<<"ip:"<<std::flush;
    getline(std::cin,ip);
    std::cout<<"port:"<<std::flush;
    std::cin>>port;
    std::cin.clear();
    if(client->connect_to_server(ip,port)){
        std::cout<<"连接成功"<<std::endl;
    }else{
        std::cout<<"连接失败"<<std::endl;
    }
}

void ls(std::shared_ptr<sf_tcp_nat_traversal_client> client){
    auto clients = client->get_clients();
    for(auto &p:clients){
        std::cout<<p<<std::endl;
    }
}

void nat_conn(std::shared_ptr<sf_tcp_nat_traversal_client> client){
    unsigned long long id;
    std::cout<<"id:"<<std::flush;
    std::cin>>id;
    std::cin.clear();
    client->connect_to_peer(id, false);
}

void send(std::shared_ptr<sf_tcp_nat_traversal_connection> conn) {
    if(!conn){
        std::cout<<"connect error"<<std::endl;
        return;
    }
    std::cout<<"msg:"<<std::flush;
    std::string msg;
    getline(std::cin,msg);
    conn->send(tcp_pkg_type_user + 1, to_byte_array(skyfire:: to_json(msg).to_string()));
}


int main() {
    // 1.创建nat穿透客户端
    auto pclient = sf_tcp_nat_traversal_client::make_client();
    std::shared_ptr<sf_tcp_nat_traversal_connection> conn;

    // 2.设新连接到来响应
    sf_bind_signal(pclient, new_nat_connection, std::function<void(std::shared_ptr<sf_tcp_nat_traversal_connection>, std::string)>([&](std::shared_ptr<sf_tcp_nat_traversal_connection> conn_t, std::string){
        std::cout<<"new connection!"<<std::endl;
        conn = conn_t;
        sf_bind_signal(conn, data_coming, [](const sf_pkg_header_t &header, const byte_array &data){
            std::string msg;
            from_json(sf_json::from_string(to_string(data)),msg);
            std::cout<<"Recv:"<<msg<<std::endl;
        }, true);
    }),true);

    while(true){
        std::string cmd;
        std::cout<<">"<<std::flush;
        getline(std::cin,cmd);
        if(cmd == "exit"){
            break;
        }
        // 3.连接
        if(cmd == "conn"){
            connect(pclient);
        }else if(cmd == "ls"){ // 4.枚举客户端
            ls(pclient);
        }else if(cmd == "natconn"){ // 5.连接客户端
            nat_conn(pclient);
        }else if(cmd == "id"){ // 6.获取id
            std::cout<<pclient->get_id()<<std::endl;
        }else if(cmd == "send"){ // 7.发送消息
            send(conn);
        }
    }
}
```

使用步骤：

1. 穿透服务器只需要监听某端口就可以了。

2. 穿透客户端首先与穿透服务器连接（`connect_to_server`函数）。

3. 获取可以连接的客户端列表。（`get_clients`函数）。

4. 根据客户端列表中的`id`，连接远程内网客户端。（`connect_to_peer`函数，传入`id`和是否使用原始通信格式）

5. 如果建立连接成功，两边的客户端都会触发一个`new_nat_connection`信号，信号中会带一个`std::shared_ptr<sf_tcp_nat_traversal_connection>`对象用于通信。

6. 对`std::shared_ptr<sf_tcp_nat_traversal_connection>`对象的操作类似于一个tcp客户端，详细接口见API文档。

#### 日志工具的使用

```cpp
// 1.SF_DEBUG表示启用sf_debug宏
#define SF_DEBUG
#include "sf_logger.hpp"

using namespace skyfire;


int main()
{
    // 2.获取单例对象
    auto logger = sf_logger::get_instance();
    // 3.添加日志文件，SF_WARN_LEVEL以上的日志级别将会打印到此文件中
    logger->add_level_file(SF_WARN_LEVEL, "runlog.log");
    // 4.SF_INFO_LEVEL以上的日志级别将会打印到标准输出流
    logger->add_level_stream(SF_INFO_LEVEL, &std::cout);
    // 5.日志打印
    sf_error("hello", "world");
    sf_warn("this is warn");
    sf_error("hello", "world");
    sf_warn("this is warn");
    getchar();
    g_logger->stop_logger();
    getchar();
}
```

使用步骤：

1. 在包含`sf_logger.hpp`之前，可以定义`SF_DEBUG`宏，这个宏的定义使`SF_DEBUG_LEVEL`日志等级生效，否则默认不生效。（建议在调试的时候打开，并使用`sf_debug`输出调试级别日志）。

2. 获取全局单例日志对象。

3. 使用`add_level_file`添加指定等级的日志文件。

4. 使用`add_level_stream`添加指定等级的输出流。

5. 使用`sf_debug,sf_info,sf_warn,sf_error,sf_fatal`打印日志。（更多内容见API文档）

#### 远程过程调用（RPC）框架使用


服务器端
```cpp
#include "sf_rpc_server.hpp"

using namespace skyfire;

void print()
{
    std::cout<<"print"<<std::endl;
}

std::vector<int> add_one(std::vector<int> data)
{
    for(auto &p: data)
    {
        ++p;
    }
    return data;
}

void output(const char *str)
{
    std::cout<<str<<std::endl;
}

void print_str(byte_array data)
{
	std::cout << skyfire::to_string(data) << std::endl;
}

int main()
{
    // 1.创建server对象
    auto server = sf_rpc_server::make_server();
    // 2.注册rpc函数
    server->reg_rpc_func("print", print);
    server->reg_rpc_func("add_one", add_one);
	server->reg_rpc_func("print_str", print_str);
    // 3.监听
    std::cout<<server->listen("127.0.0.1",10001)<<std::endl;
    sf_eventloop event_loop;
    // 4.启动时间循环
    event_loop.exec();
}
```

使用步骤：

1. 创建`sf_rpc_server`对象

2. 使用`reg_rpc_func`注册RPC函数，函数接收一个字符串`id`和一个可调用对象。

3. 监听端口，提供服务

4. 启动消息循环

客户端
```cpp
#include "sf_rpc_client.hpp"
using namespace skyfire;
using namespace std::literals;

void disp_vec(std::vector<int> data)
{
    for(auto p: data)
    {
        std::cout<<p<<" "<<std::flush;
    }
    std::cout<<std::endl;
}

int main()
{
    // 1.创建rpc客户端
    auto client = sf_rpc_client::make_client();
    // 2.连接rpc服务器
    if (!client->connect_to_server("127.0.0.1", 10001))
    {
        std::cout << "connect error" << std::endl;
        return -1;
    }
    // 3.同步调用，无返回值
    client->call<>("print"s);
	client->call<>("print_str", to_byte_array("hello world"s));
    std::cout<<"call finished"<<std::endl;
    std::vector<int> data = {9,5,6,7,41,23,4,5,7};
    disp_vec(data);
    // 4.同步调用，返回sf_assigned_type<vector<int>>，使用*解引用（需要显式指明返回值类型）
    data = *client->call<std::vector<int>>("add_one"s, data);
    disp_vec(data);
    std::cout<<"---------"<<std::endl;
    // 5.异步调用，第二个参数为参数为rpc函数返回类型的回调函数（需要显式指明返回值类型）
    client->async_call<std::vector<int>>("add_one"s, disp_vec, data);
    getchar();
}

```

使用步骤：

1. 创建RPC客户端

2. 连接RPC服务器

3. 使用`call<>`进行同步调用无返回值函数，参数为过程`id`和参数列表。

4. 使用`call<Type>`同步调用返回值类型为`Type`的函数，实际返回的类型为`sf_assigned_type<Type>`包装后类型，使用`bool()`可以判断值的合法性，使用`*`解引用获取原始值。

5. 使用`async_call<Type>`进行异步调用，调用方式为`async_call<Type>(过程id,异步回调函数,参数列表...)`。回调函数会在RPC调用返回时被调用，参数类型为`Type`

#### 消息总线框架使用

服务器端
```cpp
#include "sf_msg_bus_server.hpp"

using namespace skyfire;

int main()
{
    // 1.创建一个消息总线服务器
    auto server = sf_msg_bus_server::make_server();
    // 2.监听
    server->listen("127.0.0.1", 5678);
    std::string type;
    std::string data;
    while(true)
    {
        // 3.输入并往总线上投递消息
        std::cout<<"type:"<<std::flush;
        std::cin>>type;
        if(type == "quit")
            break;
        std::cout<<"data:"<<std::flush;
        std::cin>>data;
        server->send_msg(type, to_byte_array(skyfire::to_json(data).to_string()));
    }
    // 4.关闭总线
    server->close();
}
```

使用步骤：

1. 创建一个消息总线服务器

2. 监听端口

3. 投递消息

4. 关闭总线


客户端
```cpp
#include "sf_msg_bus_client.hpp"

using namespace skyfire;

int main()
{
    // 1. 生成客户端
    auto client = sf_msg_bus_client::make_client();
    // 2. 连接到消息总线服务器
    client->connect_to_server("127.0.0.1", 5678);
    // 3. 添加事件到来相应
    sf_bind_signal(client, msg_come, [](std::string, byte_array data){
        std::string str;
		from_json(sf_json::from_string(to_string(data)), str);
        std::cout<<"msg_come:"<<str<<std::endl;
        }, true);
    std::string type;
    std::cin>>type;
    // 4. 注册消息
    client->reg_msg_to_bus(type);
    sf_eventloop e;
    // 5.事件循环
    e.exec();
}
```

使用步骤：

1. 生成消息总线客户端

2. 连接服务器

3. 添加信号响应

4. 注册信息

5. 事件循环，处理信号

#### http服务器框架使用

```cpp
#include "sf_http_server.hpp"

using namespace std::literals;

using namespace skyfire;

void upload_file_route(const sf_http_request &req,sf_http_response& res) {
    // 判断请求类型是否为分块请求
    if(req.is_multipart_data())
    {
        std::string ret_str;
        // 获取请求头
        auto header = req.get_multipart_data_context().header;
        for(auto &p:header)
        {
            ret_str += p.first + ":" + p.second + "\n";
        }
        ret_str += "-----------header end-------------\n";
        // 获取分块
        auto multipart = req.get_multipart_data_context().multipart;
        for (auto &p: multipart)
        {
            // 获取分块请求头
            auto tmp_header = p.get_header().get_header();
            for (auto &q:tmp_header)
            {
                ret_str += q.first + ":" + q.second + "\n";
            }
            ret_str += "temp filename:" + p.get_filename() + "\n--------------------------\n\n";
        }
        // 将上述拼接的头信息拼装至body返回
        res.set_body(to_byte_array(ret_str));
    }
    else
    {
        res.set_body(to_byte_array("upload ok, no file"s));
        sf_http_cookie_t cookie;

        cookie.path = req.get_request_line().url;
        cookie.key="token";
        cookie.value=sf_random::get_instance()->get_uuid_str();
        // 添加cookie
        res.add_cookie(cookie);
        cookie.key="time";
        cookie.value = sf_make_http_time_str();
        cookie.http_only = false;
        res.add_cookie(cookie);
    }

    res.set_status(200);
}

void websocket_route(const sf_websocket_param_t& param)
{
    // 如果类型为文本，返回hello:原内容
    if(param.type == websocket_data_type::TextData){
        std::cout<<"received:"<<param.text_msg<<std::endl;
        param.p_server->send_websocket_data(param.sock,"hello:" + param.text_msg);
    } else{
        std::cout<<"binary data"<<std::endl;
    }
}


int main() {
    // 1. 生成一份http server配置
    sf_http_server_config config;
    config.host = "0.0.0.0";        // 监听ip
    config.port = 8080;             // 端口
    config.request_timeout = 30;    // http请求超时
    // 2. 根据配置生成一个http server
    auto server = sf_http_server::make_server(config);

    // 3. 添加一个http路由，地址为/upload_file， 回调函数为upload_file_route，方法为所有
    server->add_router(make_http_router(
            "/upload_file"s,
            upload_file_route,
            std::vector<std::string>{{"*"s}}
    ));

    // 4. 同样支持lambda
    server->add_router(make_http_router(
            "/user/(.*)/name"s,
            // 使用lambda时需要使用function包装一下，例子中第一个string参数会接收整个url，第二个会接收(.*?)匹配的url，规则与<regex>相同
            std::function([](const sf_http_request &req,sf_http_response& res, std::string, std::string user){
                res.set_body(to_byte_array(user+"'s name is skyfire"s));
            }),
            std::vector<std::string>{{"GET"s}}
    ));

    // 5. 添加一个websocket路由，地址为/ws，回调函数为websocket_route
    server->add_router(make_websocket_router("/ws", websocket_route));

    // 6. 设置一个静态资源路由，位置为"../example/test_http_server/testWebsite"，请求为所有，默认文件编码为utf-8，启用deflate压缩
    server->add_router(make_static_router(R"(C:\code\sflib\example\test_http_server\testWebsite)",  {{"*"s}}, "utf-8", true));

    // 7. 启动服务
    server->start(); 
}
```

使用步骤：

1. 生成服务器配置`sf_http_server_config`。

2. 创建`sf_http_server`实例。

3. 添加一个http路由，`sf_http_router`的构造函数参数列表为`url`（可以是正则表达式）,回调函数（接收一个`sf_http_request`，一个`sf_http_response`，若干个`std::string`，其中`std::string`表示前面正则表达式匹配的`url`字符串，规则见标准库`<regex>`），请求方式列表（`std::vector<std::string>`，每一项表示一个方式，可以是`GET`,`POST`等，`*`表示任意方式），优先级，默认为0，优先级越低越先匹配。

4. 回调函数同样支持`lambda`，需要使用`std::function`包装一下。

5. 添加一个`WebSocket`路由。make_websocket_router函数参数为`url`和回调函数，回调函数接收`sf_websocket_param_t`参数。

6. 设置一个静态资源路由。`make_static_router`参数为本地静态资源路径、请求方式列表、默认编码，是否启用压缩，优先级（默认为100）。

7. 启动服务。

该框架单纯为`http`框架，开发`Web`应用可搭配[`ODB`orm框架](https://www.codesynthesis.com/products/odb/)和[`inja`模板引擎](https://github.com/pantor/inja)


#### 词法分析器与语法分析器的使用。

```cpp

#include "sf_lex.hpp"
#include "sf_yacc.hpp"

using namespace skyfire;

int main()
{
    // 1. 一个测试用的json
    std::string json_str = R"({
    "code": 0,
    "message": "交易成功",
    "object": {
        "head": {
            "version": "1.0.0",
            "code": "contract_setting_info",
            "timestamp": 1400138523979
        },
        "body": {
            "contractSettings": [
                {
                    "contractDict": {
                        "code": "YGXAG9995",
                        "name": "粤贵银9995",
                        "market_code": "PMEC",
                        "status": 1,
                        "sort": 0
                    },
                    "contractEx": {
                        "unit_number": 15,
                        "unit_type": "KG",
                        "min_quote_unit": 1,
                        "min_var_unit": 1,
                        "step_unit": 1,
                        "currency": "CNY",
                        "quote_type": 1
                    },
                    "contractProtect": {
                        "spread": 20,
                        "time_interval": 60000
                    },
                    "contractAccess": null
                },
                {
                    "contractDict": {
                        "code": "YGXAG9999",
                        "name": "粤贵银9999",
                        "market_code": "PMEC",
                        "status": 1,
                        "sort": 0
                    },
                    "contractEx": {
                        "unit_number": 1,
                        "unit_type": "KG",
                        "min_quote_unit": 1,
                        "min_var_unit": 1,
                        "step_unit": 1,
                        "currency": "CNY",
                        "quote_type": 1
                    },
                    "contractProtect": {
                        "spread": 20,
                        "time_interval": 600
                    },
                    "contractAccess": null
                },
                {
                    "contractDict": {
                        "code": "YGXAG100G",
                        "name": "粤贵银100G",
                        "market_code": "PMEC",
                        "status": 1,
                        "sort": 0
                    },
                    "contractEx": {
                        "unit_number": 100,
                        "unit_type": "g",
                        "min_quote_unit": 0.001,
                        "min_var_unit": 0.001,
                        "step_unit": 0.001,
                        "currency": "CNY",
                        "quote_type": 1
                    },
                    "contractProtect": {
                        "spread": 20,
                        "time_interval": 600
                    },
                    "contractAccess": null
                }
            ]
        }
    },
    "success": true
})";

    // 2. 创建一个词法分析器，并添加词法规则
    sf_lex lex;
    lex.set_rules({
                          {"string", R"("([^\\"]|(\\["\\/bnrt]|(u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])))*")"},
                          {"[",      R"(\[)"},
                          {"]",      R"(\])"},
                          {"{",      R"(\{)"},
                          {"}",      R"(\})"},
                          {",",      R"(,)"},
                          {":",      R"(:)"},
                          {"ws",     R"([\r\n\t ]+)"},
                          {"number", R"(-?(0|[1-9]\d*)(\.\d+)?(e|E(\+|-)?0|[1-9]\d*)?)"},
                          {"true",   R"(true)"},
                          {"false",  R"(false)"},
                          {"null",   R"(null)"}
                  });

    // 3.词法分析
    std::vector<sf_lex_result_t> lex_result;
    if (lex.parse(json_str, lex_result))
    {
        // 4. 移除词法分析结果中的空白(ws)
        lex_result.erase(std::remove_if(lex_result.begin(), lex_result.end(), [](const sf_lex_result_t &r)
        {
            return r.id == "ws";
        }), lex_result.end());
        for (auto &p:lex_result)
        {
            std::cout << p.matched_str << "(" << p.id << ")" << std::flush;
        }

        std::cout << std::endl << std::endl;

        // 5. 生成语法分析器，添加语法规则
        sf_yacc yacc;
        yacc.set_rules({
                               {
                                       "value",
                                       {
                                               {
                                                       {"object"},
                                                       nullptr
                                               },
                                               {
                                                       {"array"},
                                                       nullptr
                                               },
                                               {
                                                       {"string"},
                                                       nullptr
                                               },
                                               {
                                                       {"number"},
                                                       nullptr
                                               },
                                               {
                                                       {"true"},
                                                       nullptr
                                               },
                                               {
                                                       {"false"},
                                                       nullptr
                                               },
                                               {
                                                       {"null"},
                                                       nullptr
                                               }
                                       }
                               },
                               {
                                   // 6. 表示 文法 object 可由 "{",  "}" （即{}）或者
                                   // "{",  "members",  "}"（即{"hello":"world"}）推导出来
                                   // 用通俗一点的话说，一个json对象，可以是空对象，或者一个或多个键值对构成
                                       "object",
                                       {
                                               {
                                                       {"{",  "}"},
                                                       nullptr
                                               },
                                               {
                                                       {"{",  "members",  "}"},
                                                       nullptr
                                               }
                                       }
                               },
                               {
                                       "members",
                                       {
                                               {
                                                       {"member"},
                                                       nullptr
                                               },
                                               {
                                                       {"members",             ",",    "member"},
                                                       nullptr
                                               }
                                       }
                               },
                               {
                                       "member",
                                       {
                                               {
                                                       {"string",              ":", "value"},
                                                       nullptr
                                               }
                                       }
                               },
                               {
                                       "array",
                                       {
                                               {
                                                       {"[", "]"},
                                                       nullptr
                                               },
                                               {
                                                       {"[", "values", "]"},
                                                       nullptr
                                               }
                                       }
                               },
                               {
                                       "values",
                                       {
                                               {
                                                       {"value"},
                                                       nullptr
                                               },
                                               {
                                                       {"values",            ",",    "value"},
                                                       nullptr
                                               }
                                       }
                               }
                       });

        // 7. 添加终结符号，即json最终解析出来的结果应该是value 或者 array 或者 object
        yacc.add_terminate_ids({"array", "object"});
        // 8. 语法分析，使用词法分析的结果作为输入
        std::vector<std::shared_ptr<sf_yacc_result_t>> yacc_result;
        yacc.parse(lex_result, yacc_result);
        {
            for (auto &p:yacc_result)
            {
                std::cout << "(" << p->id << ")[" << p->text << "]" << std::flush;
            }
        }
    }


}
```

使用步骤：

对照上述代码，实现一个`json`的词法与语法分析。

1. 测试数据，一个相对比较复杂的`json`。

2. 生成一个词法分析器。设置词法规则。

    ``词法分析器的规则设置函数参数为一个集合，每一项包含一个关键字`id`和一个正则表达式，表示符合该正则表达式的字符串可归约为指定的`id`。``

3. 词法分析，如果成功，回生成一个词法分析结果集合。

4. 移除词法分析结果中的空白结果（`json`中的空白没什么用，但是如果是`python`这类的语言，就不能移除空白了）.

5. 生成语法分析器，添加语法规则。

6. 语法规则的参数相对比较复杂。

    ``整体是一个规则集合，每条记录包含一个关键字`id`，以及可以推导出此`id`的若干条语法/词法规则序列以及推导出来是生成用户数据的回调函数（示例只是为了给`json`做语法解析，而不是真的要处理`json`数据，所以回调函数留空，详细的使用方法，可以参考`sf_json`的实现代码）。

7. 添加终结符号，即可以生成语言的符号集合，`json`为`object`，`array`和`value`。

8. 进行语法分析，如果成功，则会返回语法分析结果（如果失败也会返回结果，可以通过查看结果判断语法规则是否出错）。

该分析器使用需要懂一些编译原理的相关知识，如果了解一些语法词法推导，使用还是很轻松的。

#### `json`处理。

使用例子：

```cpp
#include "sf_json.hpp"

using namespace skyfire;


struct Time
{
    int h;
    int m;
    int s;
};

SF_JSONIFY(Time,h,m,s)

struct People
{
    int age;
    std::string name;
    std::string address;
    Time t;
};

SF_JSONIFY(People, age, name ,address, t)

int main()
{
	using namespace std::literals;
    // 1. 使用字面值操作符解析json
    auto json1 = R"({"name": "wmb","data": [1,2,3,"666"]})"_json;
    auto json2 = R"({"age":25, "address":"西安"})"_json;
    auto json3 = R"({"company":"saming"})"_json;
    // 2. 输出json
    std::cout<<json1<<std::endl;
    std::cout<<json2<<std::endl;
    std::cout<<json3<<std::endl;
    std::cout<<"---------"<<std::endl;
    // 3. 合并json
    json1.join(json2);
    json1.join(json3.clone());
    std::cout<<json1<<std::endl;
    std::cout<<json2<<std::endl;
    std::cout<<json3<<std::endl;
    std::cout<<"---------"<<std::endl;
    // 4. sf_json底层使用指针，数据属于浅拷贝，所以改变json2的值同样会影响json1的值
    json2["address"] = "北京";
    std::cout<<json1<<std::endl;
    std::cout<<json2<<std::endl;
    std::cout<<json3<<std::endl;
    std::cout<<"---------"<<std::endl;
    // 5. json3与json1进行合并时，使用clone()创建了副本（深拷贝）,操作json3不会影响json1
    json3["company"]="lenovo";
    std::cout<<json1<<std::endl;
    std::cout<<json2<<std::endl;
    std::cout<<json3<<std::endl;
    std::cout<<"---------"<<std::endl;

	// 6.json序列化
    std::tuple<int, std::string,double> t{5,"hello"s, 5.2};
    auto t_js = to_json(t);
    std::cout<<t_js<<std::endl;

    std::tuple<int, std::string,double> t2;
    from_json(t_js, t2);
    std::cout<< std::get<0>(t2)<<std::endl;
    std::cout<< std::get<1>(t2)<<std::endl;
    std::cout<< std::get<2>(t2)<<std::endl;

    People p;
    p.name = "skyfire";
    p.age = 100;
    p.address = "China";
    p.t.h = 24;
    p.t.m = 60;
    p.t.s = 25;
    auto js = to_json(p);
    std::cout<<js<<std::endl;

    People p2;
    from_json(js, p2);
    std::cout<<to_json(p2)<<std::endl;

	// 7.对字节容器的序列化有优化
	byte_array bin_data{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26 };
	auto bin_data_json = to_json(bin_data);
	std::cout << bin_data_json << std::endl;

	byte_array bin_data2;
	from_json(bin_data_json, bin_data2);
	for (auto &p : bin_data2)
	{
		std::cout << int(p) << std::ends;
	}
	std::cout<<std::endl;

	getchar();
}

```


1. 使用字面值操作符解析`json`。

2. 输出`json`。

3. 合并`json`。

4. `sf_json`底层使用指针，数据属于浅拷贝，所以改变`json2`的值同样会影响`json1`的值。

5. `json3`与`json1`进行合并时，使用`clone()`创建了副本（深拷贝）,操作`json3`不会影响`json1`。

对`json`的更多操作请参阅API文档。

6. `json`序列化

7. 对字节容器的序列化有优化

#### 更多

在例子中，用到了一些其他组件，如二进制序列化`to_json`、随机数`sf_random`以及单例对象`sf_singel_instance`等。

此快速入门仅作为介绍作用，详细使用方法见API文档。（下文会给出API文档生成方法）


### 编译&使用

`sflib`是纯头文件库(`Header-Only`)，所以不需要编译即可使用，将包含路径设置到项目路径下即可。（链接时需要依赖的库可参见`CMakeLists.txt`中测试程序的链接库，主要为`ws2_32.lib(Windows)`，`pthread(Linux)`，`libssl(openssl)`，`zlib`）

如果想要编译测试程序，需要准备以下工具：

* CMake 3.7 +
* make（或mingw32-make）
* g++ 7.2 +
* openssl 1.1.0h （建议版本）
* zlib 1.2.11 （建议版本）
* doxygen （生成文档时必需）

在`Linux`环境下，使用终端进入项目目录，生成编译目录并进入：

```shell
mkdir build
cd build
```

执行`cmake`

```shell
cmake ..
```

如果此步骤出错，请检查`openssl`和`zlib`是否已经安装并配置完成。

执行`make`。

```shell
make -j
```

如果需要安装`sflib`至系统包含目录（不建议），执行`install`操作。

```shell
sudo make install
```

如果需要生成API文档，进入到项目目录`doc`子目录下，调用`doxygen`生成。

```shell
doxygen
```

生成后的API文档位于`html`文件夹下，使用浏览器打开`index.html`即可浏览API文档。

### 后续

`sflib`第一版草草完成，涉及到错误及异常处理的代码不完善，整体比较脆弱，也没有系统的异常处理体系，后续在使用开发过程中会逐步完善。

后面计划在不断完善打磨`sflib`的同时，不断往里面加入一些新元素，并会开发一些示例`demo`引导用户使用。

欢迎各位大佬批评指正！

Github: [https://github.com/skyfireitdiy](https://github.com/skyfireitdiy)

E-mail:[skyfireitdiy@hotmail.com](mailto:skyfireitdiy@hotmail.com)

QQ:1513008876

