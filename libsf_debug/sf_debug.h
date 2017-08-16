/// @file	sf_debug.h
///
/// @brief	Declares the sf debug class.

#pragma once
#include <fstream>
#include <sstream>
#include <mutex>
#include <map>
#include <ctime>
#include <iomanip>
#include <functional>
#include <thread>
#include "../libsfempty_class/sfempty_class.h"
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

namespace skyfire
{
	/// @brief	The sf debug version[].
	constexpr char sf_debug_version[] = { "1.0.0.1" };

	/// @class	sf_debug
	///
	/// @brief	提供基类链，提供默认sf_empty_class作为模板参数，方便实例化.
	///
	/// @author	SkyFire
	/// @date	2017/8/16
	///
	/// @tparam	_Caller_Class	Type of the caller class.
	/// @tparam	_Base_Class  	Type of the base class.

	template<typename _Caller_Class = sf_empty_class, typename _Base_Class = sf_empty_class>
	class sf_debug :public _Base_Class
	{
	public:

		/// @fn	explicit sf_debug::sf_debug(const std::string& debug_file, std::function<bool()> func_log_to_file = if_output_to_file__);
		///
		/// @brief	Constructor.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param	debug_file			The debug file.
		/// @param	func_log_to_file	(Optional) The function log to file.

		explicit sf_debug(const std::string& debug_file, std::function<bool()> func_log_to_file = if_output_to_file__);

		/// @fn	explicit sf_debug::sf_debug(std::function<bool()> func_log_to_file = if_output_to_file__);
		///
		/// @brief	Constructor.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param	func_log_to_file	(Optional) The function log to file.

		explicit sf_debug(std::function<bool()> func_log_to_file = if_output_to_file__);

		/// @fn	virtual sf_debug::~sf_debug();
		///
		/// @brief	Destructor.
		///
		/// @author	SkyFire
		/// @date	2017/8/16

		virtual ~sf_debug();

		/// @fn	void sf_debug::set_debug_file(const std::string& debug_file);
		///
		/// @brief	Sets debug file.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param	debug_file	The debug file.

		void set_debug_file(const std::string& debug_file);

		/// @fn	void sf_debug::set_out_to_file(bool enable);
		///
		/// @brief	Sets out to file.
		///
		/// @author	SkyFire
		/// @date	2017/8/16
		///
		/// @param	enable	True to enable, false to disable.

		void set_out_to_file(bool enable);

		/// @brief	以下两个模板函数是为了让IDE自动完成，配合后面的宏实现.

		template<typename...T>
		static void sf_logout(T...){}
		template<typename...T>
		static void sf_track(T...){}
		template<typename T>
		void logout(const std::string& file, int line, const std::string& func, const T& dt);
#ifdef QT_CORE_LIB
		template<>
		void logout<QString>(const std::string& file, int line, const std::string& func, const QString& dt)
		{
			if (!if_out_to_file__ || !fp__.is_open())
				return;
			std::lock_guard<std::mutex> lck(file_mutex__[curr_file__]);
			fp__ << "[" << make_time_str__() << "][" << std::this_thread::get_id() << "][" << file << "][" << line << "][" << func << "][" << dt.toStdString() << "]" << std::endl;
		}
#endif // QT_CORE_LIB

		template<typename...T>
		void logout(const std::string& file, int line, const std::string& func, const T&...dt);
		template<typename T>
		void track(const std::string& file, int line, const std::string& func, const T& dt);
		template<typename...T>
		void track(const std::string& file, int line, const std::string& func, const T&...dt);
	protected:
		virtual void out_debug_string_(const char* str);

	private:
		std::string curr_file__;
		std::ofstream fp__;
		std::ostringstream os__;
		static std::map<std::string, std::mutex> file_mutex__;
		std::mutex __debug_mutex;
		volatile bool if_out_to_file__;

