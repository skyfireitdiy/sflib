
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_timer.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_timer 定时器
 */
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_timer.h"

namespace skyfire {

inline void sf_timer::start(int ms, bool once) {
    if (running__) {
        return;
    }
    running__ = true;

    std::thread new_thread = std::thread(
        [this, ms](bool is_once) {
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
                if (std::this_thread::get_id() != current_timer_thread__) {
                    return;
                }
                if (running__) {
                    timeout();
                    if (is_once) {
                        running__ = false;
                        return;
                    }
                } else {
                    break;
                }
            }
        },
        once);
    current_timer_thread__ = new_thread.get_id();
    new_thread.detach();
}

inline bool sf_timer::is_active() const { return running__; }

inline void sf_timer::stop() { running__ = false; }
}    // namespace skyfire

#pragma clang diagnostic pop