/**
 * @file	sf_watch_time.h
 *
 * @brief	Declares the sf watch time class.
 */

#pragma once
#include <map>
#include <thread>
#include <iostream>
#include <iomanip>
#include <string>

namespace skyfire
{
	template<typename>
	class check_point;

	/**
	 * @class	sf_watch_time
	 *
	 * @brief	A sf watch time.
	 *
	 * @author	Sky Fire
	 * @date	2017/8/18
	 *
	 * @tparam	T	Generic type parameter.
	 */

	template<typename T=std::string>
	class sf_watch_time
	{
	public:

		/**
		 * @fn	sf_watch_time::~sf_watch_time()
		 *
		 * @brief	Destructor. Output this result
		 *
		 * @author	Sky Fire
		 * @date	2017/8/18
		 */

		~sf_watch_time()
		{
			auto old = std::cout.flags();

			std::cout << std::left;
			std::cout << std::setw(30) << "Points" << std::setw(6) << "|" << std::setw(12) << "Thread" << std::setw(6) << "|" << std::setw(12) << "Time" << std::endl;
			std::cout << "--------------------------------------------------------------------" << std::endl;

			for(auto &p:data__)
			{
				for(auto &q:p.second)
				{
					std::cout << std::setw(30) << p.first << std::setw(6) << "|" << std::setw(12) << q.first << std::setw(6) << "|" << std::setw(12) << convert_ms_to_readable__(q.second) << std::endl;
				}
				std::cout << "--------------------------------------------------------------------" << std::endl;
			}
			std::cout.flags(old);
		}
	private:
		std::map<T, std::map<std::thread::id, long long>> data__;
		std::string convert_ms_to_readable__(long long time)
		{
			std::string ret;
			const long long ns = 1; 
			const long long us = 1000;
			const auto ms = 1000 * us;
			const auto s = 1000 * ms;
			const auto m = 60 * s;
			const auto h = 60 * m;
			const auto d = 24 * h;

			if(time>=d)
			{
				ret += std::to_string(time / d) + "d";
				time %= d;
			}
			if (time >= h)
			{
				ret += std::to_string(time / h) + "h";
				time %= h;
			}
			if (time >= m)
			{
				ret += std::to_string(time / m) + "m";
				time %= m;
			}
			if (time >= s)
			{
				ret += std::to_string(time / s) + "s";
				time %= s;
			}
			if (time >= ms)
			{
				ret += std::to_string(time / ms) + "ms";
				time %= ms;
			}
			if (time >= us)
			{
				ret += std::to_string(time / us) + "us";
				time %= us;
			}
			if (time >= ns)
			{
				ret += std::to_string(time / ns) + "ns";
			}

			return ret;
		}

		friend class check_point<T>;
	};

	/**
	 * @class	check_point
	 *
	 * @brief	A check point.
	 *
	 * @author	Sky Fire
	 * @date	2017/8/18
	 *
	 * @tparam	T	Generic type parameter.
	 */

	template<typename T=std::string>
	class check_point
	{
	private:
		T point_name__;
		std::chrono::high_resolution_clock::time_point clock__;
		sf_watch_time<T> &parent__;
	public:

		/**
		 * @fn	check_point::check_point(const T &name, sf_watch_time<T> &parent) : point_name__(name), clock__(std::chrono::high_resolution_clock::now()), parent__(parent)
		 *
		 * @brief	Constructor.
		 *
		 * @author	Sky Fire
		 * @date	2017/8/18
		 *
		 * @param 		  	name  	The check point name.
		 * @param [in,out]	parent	The parent.
		 */

		check_point(const T &name, sf_watch_time<T> &parent) : 
			point_name__(name), clock__(std::chrono::high_resolution_clock::now()),
			parent__(parent)
		{}

		/**
		 * @fn	check_point::~check_point()
		 *
		 * @brief	Destructor.
		 *
		 * @author	Sky Fire
		 * @date	2017/8/18
		 */

		~check_point()
		{
			parent__.data__[point_name__][std::this_thread::get_id()] += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - clock__).count();
		}
	};
}


