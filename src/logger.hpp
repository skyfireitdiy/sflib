
#pragma once
#include "co_utils.h"
#include "color.h"
#include "json.h"
#include "logger.h"
#include "thread_pool.h"
namespace skyfire
{
inline std::unordered_map<int, std::vector<color>> log_color_map = {
    { SF_DEBUG_LEVEL, { color_fg_cyan } },
    { SF_INFO_LEVEL, { color_fg_blue } },
    { SF_WARN_LEVEL, { color_fg_yellow } },
    { SF_ERROR_LEVEL, { color_fg_magenta } },
    { SF_FATAL_LEVEL, { color_fg_red } },
};
inline int
logger::add_level_func(
    int level, const std::function<void(const logger_info_t&, bool)> func, bool colored)
{
    std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
    if (logger_func_set__.count(level) == 0)
    {
        logger_func_set__[level] = std::unordered_map<
            int, log_attr>();
    }
    const auto key                = make_random_logger_id__();
    logger_func_set__[level][key] = { func, colored };
    return key;
}
inline int logger::add_level_stream(const int     level,
                                    std::ostream* os,
                                    std::string format_str, bool colored)
{
    std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
    if (logger_func_set__.count(level) == 0)
    {
        logger_func_set__[level] = std::unordered_map<
            int, log_attr>();
    }
    const auto key                = make_random_logger_id__();
    logger_func_set__[level][key] = { [=](const logger_info_t& log_info, bool colored) {
                                         *os << format(format_str, log_info, colored) << std::flush;
                                     },
                                      colored };
    return key;
}
inline int logger::add_level_file(const int          level,
                                  const std::string& filename,
                                  std::string format_str, bool colored)
{
    const auto ofs = std::make_shared<std::ofstream>(filename, std::ios::app);
    if (*ofs)
    {
        std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
        if (logger_func_set__.count(level) == 0)
        {
            logger_func_set__[level] = std::unordered_map<
                int, log_attr>();
        }
        const auto key                = make_random_logger_id__();
        logger_func_set__[level][key] = {
            [=](const logger_info_t& log_info, bool colored) {
                *ofs << format(format_str, log_info, colored) << std::flush;
            },
            colored
        };
        return key;
    }
    else
    {
        return -1;
    }
}
inline void logger::remove_filter(const int key)
{
    std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
    for (auto& p : logger_func_set__)
    {
        p.second.erase(key);
    }
}
inline bool logger::check_key_can_use__(const int key)
{
    std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
    for (const auto& p : logger_func_set__)
    {
        for (const auto& q : p.second)
        {
            if (q.first == key)
                return false;
        }
    }
    return true;
}
inline int logger::make_random_logger_id__()
{
    const auto make_rand = [] {
        return random::instance()->rand_int(0, INT_MAX);
    };
    auto tmp_key = make_rand();
    while (!check_key_can_use__(tmp_key))
    {
        tmp_key = make_rand();
    }
    return tmp_key;
}
template <typename T>
inline void logger::logout(int level, const std::string& file,
                           int line, const std::string& func, const T& dt)
{
    std::ostringstream so;
    so << std::this_thread::get_id();
    logger_info_t log_info;
    log_info.level     = level;
    log_info.file      = file;
    log_info.line      = line;
    log_info.thread_id = so.str();
    log_info.time      = make_time_str();
    log_info.func      = func;
    log_info.co_id     = std::to_string(this_coroutine::get_id());
    log_info.co_name   = this_coroutine::get_name();

    std::ostringstream oss;
    logout__(oss, log_info, dt);
}
inline logger::logger()
{
    add_level_stream(SF_DEBUG_LEVEL, &std::cout);
#ifdef SF_ASYNC_LOG
    log_thread__ = std::make_shared(std::thread, [this]() {
        while (true)
        {
            std::deque<logger_info_t> tmp_info;
            {
                std::unique_lock<std::mutex> lck(deque_mu__);
                cond_pop__.wait(lck,
                                [&] { return !log_deque__.empty() || !run__; });
                tmp_info = std::move(log_deque__);
                log_deque__.clear();
            }
            for (auto& log : tmp_info)
            {
                std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
                for (auto& level_func : logger_func_set__)
                {
                    if (log.level >= level_func.first)
                    {
                        for (auto& func : level_func.second)
                        {
                            func.second.callback(log, func.second.colored);
                        }
                    }
                }
            }
            if (!run__)
            {
                run__ = true;
                break;
            }
        }
        std::cout << "thread exit" << std::endl;
    });
#endif
}
inline logger::~logger()
{
    if (log_thread__)
    {
        stop_logger();
        log_thread__->join();
    }
}
template <typename T, typename... U>
inline void logger::logout__(std::ostringstream& oss,
                             logger_info_t& log_info, const T& tmp,
                             const U&... tmp2)
{
    oss << "[" << tmp << "]";
    logout__(oss, log_info, tmp2...);
}
template <typename T>
inline void logger::logout__(std::ostringstream& oss,
                             logger_info_t& log_info, const T& tmp)
{
    oss << "[" << tmp << "]";
    log_info.msg = oss.str();
#ifdef SF_ASYNC_LOG
    {
        std::lock_guard<std::mutex> lck(deque_mu__);
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
inline void logger::logout(const int level, const std::string& file,
                           const int line, const std::string& func,
                           const T&... dt)
{
    std::ostringstream so;
    so << std::this_thread::get_id();
    logger_info_t log_info;
    log_info.level     = level;
    log_info.file      = file;
    log_info.line      = line;
    log_info.thread_id = so.str();
    log_info.time      = make_time_str();
    log_info.func      = func;
    log_info.co_id     = std::to_string(this_coroutine::get_id());
    log_info.co_name   = this_coroutine::get_name();
    std::ostringstream oss;
    logout__(oss, log_info, dt...);
}
inline void logger::stop_logger()
{
    run__ = false;
    cond__.notify_all();
}
inline auto g_logger = logger::instance();
#ifdef QT_CORE_LIB
inline void logger::logout__(std::ostringstream& oss,
                             logger_info_t&      log_info,
                             const QString&      tmp)
{
    oss << "[" << tmp.toStdString() << "]";
    log_info.msg = oss.str();
#ifdef SF_ASYNC_LOG
    {
        std::lock_guard<std::mutex> lck(deque_mu__);
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
                func.second(log_info);
            }
        }
    }
#endif
}
template <typename... U>
void logger::logout__(std::ostringstream& oss, logger_info_t& log_info,
                      const QString& tmp, const U&... tmp2)
{
    oss << "[" << tmp.toStdString() << "]";
    logout__(oss, log_info, tmp2...);
}
#endif
inline std::string logger::format(std::string          format_str,
                                  const logger_info_t& log_info, bool colored)
{
    const auto replace = [](std::string& str, const std::string& from,
                            const std::string& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    };
    replace(format_str, "{func}", log_info.func);
    replace(format_str, "{time}", log_info.time);
    replace(format_str, "{thread}", log_info.thread_id);
    replace(format_str, "{line}", std::to_string(log_info.line));
    replace(format_str, "{file}", log_info.file);
    replace(format_str, "{level}", logger_level_str__[log_info.level]);
    replace(format_str, "{msg}", log_info.msg);
    replace(format_str, "{co_id}", log_info.co_id);
    replace(format_str, "{co_name}", log_info.co_name);

    return colored ? color_string(format_str, log_color_map[log_info.level]) : format_str;
}
} // namespace skyfire
