
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_logger.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_logger日志打印
 */

#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedMacroInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "cert-err58-cpp"

#include "core/sf_stdc++.h"

#ifdef QT_CORE_LIB
#include <QString>
#endif

#include "sf_random.hpp"
#include "sf_single_instance.hpp"
#include "sf_utils.hpp"

namespace skyfire {

/**
 * @brief  日志等级
 */
enum SF_LOG_LEVEL {
    SF_DEBUG_LEVEL = 0,
    SF_INFO_LEVEL = 1,
    SF_WARN_LEVEL = 2,
    SF_ERROR_LEVEL = 3,
    SF_FATAL_LEVEL = 4
};

/**
 *  @brief 日志信息
 */
struct sf_logger_info_t__ {
    SF_LOG_LEVEL level;           // 日志等级
    std::string time;             // 时间
    int line;                     // 行号
    std::string file;             // 文件名称
    std::thread::id thread_id;    // 线程号
    std::string func;             // 函数名称
    std::string msg;              // 消息
};

/**
 * 各种level对应的字符串
 */
inline std::unordered_map<SF_LOG_LEVEL, std::string> logger_level_str__{
    {SF_DEBUG_LEVEL, "DEBUG"}, {SF_INFO_LEVEL, "INFO "},
    {SF_WARN_LEVEL, "WARN "},  {SF_ERROR_LEVEL, "ERROR"},
    {SF_FATAL_LEVEL, "FATAL"},
};

/**
 *  @brief 默认日志格式
 */
constexpr char sf_default_log_format[] =
    "[{level}][{time}][{thread}][{file} ({line}) {func}] --> {msg}\n";

/**
 * 日志类
 */
class sf_logger {
   public:
    SF_SINGLE_TON(sf_logger)

    /**
     * 添加指定等级的回调函数
     * @param level 等级
     * @param func 函数
     * @return id号（可用于移除回调）
     */
    int add_level_func(SF_LOG_LEVEL level,
                       std::function<void(const sf_logger_info_t__ &)> func);

    /**
     * 添加指定等级日志输出流
     * @param level 等级
     * @param os 输出流
     * @param format 格式化字符串
     * @return id号（可用于移除回调）
     */
    int add_level_stream(SF_LOG_LEVEL level, std::ostream *os,
                         std::string format = sf_default_log_format);

    /**
     * 添加指定等级日志输出文件
     * @param level 等级
     * @param filename 文件名
     * @param format 格式化字符串
     * @return id号（可用于移除回调）
     */
    int add_level_file(SF_LOG_LEVEL level, const std::string &filename,
                       std::string format = sf_default_log_format);

    /**
     * 根据id删除过滤器
     * @param key id
     */
    void remove_filter(int key);

    template <typename T>
    void logout(SF_LOG_LEVEL level, const std::string &file, int line,
                const std::string &func, const T &dt);

    template <typename... T>
    void logout(SF_LOG_LEVEL level, const std::string &file, int line,
                const std::string &func, const T &... dt);

    void stop_logger();

    static void empty_func__() {}

    static std::string format(std::string format_str,
                              const sf_logger_info_t__ &log_info);

   private:
    std::deque<sf_logger_info_t__> log_deque__;
    std::condition_variable cond__;
    std::mutex deque_mu__;
    std::unordered_map<
        int, std::unordered_map<
                 int, std::function<void(const sf_logger_info_t__ &)>>>
        logger_func_set__;
    std::atomic<bool> run__{true};
    std::recursive_mutex func_set_mutex__;

    bool check_key_can_use__(int key);

    int make_random_logger_id__();

    sf_logger();

    template <typename T, typename... U>
    void logout__(std::ostringstream &oss, sf_logger_info_t__ &log_info,
                  const T &tmp, const U &... tmp2);

#ifdef QT_CORE_LIB
    template <typename... U>
    void logout__(std::ostringstream &oss, sf_logger_info_t__ &log_info,
                  const QString &tmp, const U &... tmp2);
    // template<>
    void logout__(std::ostringstream &oss, sf_logger_info_t__ &log_info,
                  const QString &tmp);
#endif
    template <typename T>
    void logout__(std::ostringstream &oss, sf_logger_info_t__ &log_info,
                  const T &tmp);
};

#ifdef SF_DEBUG
#define sf_debug(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_DEBUG_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#else
#define sf_debug(...) skyfire::g_logger->empty_func__()
#endif

#define sf_info(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_INFO_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#define sf_warn(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_WARN_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#define sf_error(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_ERROR_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)
#define sf_fatal(...)                                                      \
    skyfire::g_logger->logout(skyfire::SF_FATAL_LEVEL, __FILE__, __LINE__, \
                              __FUNCTION__, __VA_ARGS__)

}    // namespace skyfire

#pragma clang diagnostic pop