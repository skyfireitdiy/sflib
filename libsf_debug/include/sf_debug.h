#pragma once
#include <fstream>
#include <sstream>
#include <mutex>
#include <map>
#include <ctime>
#include <iomanip>
#include <functional>
#include <thread>
#ifdef QT_CORE_LIB
#include <QDebug>
#include <QSettings>
#else
#ifdef _WIN32
#include <windows.h>
#else
#include <iostream>
#endif
#endif
#include "sf_empty_class.h"

namespace skyfire
{
    constexpr char sf_debug_version[] = {"1.0.0.1"};


    inline void __out_debug_string__(const char *str)
    {
#ifdef QT_CORE_LIB
        qDebug() << str;
#else
#ifdef _WIN32
        OutputDebugStringA(str);
#else
        std::cout << str << std::endl;
#endif
#endif
    }


//提供基类链，提供默认sf_empty_class作为模板参数，方便实例化
    template<typename _Base_Class = sf_empty_class>
    class sf_debug : public _Base_Class
    {
    public:
        explicit sf_debug(const std::string &debug_file, std::function<bool()> func_log_to_file = if_output_to_file__) :
                curr_file__(debug_file)
        {
            if_out_to_file__ = func_log_to_file();
            if (!if_out_to_file__)
                return;
            fp__.open(debug_file, std::ios::out | std::ios::app);
        }

        explicit sf_debug(std::function<bool()> func_log_to_file = if_output_to_file__)
        {
            if_out_to_file__ = func_log_to_file();
        }

        virtual ~sf_debug()
        {
            if (fp__.is_open())
                fp__.close();
            delete p_os__;
        }

        void set_debug_file(const std::string &debug_file)
        {
            curr_file__ = debug_file;
            if (fp__.is_open())
                fp__.close();
            if (!if_out_to_file__)
                return;
            fp__.open(debug_file, std::ios::out | std::ios::app);
        }

        void set_out_to_file(bool enable)
        {
            if_out_to_file__ = enable;
        }

        //以下两个模板函数是为了让IDE自动完成，配合后面的宏实现
        template<typename...T>
        static void sf_logout(T...)
        {}

        template<typename...T>
        static void sf_track(T...)
        {}

        template<typename T>
        void logout(const std::string &file, int line, const std::string &func, const T &dt)
        {
            if (!if_out_to_file__ || !fp__.is_open())
                return;
            std::lock_guard<std::recursive_mutex> lck(file_mutex__[curr_file__]);
            fp__ << "[" << make_time_str__() << "][" << std::this_thread::get_id() << "][" << file << "][" << line
                 << "][" << func << "][" << dt << "]" << std::endl;
        }

#ifdef QT_CORE_LIB
        void logout(const std::string& file, int line, const std::string& func, const QString& dt)
        {
            if (!if_out_to_file__ || !fp__.is_open())
                return;
            std::lock_guard<std::recursive_mutex> lck(file_mutex__[curr_file__]);
            fp__ << "[" << make_time_str__() << "][" << std::this_thread::get_id() << "][" << file << "][" << line << "][" << func << "][" << dt.toLocal8Bit().data() << "]" << std::endl;
        }
#endif // QT_CORE_LIB

        template<typename...T>
        void logout(const std::string &file, int line, const std::string &func, const T &...dt)
        {
            if (!if_out_to_file__ || !fp__.is_open())
                return;
            {
                std::lock_guard<std::recursive_mutex> lck(file_mutex__[curr_file__]);
                fp__ << "[" << make_time_str__() << "][" << std::this_thread::get_id() << "][" << file << "][" << line
                     << "][" << func << "]" << std::flush;
            }
            logout__(dt...);
        }

        template<typename T>
        static void track(const std::string &file, int line, const std::string &func, const T &dt)
        {
            std::lock_guard<std::recursive_mutex> lck(__debug_mutex);
            *p_os__ << "[" << make_time_str__() << "][" << std::this_thread::get_id() << "][" << file << "][" << line
                    << "][" << func << "]";
            track__(dt);
        }

        template<typename...T>
        static void track(const std::string &file, int line, const std::string &func, const T &...dt)
        {
            std::lock_guard<std::recursive_mutex> lck(__debug_mutex);
            *p_os__ << "[" << make_time_str__() << "][" << std::this_thread::get_id() << "][" << file << "][" << line
                    << "][" << func << "]";
            track__(dt...);
        }

        static void set_output_debug_string_func(std::function<void(const char*)> f)
        {
            out_debug_str_func__ = f;
        }

