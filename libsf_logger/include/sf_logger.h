/*
 * sf_logger日志打印
 */

#pragma once
#include <fstream>
#include <sstream>
#include <mutex>
#include <map>
#include <ctime>
#include <iomanip>
#include <functional>
#include <thread>
#include <condition_variable>
#include <deque>

#include "sf_single_instance.h"
#include "sf_empty_class.h"

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
        std::string msg;
    };


    template<typename _Base = sf_empty_class>
    class sf_logger__ : public _Base
    {
    public:
        SF_SINGLE_TON(sf_logger__)

        virtual ~sf_logger__()
        {
        }

        void add_level_func(SF_LOG_LEVEL level, std::function<void(const std::string &)> func)
        {
            if (logger_func_set__.count(level) == 0)
            {
                logger_func_set__[level] = std::vector<std::function<void(const std::string &)>>();
            }
            logger_func_set__[level].push_back(func);
        }

        void add_level_stream(SF_LOG_LEVEL level, std::ostream *os)
        {
            if (logger_func_set__.count(level) == 0)
            {
                logger_func_set__[level] = std::vector<std::function<void(const std::string &)>>();
            }
            logger_func_set__[level].push_back([=](const std::string &str)
                                               {
                                                   *os << str;
                                               });
        }

        template<typename T>
        void logout(SF_LOG_LEVEL level, const std::string &file, int line, const std::string &func, const T &dt)
        {
            if (p_os__ == nullptr)
            {
                p_os__ = std::make_shared<std::ostringstream>();
            }
            *p_os__ << "[" << make_time_str__() << "]"
                    #ifdef SF_LOG_OUTPUT_THREAD_ID
                    <<"[" << std::this_thread::get_id() << "]"
                    #endif
                    << "[" << file << "]["
                    << line
                    << "][" << func << "]";
            logout__(level, dt);
        }

        template<typename...T>
        void logout(SF_LOG_LEVEL level, const std::string &file, int line, const std::string &func, const T &...dt)
        {
            if (p_os__ == nullptr)
            {
                p_os__ = std::make_shared<std::ostringstream>();
            }
            *p_os__ << "[" << make_time_str__() << "]"
                    #ifdef SF_LOG_OUTPUT_THREAD_ID
                    <<"[" << std::this_thread::get_id() << "]"
                    #endif
                    << "[" << file << "]["
                    << line
                    << "][" << func << "]";
            logout__(level, dt...);
        }


    private:
        std::deque<logger_info_t__> log_deque__;
        std::mutex cond_mu__;
        std::condition_variable cond__;
        std::mutex deque_mu__;
        std::map<int, std::vector<std::function<void(const std::string &)>>> logger_func_set__;

        static thread_local std::shared_ptr<std::ostringstream> p_os__;

        std::map<SF_LOG_LEVEL, std::string> logger_level_str__{
                {SF_DEBUG_LEVEL, "DEBUG"},
                {SF_INFO_LEVEL,  "INFO"},
                {SF_WARN_LEVEL,  "WARN"},
                {SF_ERROR_LEVEL, "ERROR"},
                {SF_FATAL_LEVEL, "FATAL"},
        };


        sf_logger__()
        {
            std::thread([=]()
                        {
                            while (true)
                            {
                                std::unique_lock<std::mutex> lck(cond_mu__);
                                cond__.wait(lck);
                                std::unique_lock<std::mutex> de_lck(deque_mu__);
                                for (auto &log: log_deque__)
                                {
                                    for (auto &level_func:logger_func_set__)
                                    {
                                        if (log.level >= level_func.first)
                                        {
                                            for (auto &func:level_func.second)
                                            {
                                                func(log.msg);
                                            }
                                        }
                                    }
                                }
                                log_deque__.clear();
                            }
                        }).detach();
        }


        template<typename T, typename...U>
        void logout__(SF_LOG_LEVEL level, const T &tmp, const U &...tmp2)
        {
            *p_os__ << "[" << tmp << "]";
            logout__(level, tmp2...);
        }

        template<typename T>
        void logout__(SF_LOG_LEVEL level, const T &tmp)
        {
            *p_os__ << "[" << tmp << "]";
            logger_info_t__ info{level, "[" + logger_level_str__[level] + "]" + p_os__->str() + "\n"};
            p_os__ = std::make_shared<std::ostringstream>();
            std::unique_lock<std::mutex> lck(deque_mu__);
            log_deque__.push_back(info);
            cond__.notify_one();
        }

        static std::string make_time_str__()
        {
            auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            tm tm_d;
            tm *ptm = &tm_d;
#ifdef _MSC_VER
            localtime_s(ptm, &tt);
#else
            ptm = localtime(&tt);
#endif
#if 0
            //sprint_f无法完成格式化，而sprintf可以，Why？
            char tmp[60] = {0};
#ifdef _MSC_VER
            sprintf_s
#else
            sprintf
#endif
                    (tmp, "%04d-%02d-%02d %02d:%02d:%02d",
                    ptm->tm_year + 1900,
                    ptm->tm_mon + 1,
                    ptm->tm_mday,
                    ptm->tm_hour,
                    ptm->tm_min,
                    ptm->tm_sec
            );
            return tmp;
#else
            std::ostringstream os;
            os << std::put_time(ptm, "%Y-%m-%d %H:%M:%S");
            return os.str();
#endif
        }
    };

    template<typename _Base>
    thread_local std::shared_ptr<std::ostringstream> sf_logger__<_Base>::p_os__{nullptr};

    using sf_logger = sf_logger__<>;

#define sf_debug(...) logout(SF_DEBUG_LEVEL,__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define sf_info(...) logout(SF_INFO_LEVEL,__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define sf_warn(...) logout(SF_WARN_LEVEL,__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define sf_error(...) logout(SF_ERROR_LEVEL,__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define sf_fatal(...) logout(SF_FATAL_LEVEL,__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)


}
