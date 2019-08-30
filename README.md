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

### 编译&使用

`sflib`是纯头文件库(`Header-Only`)，所以不需要编译即可使用，将包含路径设置到项目路径下即可。（链接时需要依赖的库可参见`CMakeLists.txt`中测试程序的链接库，主要为`ws2_32.lib(Windows)`，`pthread(Linux)`，`libssl(openssl)`，`zlib`）

如果想要编译测试程序，需要准备以下工具：

* CMake 3.7 +
* make（或mingw32-make）
* g++ 8.1 +
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