        template<typename...T>
        static void __empty_func__(T...){}

    private:
        std::string curr_file__;
        std::ofstream fp__;
        static std::ostringstream *p_os__;
        static std::map<std::string, std::recursive_mutex> file_mutex__;
        static std::recursive_mutex __debug_mutex;
        static std::function<void(const char*)> out_debug_str_func__;
        volatile bool if_out_to_file__;

        template<typename T, typename...U>
        void logout__(const T &tmp, const U &...tmp2)
        {
            {
                std::lock_guard<std::recursive_mutex> lck(file_mutex__[curr_file__]);
                fp__ << "[" << tmp << "]" << std::flush;
            }
            logout__(tmp2...);
        }

        template<typename T>
        void logout__(const T &tmp)
        {
            std::lock_guard<std::recursive_mutex> lck(file_mutex__[curr_file__]);
            fp__ << "[" << tmp << "]" << std::endl;
        }

#ifdef QT_CORE_LIB
        template<typename...U>
        void logout__(const QString & tmp, const U&...tmp2)
        {
            {
                std::lock_guard<std::recursive_mutex> lck(file_mutex__[curr_file__]);
                fp__ << "[" << tmp.toLocal8Bit().data() << "]" << std::flush;
            }
            logout__(tmp2...);
        }

        void logout__(const QString& tmp)
        {
            std::lock_guard<std::recursive_mutex> lck(file_mutex__[curr_file__]);
            fp__ << "[" << tmp.toLocal8Bit().data() << "]" << std::endl;
        }

#endif

        template<typename T, typename...U>
        static void track__(const T &tmp, const U &...tmp2)
        {
            std::lock_guard<std::recursive_mutex> lck(__debug_mutex);
            *p_os__ << "[" << tmp << "]";
            track__(tmp2...);
        }

        template<typename T>
        static void track__(const T &tmp)
        {
            std::lock_guard<std::recursive_mutex> lck(__debug_mutex);
            *p_os__ << "[" << tmp << "]";
            out_debug_str_func__(p_os__->str().c_str());
            delete p_os__;
            p_os__ = new std::ostringstream();
        }

#ifdef QT_CORE_LIB
        template<typename...U>
        static void track__(const QString& tmp, const U&...tmp2)
        {
            std::lock_guard<std::recursive_mutex> lck(__debug_mutex);
            *p_os__ << "[" << tmp.toLocal8Bit().data() << "]";
            track__(tmp2...);
        }


        static void track__(const QString& tmp) {
            std::lock_guard<std::recursive_mutex> lck(__debug_mutex);
            *p_os__ << "[" << tmp.toLocal8Bit().data() << "]";
            out_debug_str_func__(p_os__->str().c_str());
            delete p_os__;
            p_os__ = new std::ostringstream();
        }

#endif

        void write_begin_info__()
        {
            if (!if_out_to_file__ || !fp__.is_open())
                return;
            std::lock_guard<std::recursive_mutex> lck(file_mutex__[curr_file__]);
            fp__ << "Welcome to SkyFire debug, version: " << sf_debug_version << std::endl;
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
#if 1
            //sprint_f无法完成格式化，而sprintf可以，Why？
            char tmp[60] = {0};
            sprintf(tmp, "%04d-%02d-%02d %02d:%02d:%02d",
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

        static bool if_output_to_file__()
        {
#ifdef QT_CORE_LIB
            QSettings st("HKEY_LOCAL_MACHINE\\SOFTWARE\\SkyFire", QSettings::NativeFormat);
            return st.value("debug").toBool();
#else
            return false;
#endif
        }
    };

//声明两个宏作为调用接口,覆盖类中的两个未定义的模板成员函数
#define sf_logout(...) logout(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)

#ifndef SF_DISABLE_DEBUG
#define sf_log(...) skyfire::sf_debug<skyfire::sf_empty_class>::track(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define sf_track(...) track(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#else
#define sf_log(...) skyfire::sf_debug<skyfire::sf_empty_class>::__empty_func__(__VA_ARGS__)
#define sf_track(...) __empty_func__(__VA_ARGS__)
#endif

    template<typename T>
    std::map<std::string, std::recursive_mutex> sf_debug<T>::file_mutex__;

    template <typename T>
    std::ostringstream * sf_debug<T>::p_os__= new std::ostringstream();

    template <typename T>
    std::recursive_mutex sf_debug<T>::__debug_mutex;

    template <typename T>
    std::function<void(const char*)> sf_debug<T>::out_debug_str_func__ = __out_debug_string__;

}
