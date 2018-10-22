
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_watch_time.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_watch_time 时间监测类型
 */

#pragma once
#include <map>
#include <thread>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

namespace skyfire
{
	template<typename>
	class check_point;


	template<typename T=std::string>
	class sf_watch_time
	{
	public:

		void watch();

	protected:
		std::string to_string();

		std::map<T, std::map<std::thread::id, long long>> data__;
		std::string convert_ms_to_readable__(long long time) const;

		friend class check_point<T>;
	};

	template<typename T=std::string>
	class check_point
	{
	private:
		T point_name__;
		std::chrono::high_resolution_clock::time_point clock__;
		sf_watch_time<T> &parent__;
	public:

        /**
         * @brief check_point 设置检查点
         * @param name 检查点名称
         * @param parent 对象
         */
		check_point(const T &name, sf_watch_time<T> &parent);

		~check_point();
	};
}


