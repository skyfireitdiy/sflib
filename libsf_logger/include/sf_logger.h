/*
 * sf_logger日志打印
 */

#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <mutex>
#include <map>
#include <ctime>
#include <iomanip>
#include <functional>
#include <thread>
#include <condition_variable>
#include <deque>
#include <atomic>
#include <unordered_map>
#include <climits>
#include <shared_mutex>
#include <thread>

#ifdef QT_CORE_LIB
#include <QString>
#endif

// 独立使用

#ifdef SF_LOGGER_STANDALONE
namespace skyfire{
    class sf_empty_class{};

}

#define SF_SINGLE_TON(ClassName)								\
ClassName(const ClassName&) = delete;							\
ClassName(ClassName&&) = delete;								\
ClassName& operator=(const ClassName&) = delete;				\
static ClassName* get_instance()								\
{																\
	static std::mutex init_mutex;								\
	static ClassName* instance__{ nullptr };					\
	if(instance__==nullptr){									\
		std::lock_guard<std::mutex> lck(init_mutex);			\
		if (instance__ == nullptr)								\
		{														\
			instance__ = new ClassName;							\
		}														\
	}															\
	return instance__;											\
}																\

#else
#include "sf_random.hpp"
#include "sf_single_instance.hpp"
#include "sf_empty_class.hpp"
#endif

namespace skyfire
{

    enum SF_LOG_LEVEL
    {
        SF_DEBUG_LEVEL = 0,
        SF_INFO_LEVEL = 1,
        SF_WARN_LEVEL = 2,
        SF_ERROR_LEVEL = 3,
        SF_FATAL_LEVEL = 4
    };

    struct logger_info_t__
    {
        SF_LOG_LEVEL level;
        std::string time;
        int line;
        std::string file;
        std::thread::id thread_id;
        std::string func;
        std::string msg;
    };


    constexpr char sf_default_log_format[] = "[{level}][{time}][{thread}][{file} ({line}) {func}] --> {msg}\n";

    template<typename _Base = sf_empty_class>
    class sf_logger__ : public _Base
    {
    public:
        SF_SINGLE_TON(sf_logger__)

        int add_level_func(SF_LOG_LEVEL level, std::function<void(const logger_info_t__ &)> func);

        int add_level_stream(SF_LOG_LEVEL level, std::ostream *os, std::string format = sf_default_log_format);

        int add_level_file(SF_LOG_LEVEL level, const std::string &filename, std::string format = sf_default_log_format);

        void remove_filter(int key);

        template<typename T>
        void logout(SF_LOG_LEVEL level, const std::string &file, int line, const std::string &func, const T &dt);

        template<typename...T>
        void logout(SF_LOG_LEVEL level, const std::string &file, int line, const std::string &func, const T &...dt);

        void stop_logger();

        void empty_func__(){}

        std::string format(std::string format_str, const logger_info_t__& log_info);

    private:
        std::deque<logger_info_t__> log_deque__;
        std::mutex cond_mu__;
        std::condition_variable cond__;
        std::mutex deque_mu__;
        std::map<int, std::unordered_map<int ,std::function<void(const logger_info_t__ &)>>> logger_func_set__;
        std::atomic<bool> run__ {true};
        std::recursive_mutex func_set_mutex__;

        static thread_local std::shared_ptr<std::ostringstream> p_os__;

        bool check_key_can_use__(int key);

        int make_random_logger_id__();

        std::map<SF_LOG_LEVEL, std::string> logger_level_str__{
                {SF_DEBUG_LEVEL, "DEBUG"},
                {SF_INFO_LEVEL,  "INFO "},
                {SF_WARN_LEVEL,  "WARN "},
                {SF_ERROR_LEVEL, "ERROR"},
                {SF_FATAL_LEVEL, "FATAL"},
        };


        sf_logger__();


        template<typename T, typename...U>
        void logout__(logger_info_t__ &log_info, const T &tmp, const U &...tmp2);

#ifdef QT_CORE_LIB
        template<typename...U>
        void logout__(logger_info_t__ &log_info, const QString &tmp, const U &...tmp2);
        //template<>
        void logout__(logger_info_t__ &log_info, const QString &tmp);
#endif
        template<typename T>
        void logout__(logger_info_t__ &log_info, const T &tmp);

        static std::string make_time_str__();
    };
    using sf_logger = sf_logger__<>;


#ifdef SF_DEBUG
#define sf_debug(...) skyfire::g_logger->logout(skyfire::SF_DEBUG_LEVEL,__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#else
#define sf_debug(...) skyfire::g_logger->empty_func__()
#endif

#define sf_info(...) skyfire::g_logger->logout(skyfire::SF_INFO_LEVEL,__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define sf_warn(...) skyfire::g_logger->logout(skyfire::SF_WARN_LEVEL,__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define sf_error(...) skyfire::g_logger->logout(skyfire::SF_ERROR_LEVEL,__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define sf_fatal(...) skyfire::g_logger->logout(skyfire::SF_FATAL_LEVEL,__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)



}
