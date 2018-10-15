/*
 * sf_logger日志打印
 */

#pragma once

#include "sf_logger.h"

namespace skyfire
{
    template<typename _Base>
    int sf_logger__<_Base>::add_level_func(SF_LOG_LEVEL level, std::function<void(const logger_info_t__ &)> func) {
        std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
        if (logger_func_set__.count(level) == 0)
        {
            logger_func_set__[level] = std::unordered_map<int,std::function<void(const logger_info_t__ &)>>();
        }
        auto key = make_random_logger_id__();
        logger_func_set__[level][key] = func;
        return key;
    }

    template<typename _Base>
    int sf_logger__<_Base>::add_level_stream(SF_LOG_LEVEL level, std::ostream *os, std::string format_str) {
        std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
        if (logger_func_set__.count(level) == 0)
        {
            logger_func_set__[level] = std::unordered_map<int,std::function<void(const logger_info_t__ &)>>();
        }
        auto key = make_random_logger_id__();
        logger_func_set__[level][key] = [=](const logger_info_t__ &log_info)
        {
            *os<<format(format_str, log_info);
        };
        return key;
    }

    template<typename _Base>
    int sf_logger__<_Base>::add_level_file(SF_LOG_LEVEL level, const std::string &filename, std::string format_str) {
        std::shared_ptr<std::ofstream> ofs = std::make_shared<std::ofstream>(filename, std::ios::app);
        if(*ofs)
        {
            std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
            if (logger_func_set__.count(level) == 0)
            {
                logger_func_set__[level] = std::unordered_map<int,std::function<void(const logger_info_t__ &)>>();
            }

            auto key = make_random_logger_id__();
            logger_func_set__[level][key] = [=](const logger_info_t__ &log_info){
                *ofs<<format(format_str, log_info);
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
        logger_info_t__ log_info;
        log_info.level = level;
        log_info.file = file;
        log_info.line = line;
        log_info.thread_id = std::this_thread::get_id();
        log_info.time = make_time_str__();
        log_info.func = func;
        std::ostringstream oss;
        logout__(oss, log_info, dt);
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
                            {
                                std::deque<logger_info_t__> tmp_info;
                                {
                                    std::unique_lock<std::mutex> de_lck(deque_mu__);
                                    tmp_info=std::move(log_deque__);
                                    log_deque__.clear();
                                }
                                for (auto &log: tmp_info)
                                {
                                    std::unique_lock<std::recursive_mutex> lock(func_set_mutex__);
                                    for (auto &level_func:logger_func_set__)
                                    {
                                        if (log.level >= level_func.first)
                                        {
                                            for (auto &func:level_func.second)
                                            {
                                                func.second(log);
                                            }
                                        }
                                    }
                                }
                            }
                            if(!run__){
                                run__ = true;
                                break;
                            }
                        }
                        std::cout<<"thread exit"<<std::endl;
                    }).detach();
    }

    template<typename _Base>
    template<typename T, typename... U>
    void sf_logger__<_Base>::logout__(std::ostringstream &oss, logger_info_t__ &log_info, const T &tmp, const U &... tmp2) {
        oss << "[" << tmp << "]";
        logout__(oss, log_info, tmp2...);
    }

    template<typename _Base>
    template<typename T>
    void sf_logger__<_Base>::logout__(std::ostringstream &oss, logger_info_t__ &log_info, const T &tmp) {
        oss << "[" << tmp << "]";
        log_info.msg = oss.str();
        {
            std::unique_lock<std::mutex> lck(deque_mu__);
            log_deque__.push_back(log_info);
        }
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
        std::ostringstream os;
        os << std::put_time(ptm, "%Y-%m-%d %H:%M:%S");
        return os.str();
    }

    template<typename _Base>
    template<typename... T>
    void sf_logger__<_Base>::logout(SF_LOG_LEVEL level, const std::string &file, int line, const std::string &func,
                                    const T &... dt) {
        logger_info_t__ log_info;
        log_info.level = level;
        log_info.file = file;
        log_info.line = line;
        log_info.thread_id = std::this_thread::get_id();
        log_info.time = make_time_str__();
        log_info.func = func;
        std::ostringstream oss;
        logout__(oss, log_info, dt...);
    }

    template<typename _Base>
    void sf_logger__<_Base>::stop_logger()
    {
        run__ = false;
        cond__.notify_all();
    }


    namespace {
        auto g_logger = sf_logger::get_instance();
    }

#ifdef QT_CORE_LIB
    template<typename _Base>
    void sf_logger__<_Base>::logout__(std::ostringstream &oss, logger_info_t__ &log_info, const QString &tmp)
    {
        oss << "[" <<  tmp.toStdString() << "]";
        log_info.msg = oss.str();
        {
            std::unique_lock<std::mutex> lck(deque_mu__);
            log_deque__.push_back(log_info);
        }
        cond__.notify_one();
    }

    template<typename _Base>
    template<typename...U>
    void sf_logger__<_Base>::logout__(std::ostringstream &oss, logger_info_t__ &log_info, const QString &tmp, const U &...tmp2)
    {
        oss << "[" << tmp.toStdString() << "]";
        logout__(oss, log_info, tmp2...);
    }

#endif

    template <typename _Base>
    inline std::string sf_logger__<_Base>::format(std::string format_str, const logger_info_t__& log_info)
    {
        auto replace = [](std::string& str,const std::string &from, const std::string &to)
        {
            size_t start_pos = 0;
            while((start_pos = str.find(from, start_pos)) != std::string::npos) {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length();
            }
        };
        auto thread_to_str=[](std::thread::id id){
            std::ostringstream oss;
            oss<<id;
            return oss.str();
        };
        replace(format_str,"{func}",log_info.func);
        replace(format_str,"{time}",log_info.time);
        replace(format_str,"{thread}",thread_to_str(log_info.thread_id));
        replace(format_str,"{line}",std::to_string(log_info.line));
        replace(format_str,"{file}",log_info.file);
        replace(format_str,"{level}",logger_level_str__[log_info.level]);
        replace(format_str,"{msg}",log_info.msg);
        return format_str;
    }

}
