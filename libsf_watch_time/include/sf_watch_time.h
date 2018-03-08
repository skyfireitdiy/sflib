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
#include <sstream>

namespace skyfire
{
	template<typename>
	class check_point;

	/**
	 * @class	sf_watch_time
	 *
	 * @brief	A sf watch time.
	 *
	 * @author	SkyFire
	 * @date	2017/8/18
	 *
	 * @tparam	T	Generic type parameter.
	 */

	template<typename T=std::string>
	class sf_watch_time
	{
	public:
		/**
		 * @fn	void sf_watch_time::watch()
		 *
		 * @brief	Output this result.
		 *
		 * @author	SkyFire
		 * @date	2017/8/18
		 */

	
		void watch()
		{
			std::cout << to_string() << std::flush;
		}

	protected:
		std::string to_string()
		{
			std::ostringstream so;
			auto old = so.flags();

			so << std::right;
			so << std::setw(55) << "SkyFire Time Watch" << std::endl;;
			so << "=========================================================================================" << std::endl;
			so << std::left;
			so << std::setw(30) << "Points" << std::setw(6) << "|" << std::setw(12) << "Thread" << std::setw(20) << "|" << "Time" << std::endl;
			so << "=========================================================================================" << std::endl;

			for (auto &p : data__)
			{
				for (auto &q : p.second)
				{
					so << std::setw(30) << p.first << std::setw(6) << "|" << std::setw(12) << q.first << std::setw(6) << "|" << convert_ms_to_readable__(q.second) << std::endl;
				}
			}
			so << "-----------------------------------------------------------------------------------------" << std::endl;
			so.flags(old);
			return so.str();
		}

		std::map<T, std::map<std::thread::id, long long>> data__;
		std::string convert_ms_to_readable__(long long time) const
		{
			std::ostringstream so;
			auto old = so.flags();
			so << std::left;
			const long long ns = 1; 
			const long long us = 1000;
			const auto ms = 1000 * us;
			const auto s = 1000 * ms;
			const auto m = 60 * s;
			const auto h = 60 * m;
			const auto d = 24 * h;

			if(time>=d)
			{
				so<<std::to_string(time / d) + "d";
				time %= d;
			}
			else
			{
				so << "";
			}
			so << std::setw(6);
			if (time >= h)
			{
				so<< std::to_string(time / h) + "h";
				time %= h;
			}
			else
			{
				so << "";
			}
			so << std::setw(6);
			if (time >= m)
			{
				so<< std::to_string(time / m) + "m";
				time %= m;
			}
			else
			{
				so << "";
			}
			so << std::setw(6);
			if (time >= s)
			{
				so<< std::to_string(time / s) + "s";
				time %= s;
			}
			else
			{
				so << "";
			}
			so << std::setw(6);
			if (time >= ms)
			{
				so<< std::to_string(time / ms) + "ms";
				time %= ms;
			}
			else
			{
				so << "";
			}
			so << std::setw(6);
			if (time >= us)
			{
				so<< std::to_string(time / us) + "us";
				time %= us;
			}
			else
			{
				so << "";
			}
			so << std::setw(6);
			if (time >= ns)
			{
				so<< std::to_string(time / ns) + "ns";
			}
			so.setf(old);
			return so.str();
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


