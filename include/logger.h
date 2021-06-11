
#pragma once
#include <bits/stdc++.h>
#ifdef QT_CORE_LIB
#include <QString>
#endif
#include "co_mutex.h"
#ifdef SF_COROUTINE
#include "co_condition_variable.h"
#include "coroutine.h"
#endif
#include "json.h"
#include "random.h"
#include "single_instance.h"
#include "utils.h"

namespace skyfire
{

#ifdef SF_DEBUG
#define sf_debug(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_DEBUG_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#else
#define sf_debug(...)
#endif
#define sf_info(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_INFO_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#define sf_warn(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_WARN_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#define sf_err(...)                                                        \
    skyfire::g_logger->logout(skyfire::SF_ERROR_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#define sf_fatal(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_FATAL_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)

constexpr int SF_DEBUG_LEVEL = 0;
constexpr int SF_INFO_LEVEL  = 1;
constexpr int SF_WARN_LEVEL  = 2;
constexpr int SF_ERROR_LEVEL = 3;
constexpr int SF_FATAL_LEVEL = 4;
struct logger_info_t
{
    int         level;     // 日志等级
    std::string time;      // 时间
    int         line;      // 行号
    std::string file;      // 文件名称
    std::string thread_id; // 线程号
    std::string func;      // 函数名称
    std::string msg;       // 消息
#ifdef SF_COROUTINE
    std::string co_name; // 协程名称
    std::string co_id;   // 协程id
#endif
};
SF_JSONIFY(logger_info_t, level, time, line, file, thread_id, func, msg);
inline std::unordered_map<int, std::string> logger_level_str__ {
    { SF_DEBUG_LEVEL, "DEBUG" },
    { SF_INFO_LEVEL, "INFO " },
    { SF_WARN_LEVEL, "WARN " },
    { SF_ERROR_LEVEL, "ERROR" },
    { SF_FATAL_LEVEL, "FATAL" },
};

#ifdef SF_COROUTINE
constexpr char default_log_format[]
    = "{level} {time} T:{thread} C:{co_name}({co_id}) {file}:{line} {func} --> {msg}\n";
#else
constexpr char default_log_format[]
    = "{level} {time} T:{thread} {file}:{line} {func} --> {msg}\n";
#endif

class logger
{
public:
    SF_SINGLE_TON(logger)
    int  add_level_func(int                                             level,
                        std::function<void(const logger_info_t&, bool)> func, bool color = true);
    int  add_level_stream(int level, std::ostream* os,
                          std::string format = default_log_format, bool color = true);
    int  add_level_file(int level, const std::string& filename,
                        std::string format = default_log_format, bool color = true);
    void remove_filter(int key);
    template <typename T>
    void logout(int level, const std::string& file, int line,
                const std::string& func, const T& dt);
    template <typename... T>
    void               logout(int level, const std::string& file, int line,
                              const std::string& func, const T&... dt);
    void               stop_logger();
    static void        empty_func__() {}
    static std::string format(std::string          format_str,
                              const logger_info_t& log_info, bool color);

    ~logger();

private:
    struct log_attr
    {
        std::function<void(const logger_info_t&, bool)> callback;
        bool                                            colored = true;
    };
    std::deque<logger_info_t> log_deque__;
#ifdef SF_COROUTINE
    co_condition_variable cond__;
    co_mutex              deque_mu__;
#else
    std::condition_variable                cond__;
    std::mutex                             deque_mu__;
#endif
    std::unordered_map<
        int, std::unordered_map<int, log_attr>>
        logger_func_set__;
    std::atomic<bool>    run__ { true };
    std::recursive_mutex func_set_mutex__;
#ifdef SF_COROUTINE
    std::shared_ptr<coroutine> log_routine__ = nullptr;
#else
    std::shared_ptr<std::thread>           log_routine__ = nullptr;
#endif
    bool check_key_can_use__(int key);
    int  make_random_logger_id__();
    logger();
    template <typename T, typename... U>
    void logout__(std::ostringstream& oss, logger_info_t& log_info,
                  const T& tmp, const U&... tmp2);
    template <typename T>
    void logout__(std::ostringstream& oss, logger_info_t& log_info,
                  const T& tmp);
};
inline auto g_logger = logger::instance();

// impl

template <typename T>
void logger::logout(int level, const std::string& file,
                    int line, const std::string& func, const T& dt)
{
    std::ostringstream thread_so;
    thread_so << "0x" << std::hex << std::this_thread::get_id();

#ifdef SF_COROUTINE
    std::ostringstream co_so;
    co_so << "0x" << std::hex << this_coroutine::get_id();
#endif

    logger_info_t log_info;
    log_info.level     = level;
    log_info.file      = file;
    log_info.line      = line;
    log_info.thread_id = thread_so.str();
    log_info.time      = make_time_str();
    log_info.func      = func;
#ifdef SF_COROUTINE
    log_info.co_id   = co_so.str();
    log_info.co_name = this_coroutine::get_name();
#endif

    std::ostringstream oss;
    logout__(oss, log_info, dt);
}

template <typename T, typename... U>
void logger::logout__(std::ostringstream& oss,
                      logger_info_t& log_info, const T& tmp,
                      const U&... tmp2)
{
    oss << "[" << tmp << "]";
    logout__(oss, log_info, tmp2...);
}
template <typename T>
void logger::logout__(std::ostringstream& oss,
                      logger_info_t& log_info, const T& tmp)
{
    oss << "[" << tmp << "]";
    log_info.msg = oss.str();
#ifdef SF_ASYNC_LOG
    {
        std::lock_guard<co_mutex> lck(deque_mu__);
        log_deque__.push_back(log_info);
    }
    cond_pop__.notify_one();
#else
    std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
    for (auto& level_func : logger_func_set__)
    {
        if (log_info.level >= level_func.first)
        {
            for (auto& func : level_func.second)
            {
                func.second.callback(log_info, func.second.colored);
            }
        }
    }
#endif
}
template <typename... T>
void logger::logout(const int level, const std::string& file,
                    const int line, const std::string& func,
                    const T&... dt)
{
    std::ostringstream thread_so;
    thread_so << "0x" << std::hex << std::this_thread::get_id();
#ifdef SF_COROUTINE
    std::ostringstream co_so;
    co_so << "0x" << std::hex << this_coroutine::get_id();
#endif
    logger_info_t log_info;
    log_info.level     = level;
    log_info.file      = file;
    log_info.line      = line;
    log_info.thread_id = thread_so.str();
    log_info.time      = make_time_str();
    log_info.func      = func;
#ifdef SF_COROUTINE
    log_info.co_id   = co_so.str();
    log_info.co_name = this_coroutine::get_name();
#endif
    std::ostringstream oss;
    logout__(oss, log_info, dt...);
}

} // namespace skyfire