		template<typename T, typename...U>
		void logout__(const T& tmp, const U&...tmp2);
		template<typename T>
		void logout__(const T& tmp);
#ifdef QT_CORE_LIB
		template<typename...U>
		void logout__(const QString & tmp, const U&...tmp2)
		{
			{
				std::lock_guard<std::mutex> lck(file_mutex__[curr_file__]);
				fp__ << "[" << tmp.toLocal8Bit().data() << "]" << std::flush;
			}
			logout__(tmp2...);
		}
		template<>
		void logout__(const QString& tmp)
		{
			fp__ << "[" << tmp.toLocal8Bit().data() << "]" << std::endl;
		}

#endif

		template<typename T, typename...U>
		void track__(const T& tmp, const U&...tmp2);
		template<typename T>
		void track__(const T& tmp);

#ifdef QT_CORE_LIB
		template<typename...U>
		void track__(const QString& tmp, const U&...tmp2)
		{
			os__ << "[" << tmp.toLocal8Bit().data() << "]";
			track__(tmp2...);
		}

		template<>
		void track__(const QString& tmp)
		{
			os__ << "[" << tmp.toLocal8Bit().data() << "]";
			{
				std::lock_guard<std::mutex> lck(__debug_mutex);
				out_debug_string_(os__.str().c_str());
			}
			os__ = std::ostringstream();
		}

#endif

		void write_begin_info__();
		static std::string make_time_str__();
		static bool if_output_to_file__();
	};



	template <typename _Caller_Class, typename _Base_Class>
	sf_debug<_Caller_Class, _Base_Class>::sf_debug(const std::string& debug_file, std::function<bool()> func_log_to_file):
		curr_file__(debug_file)
	{
		if_out_to_file__ = func_log_to_file();
		if (!if_out_to_file__)
			return;
		fp__.open(debug_file, std::ios::out | std::ios::app);
	}

	template <typename _Caller_Class, typename _Base_Class>
	sf_debug<_Caller_Class, _Base_Class>::sf_debug(std::function<bool()> func_log_to_file)
	{
		if_out_to_file__ = func_log_to_file();
	}

	template <typename _Caller_Class, typename _Base_Class>
	sf_debug<_Caller_Class, _Base_Class>::~sf_debug()
	{
		if (fp__.is_open())
			fp__.close();
	}

	template <typename _Caller_Class, typename _Base_Class>
	void sf_debug<_Caller_Class, _Base_Class>::set_debug_file(const std::string& debug_file)
	{
		curr_file__ = debug_file;
		if (fp__.is_open())
			fp__.close();
		if (!if_out_to_file__)
			return;
		fp__.open(debug_file, std::ios::out | std::ios::app);
	}

	template <typename _Caller_Class, typename _Base_Class>
	void sf_debug<_Caller_Class, _Base_Class>::set_out_to_file(bool enable)
	{
		if_out_to_file__ = enable;
	}

	template <typename _Caller_Class, typename _Base_Class>
	template <typename T>
	void sf_debug<_Caller_Class, _Base_Class>::logout(const std::string& file, int line, const std::string& func, const T& dt)
	{
		if (!if_out_to_file__ || !fp__.is_open())
			return;
		std::lock_guard<std::mutex> lck(file_mutex__[curr_file__]);
		fp__ << "[" << make_time_str__() << "][" << std::this_thread::get_id() << "][" << file << "][" << line << "][" << func << "][" << dt << "]" << std::endl;
	}

	template <typename _Caller_Class, typename _Base_Class>
	template <typename ... T>
	void sf_debug<_Caller_Class, _Base_Class>::logout(const std::string& file, int line, const std::string& func, const T&... dt)
	{
		if (!if_out_to_file__ || !fp__.is_open())
			return;
		{
			std::lock_guard<std::mutex> lck(file_mutex__[curr_file__]);
			fp__ << "[" << make_time_str__() << "]" << "[" << file << "][" << line << "][" << func << "]" << std::flush;
		}
		logout__(dt...);
	}

