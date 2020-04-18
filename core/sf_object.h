
/**
* @version 1.0.0
* @author skyfire
* @file sf_object.h
*/

/*
 * sf_object 基础对象
 * 提供了信号---槽的通信机制
 * 
 * sf_object提供的信号--槽通信机制分为两种工作模式：单线程与多线程
 * 由sf_bind_signal宏的最后一个参数决定，如果该参数是true，代表在
 * 单线程中工作，所有的调用都是同步调用，而如果该参数传递微为false，
 * 则会使用消息队列异步处理信号。
 * 
 * 示例1：
 * 
 * ```cpp
 * #include <core/sf_object>
 * #include <core/sf_stdc++.h>
 * 
 * using namespace skyfire;
 * using namespace std;
 * 
 * class car : public sf_object {
 *     SF_REG_SIGNAL(run)
 *     SF_REG_SIGNAL(stop, int)
 * };
 * 
 * int main()
 * {
 *     car c;
 *     sf_bind_signal(
 *         &c, run, []() {
 *             cout << "car run" << endl;
 *         },
 *         true);
 *     sf_bind_signal(
 *         &c, stop, [](int d) {
 *             this_thread::sleep_for(chrono::seconds(3));
 *             cout << "car stop at:" << d << endl;
 *         },
 *         true);
 *     sf_bind_signal(
 *         &c, stop, [](int d) {
 *             cout << "Wow!!! car run " << d << " kms!" << endl;
 *         },
 *         true);
 *     c.run();
 *     c.stop(50);
 * }
 * ```
 * 
 * 运行该程序会输出：
 * ```text
 * car run
 * car stop at:50
 * Wow!!! car run 50 kms!
 * ```
 * 
 * 由于是单线程，相当于直接函数调用，所以在第二行与第三行之间会停顿3s。
 * 
 * 示例2：
 * ```cpp
 * #include <core/sf_object>
 * #include <core/sf_stdc++.h>
 * 
 * using namespace skyfire;
 * using namespace std;
 * 
 * class car : public sf_object {
 *     SF_REG_SIGNAL(run)
 *     SF_REG_SIGNAL(stop, int)
 * };
 * 
 * int main()
 * {
 *     car c;
 *     sf_bind_signal(
 *         &c, run, []() {
 *             cout << "car run" << endl;
 *         },
 *         false);
 *     sf_bind_signal(
 *         &c, stop, [](int d) {
 *             cout << "car stop at:" << d << endl;
 *             this_thread::sleep_for(chrono::seconds(3));
 *         },
 *         false);
 *     sf_bind_signal(
 *         &c, stop, [](int d) {
 *             cout << "Wow!!! car run " << d << " kms!" << endl;
 *         },
 *         false);
 *     c.run();
 *     c.stop(50);
 *     sf_eventloop loop1;
 *     sf_eventloop loop2;
 * 
 *     thread th1([&]() {
 *         loop1.exec();
 *     });
 *     loop2.exec();
 *     th1.join();
 * }
 * ```
 * 
 * 该例子中，使用了多线程的信号槽通信机制，在多线程通信中，会使用全局消息队列进行
 * 数据传输，在调用信号函数之后，信号函数在全局消息队列中添加一个信号回调函数了并
 * 立即返回，而sf_event_loop对象的exec函数会不断从全局消息队列中取出回调函数并
 * 执行。例子中在不同线程启动了两个消息循环，这样在第一个线程sleep后。第二个线程
 * 依然可以从队列中获取任务执行。
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"

#include "core/sf_object_msg_queue.hpp"
#include "sf_stdc++.h"
#include "tools/sf_json.hpp"

/*
 * SF_REG_SIGNAL 注册信号的宏
 */
#define SF_REG_SIGNAL(name, ...)                                           \
public:                                                                    \
    std::recursive_mutex __mu_##name##_signal_;                            \
    std::vector<std::tuple<std::function<void(__VA_ARGS__)>, bool, int>>   \
        __##name##_signal_func_vec__;                                      \
    template <typename... __SF_OBJECT_ARGS__>                              \
    void name(__SF_OBJECT_ARGS__&&... args)                                \
    {                                                                      \
        std::lock_guard<std::recursive_mutex> lck(__mu_##name##_signal_);  \
        for (auto& p : __##name##_signal_func_vec__) {                     \
            if (std::get<1>(p)) {                                          \
                std::get<0>(p)(std::forward<__SF_OBJECT_ARGS__>(args)...); \
            } else {                                                       \
                auto bind_obj = std::bind(std::get<0>(p),                  \
                    std::forward<__SF_OBJECT_ARGS__>(args)...);            \
                __p_msg_queue__->add_msg(std::pair(this, bind_obj));       \
            }                                                              \
        }                                                                  \
    }

/*
 * sf_bind_signal 信号绑定
 */
#define sf_bind_signal(objptr, name, func, ...)                 \
    (objptr)->__sf_bind_helper((objptr)->__mu_##name##_signal_, \
        (objptr)->__##name##_signal_func_vec__, func,           \
        __VA_ARGS__)

/*
 * sf_unbind_signal 信号解绑
 */
#define sf_unbind_signal(objptr, name, bind_id) \
    (objptr)->__sf_signal_unbind_helper(        \
        (objptr)->__mu_##name##_signal_,        \
        (objptr)->__##name##_signal_func_vec__, bind_id);

namespace skyfire {
/**
 *  @brief 元对象
 */
class sf_object {
public:
    template <typename _VectorType, typename _FuncType>
    int __sf_bind_helper(std::recursive_mutex& mu, _VectorType& vec,
        _FuncType func, bool single_thread = true);

    template <typename _VectorType>
    void __sf_signal_unbind_helper(std::recursive_mutex& mu, _VectorType& vec,
        int bind_id);

    virtual ~sf_object();

protected:
    sf_object_msg_queue* __p_msg_queue__ = sf_object_msg_queue::instance();
};

} // namespace skyfire

#pragma clang diagnostic pop