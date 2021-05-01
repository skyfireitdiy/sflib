## sflib 工具库
相关文档：
[HTTP框架指南](doc/HTTP框架指南.md)
### 简介
`sflib`是一个集合本人在工作学习过程中常用的工具合集，主要包括以下组件：
* 根对象`object`
  `提供基于观察者模式的事件循环机制，附加包括eventloop（事件循环）、timer（定时器）、event_waiter（事件等待）。`
  
* RAII管理类`finally`
    `在作用域结束执行某些特定操作。`
    
* 日志类`logger`
    `提供线程安全的基础日志输出类，日志分级，多目的地输出。`
    
* 线程池简单实现`thread_pool`
    `提供接口易用的线程池实现。`

* 词法分析器`lex`，语法分析器`yacc`。
    `提供词法分析与语法分析，作为基础的编译器前端。`

* `json`库`json`。
    `提供对json数据的解析，生成，合并等操作，接口友好。`

* 对象工厂`object_factory`
    `提供工厂模式的简单实现。`
    
### 特点
* Header-Only
* 接口现代化、人性化
* 跨平台（目前在Windows和Linux测试通过）

Github: [https://github.com/skyfireitdiy](https://github.com/skyfireitdiy)
E-mail:[skyfireitdiy@hotmail.com](mailto:skyfireitdiy@hotmail.com)
QQ:1513008876