	template <typename _Caller_Class, typename _Base_Class>
	template <typename T>
	void sf_debug<_Caller_Class, _Base_Class>::track(const std::string& file, int line, const std::string& func, const T& dt)
	{
		os__ << "[" << make_time_str__() << "]" << "[" << file << "][" << line << "][" << func << "]";
		track__(dt);
	}

	template <typename _Caller_Class, typename _Base_Class>
	template <typename ... T>
	void sf_debug<_Caller_Class, _Base_Class>::track(const std::string& file, int line, const std::string& func, const T&... dt)
	{
		os__ << "[" << make_time_str__() << "]" << "[" << file << "][" << line << "][" << func << "]";
		track__(dt...);
	}

	template <typename _Caller_Class, typename _Base_Class>
	void sf_debug<_Caller_Class, _Base_Class>::out_debug_string_(const char* str)
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

	template <typename _Caller_Class, typename _Base_Class>
	template <typename T, typename ... U>
	void sf_debug<_Caller_Class, _Base_Class>::logout__(const T& tmp, const U&... tmp2)
	{
		{
			std::lock_guard<std::mutex> lck(file_mutex__[curr_file__]);
			fp__ << "[" << tmp << "]" << std::flush;
		}
		logout__(tmp2...);
	}

	template <typename _Caller_Class, typename _Base_Class>
	template <typename T>
	void sf_debug<_Caller_Class, _Base_Class>::logout__(const T& tmp)
	{
		fp__ << "[" << tmp << "]" << std::endl;
	}

	template <typename _Caller_Class, typename _Base_Class>
	template <typename T, typename ... U>
	void sf_debug<_Caller_Class, _Base_Class>::track__(const T& tmp, const U&... tmp2)
	{
		os__ << "[" << tmp << "]";
		track__(tmp2...);
	}

	template <typename _Caller_Class, typename _Base_Class>
	template <typename T>
	void sf_debug<_Caller_Class, _Base_Class>::track__(const T& tmp)
	{
		os__ << "[" << tmp << "]";
		{
			std::lock_guard<std::mutex> lck(__debug_mutex);
			out_debug_string_(os__.str().c_str());
		}
		os__ = std::ostringstream();
	}

	template <typename _Caller_Class, typename _Base_Class>
	void sf_debug<_Caller_Class, _Base_Class>::write_begin_info__()
	{
		if (!if_out_to_file__ || !fp__.is_open())
			return;
		std::lock_guard<std::mutex> lck(file_mutex__[curr_file__]);
		fp__ << "Welcome to SkyFire debug, version: " << sf_debug_version << std::endl;
	}

	template <typename _Caller_Class, typename _Base_Class>
	std::string sf_debug<_Caller_Class, _Base_Class>::make_time_str__()
	{
		auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		tm tm_d;
		tm* ptm = &tm_d;
		localtime_s(ptm, &tt);
#if 0
		//sprint_f无法完成格式化，而sprintf可以，Why？
			char tmp[60] = { 0 };
			sprintf_s(tmp, "%04d-%02d-%02d %02d:%02d:%02d",
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

	template <typename _Caller_Class, typename _Base_Class>
	bool sf_debug<_Caller_Class, _Base_Class>::if_output_to_file__()
	{
#ifdef QT_CORE_LIB
			QSettings st("HKEY_LOCAL_MACHINE\\SOFTWARE\\SkyFire", QSettings::NativeFormat);
			return st.value("debug").toBool();
#else
		return false;
#endif
	}

/// @def	sf_logout(...);
///
/// @brief	声明两个宏作为调用接口,覆盖类中的两个未定义的模板成员函数.
///
/// @author	SkyFire
/// @date	2017/8/16
///
/// @param	...	Variable arguments providing additional information.

#define sf_logout(...) logout(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define sf_track(...) track(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)

	template<typename T, typename B>
	std::map<std::string, std::mutex> sf_debug<T, B>::file_mutex__;
}