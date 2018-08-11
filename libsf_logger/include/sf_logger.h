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
#include "sf_random.h"
#include "sf_single_instance.h"
#include "sf_empty_class.h"
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

        int add_level_func(SF_LOG_LEVEL level, std::function<void(const std::string &)> func)
        {
            std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
            if (logger_func_set__.count(level) == 0)
            {
                logger_func_set__[level] = std::unordered_map<int,std::function<void(const std::string &)>>();
            }
            auto key = make_random_logger_id__();
            logger_func_set__[level][key] = func;
            return key;
        }

        int add_level_stream(SF_LOG_LEVEL level, std::ostream *os)
        {
            std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
            if (logger_func_set__.count(level) == 0)
            {
                logger_func_set__[level] = std::unordered_map<int,std::function<void(const std::string &)>>();
            }
            auto key = make_random_logger_id__();
            logger_func_set__[level][key] = [=](const std::string &str)
            {
                *os << str << std::flush;
            };
            return key;
        }

        int add_level_file(SF_LOG_LEVEL level, const std::string &filename)
        {
            std::shared_ptr<std::ofstream> ofs = std::make_shared<std::ofstream>(filename, std::ios::app);
            if(*ofs)
            {
                std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
                if (logger_func_set__.count(level) == 0)
                {
                    logger_func_set__[level] = std::unordered_map<int,std::function<void(const std::string &)>>();
                }

                auto key = make_random_logger_id__();
                logger_func_set__[level][key] = [=](const std::string &str){
                    *ofs << str << std::flush;
                };
                return key;

            }else{
                return -1;
            }
        }

        void remove_filter(int key){
            std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
            for(auto &p:logger_func_set__){
                p.second.erase(key);
            }
        }

        template<typename T>
        void logout(SF_LOG_LEVEL level, const std::string &file, int line, const std::string &func, const T &dt)
        {
            if (p_os__ == nullptr)
            {
                p_os__ = std::make_shared<std::ostringstream>();
            }
            *p_os__ << "[time:" << make_time_str__() << "]"
                    #ifdef SF_LOG_OUTPUT_THREAD_ID
                    <<"[thread:" << std::this_thread::get_id() << "]"
                    #endif
                    << "[file:" << file << " ("
                    << line
                    << ") -> " << func << "]";
            logout__(level, dt);
        }

        template<typename...T>
        void logout(SF_LOG_LEVEL level, const std::string &file, int line, const std::string &func, const T &...dt)
        {
            if (p_os__ == nullptr)
            {
                p_os__ = std::make_shared<std::ostringstream>();
            }
            *p_os__ << "[time:" << make_time_str__() << "]"
                    #ifdef SF_LOG_OUTPUT_THREAD_ID
                    <<"[thread:" << std::this_thread::get_id() << "]"
                    #endif
                    << "[file:" << file << " ("
                    << line
                    << ") -> " << func << "]";
            logout__(level, dt...);
        }

        void stop_logger()
        {
            run__ = false;
        }

        void empty_func__(){}

    private:
        std::deque<logger_info_t__> log_deque__;
        std::mutex cond_mu__;
        std::condition_variable cond__;
        std::mutex deque_mu__;
        std::map<int, std::unordered_map<int ,std::function<void(const std::string &)>>> logger_func_set__;
        std::atomic<bool> run__ {true};
        std::recursive_mutex func_set_mutex__;

        static thread_local std::shared_ptr<std::ostringstream> p_os__;

        bool check_key_can_use__(int key){
            std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
            for(const auto &p: logger_func_set__){
                for(const auto &q:p.second){
                    if(q.first == key)
                        return false;
                }
            }
            return true;
        }

        int make_random_logger_id__(){

            auto make_rand = []{
#ifdef SF_LOGGER_STANDALONE
                return rand();
#else
                return sf_random::get_instance()->get_int(0, INT_MAX);
#endif
            };
            int tmp_key = make_rand();
            while(!check_key_can_use__(tmp_key))
            {
                tmp_key = make_rand();
            }
            return tmp_key;
        }

        std::map<SF_LOG_LEVEL, std::string> logger_level_str__{
                {SF_DEBUG_LEVEL, "DEBUG"},
                {SF_INFO_LEVEL,  "INFO "},
                {SF_WARN_LEVEL,  "WARN "},
                {SF_ERROR_LEVEL, "ERROR"},
                {SF_FATAL_LEVEL, "FATAL"},
        };


        sf_logger__()
        {
            add_level_stream(SF_DEBUG_LEVEL, &std::cout);
            std::thread([=]()
                        {
                            while (true)
                            {
                                std::unique_lock<std::mutex> lck(cond_mu__);
                                cond__.wait(lck);
                                std::unique_lock<std::mutex> de_lck(deque_mu__);
                                for (auto &log: log_deque__)
                                {
                                    std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
                                    for (auto &level_func:logger_func_set__)
                                    {
                                        if (log.level >= level_func.first)
                                        {
                                            for (auto &func:level_func.second)
                                            {
                                                func.second(log.msg);
                                            }
                                        }
                                    }
                                }
                                log_deque__.clear();
                                if(!run__){
                                    run__ = true;
                                    break;
                                }
                            }
                        }).detach();
        }


        template<typename T, typename...U>
        void logout__(SF_LOG_LEVEL level, const T &tmp, const U &...tmp2)
        {
            *p_os__ << "[" << tmp << "]";
            logout__(level, tmp2...);
        }

#ifdef QT_CORE_LIB
        template<typename...U>
        void logout__(SF_LOG_LEVEL level, const QString &tmp, const U &...tmp2)
        {
            *p_os__ << "[" << tmp.toStdString() << "]";
            logout__(level, tmp2...);
        }

#endif

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

#ifdef QT_CORE_LIB

        //template<>
        void logout__(SF_LOG_LEVEL level, const QString &tmp)
        {
            *p_os__ << "[" <<  tmp.toStdString() << "]";
            logger_info_t__ info{level, "[" + logger_level_str__[level] + "]" + p_os__->str() + "\n"};
            p_os__ = std::make_shared<std::ostringstream>();
            std::unique_lock<std::mutex> lck(deque_mu__);
            log_deque__.push_back(info);
            cond__.notify_one();
        }
#endif

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

    namespace {
        auto g_logger = sf_logger::get_instance();
    }

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
