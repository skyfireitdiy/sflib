
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_thread_pool.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include <iostream>
#include "tools/sf_thread_pool.hpp"

using namespace skyfire;

long long sum(long long from, long long n) {
    std::cout << "add from " << from << std::endl;
    long long ret;
    for (long long i = 0; i < n; ++i) {
        ret += from + i;
    }
    return ret;
}

int main() {
    // 1. 创建线程池对象
    sf_thread_pool pool4(4);
    // 2. 用于存储线程的返回值
    std::vector<std::future<long long>> item;
    // 3. 增加任务
    for (long long i = 0; i < 10; ++i) {
        item.emplace_back(pool4.add_task(sum, 10000 * i, 10000));
    }
    long long ret = 0;
    // 4. 合并（使用返回值）
    for (auto &p : item) {
        ret += p.get();
    }
    std::cout << ret << std::endl;
}
