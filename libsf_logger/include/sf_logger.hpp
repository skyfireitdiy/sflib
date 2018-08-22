/*
 * sf_logger日志打印
 */

#pragma once

#include "sf_logger.h"

namespace skyfire
{
    template<typename _Base>
    int sf_logger__<_Base>::add_level_func(SF_LOG_LEVEL level, std::function<void(const std::string &)> func) {
        std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
        if (logger_func_set__.count(level) == 0)
        {
            logger_func_set__[level] = std::unordered_map<int,std::function<void(const std::string &)>>();
        }
        auto key = make_random_logger_id__();
        logger_func_set__[level][key] = func;
        return key;
    }

    template<typename _Base>
    int sf_logger__<_Base>::add_level_stream(SF_LOG_LEVEL level, std::ostream *os) {
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

    template<typename _Base>
    int sf_logger__<_Base>::add_level_file(SF_LOG_LEVEL level, const std::string &filename) {
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

    template<typename _Base>
    void sf_logger__<_Base>::remove_filter(int key) {
        std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
        for(auto &p:logger_func_set__){
            p.second.erase(key);
        }
    }

    template<typename _Base>
    bool sf_logger__<_Base>::check_key_can_use__(int key) {
        std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
        for(const auto &p: logger_func_set__){
            for(const auto &q:p.second){
                if(q.first == key)
                    return false;
            }
        }
        return true;
    }

    template<typename _Base>
    int sf_logger__<_Base>::make_random_logger_id__() {

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

    template<typename _Base>
    template<typename T>
    void sf_logger__<_Base>::logout(SF_LOG_LEVEL level, const std::string &file, int line, const std::string &func,
                                    const T &dt) {
        if (p_os__ == nullptr)
        {
            p_os__ = std::make_shared<std::ostringstream>();
        }
        *p_os__ << "[" << make_time_str__() << "]"
                #ifdef SF_LOG_OUTPUT_THREAD_ID
                <<"[" << std::this_thread::get_id() << "]"
                #endif
                << "[" << file << " ("
                << line
                << ") " << func << "] -----> ";
        logout__(level, dt);
    }

    template<typename _Base>
    sf_logger__<_Base>::sf_logger__() {
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

    template<typename _Base>
    template<typename T, typename... U>
    void sf_logger__<_Base>::logout__(SF_LOG_LEVEL level, const T &tmp, const U &... tmp2) {
        *p_os__ << "[" << tmp << "]";
        logout__(level, tmp2...);
    }

    template<typename _Base>
    template<typename T>
    void sf_logger__<_Base>::logout__(SF_LOG_LEVEL level, const T &tmp) {
        *p_os__ << "[" << tmp << "]";
        logger_info_t__ info{level, "[" + logger_level_str__[level] + "]" + p_os__->str() + "\n"};
        p_os__ = std::make_shared<std::ostringstream>();
        std::unique_lock<std::mutex> lck(deque_mu__);
        log_deque__.push_back(info);
        cond__.notify_one();
    }

    template<typename _Base>
    std::string sf_logger__<_Base>::make_time_str__() {
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

    template<typename _Base>
    template<typename... T>
    void sf_logger__<_Base>::logout(SF_LOG_LEVEL level, const std::string &file, int line, const std::string &func,
                                    const T &... dt) {
        if (p_os__ == nullptr)
        {
            p_os__ = std::make_shared<std::ostringstream>();
        }
        *p_os__ << "[" << make_time_str__() << "]"
                #ifdef SF_LOG_OUTPUT_THREAD_ID
                <<"[" << std::this_thread::get_id() << "]"
                #endif
                << "[" << file << " ("
                << line
                << ") -> " << func << "] -----> ";
        logout__(level, dt...);
    }

    template<typename _Base>
    void sf_logger__<_Base>::stop_logger()
    {
        run__ = false;
    }

    template<typename _Base>
    thread_local std::shared_ptr<std::ostringstream> sf_logger__<_Base>::p_os__{nullptr};

    namespace {
        auto g_logger = sf_logger::get_instance();
    }

#ifdef QT_CORE_LIB
    template<typename _Base>
    void sf_logger__<_Base>::logout__(SF_LOG_LEVEL level, const QString &tmp)
    {
        *p_os__ << "[" <<  tmp.toStdString() << "]";
        logger_info_t__ info{level, "[" + logger_level_str__[level] + "]" + p_os__->str() + "\n"};
        p_os__ = std::make_shared<std::ostringstream>();
        std::unique_lock<std::mutex> lck(deque_mu__);
        log_deque__.push_back(info);
        cond__.notify_one();
    }

    template<typename _Base>
    template<typename...U>
    void sf_logger__<_Base>::logout__(SF_LOG_LEVEL level, const QString &tmp, const U &...tmp2)
    {
        *p_os__ << "[" << tmp.toStdString() << "]";
        logout__(level, tmp2...);
    }

#endif

}
