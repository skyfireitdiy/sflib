
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_range.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_range range产生序列的功能
 */

#pragma once

#include "sf_range.h"

namespace skyfire
{

	template<typename T>
	sf_range_iteator__<T>::sf_range_iteator__(sf_range_iteator__::size_type start, const value_type &value,
											  const value_type &step):
			cursor__(start),value__(value),step__(step)
	{
		value__ += (step__*cursor__);
	}

	template<typename T>
	typename sf_range_iteator__<T>::value_type sf_range_iteator__<T>::operator*() const {
		return value__;
	}

	template<typename T>
	bool sf_range_iteator__<T>::operator!=(const sf_range_iteator__<T> &other) const {
		return cursor__ != other.cursor__;
	}

	template<typename T>
	sf_range_iteator__<T> &sf_range_iteator__<T>::operator++() {
		value__ += step__;
		++cursor__;
		return *this;
	}

	template<typename T>
	typename sf_range_impl__<T>::size_type sf_range_impl__<T>::get_max_count__() const {
		if(step__>0&&begin__>=end__)
		{
			throw std::logic_error("End value must be greater than begin value");
		}
		if(step__<0&&begin__<=end__)
		{
			throw std::logic_error("End value must be less than begin value");
		}
		if(step__==0)
		{
			throw std::logic_error("Step can't be zero");
		}
		auto x = static_cast<size_type>((end__ - begin__) / step__);
		if(begin__+(x*step__)!=end__)
		{
			++x;
		}
		return x;
	}

	template<typename T>
	typename sf_range_impl__<T>::const_iterator sf_range_impl__<T>::end() const {
		return { max_count__,begin__,step__ };
	}

	template<typename T>
	typename sf_range_impl__<T>::const_iterator sf_range_impl__<T>::begin() const {
		return { 0, begin__, step__};
	}

	template<typename T>
	typename sf_range_impl__<T>::size_type sf_range_impl__<T>::size() const {
		return max_count__;
	}

	template<typename T>
	sf_range_impl__<T>::sf_range_impl__(const value_type vbegin, const value_type vend, const value_type vstep):
			begin__(vbegin),end__(vend),step__(vstep),max_count__(get_max_count__())
	{

	}


	template<typename T>
	sf_range_impl__<T> sf_range(const T &end)
	{
		return { {},end,1 };
	}

	template<typename T>
	sf_range_impl__<T> sf_range(const T &begin, const T &end)
	{
		return { begin,end,1 };
	}

	template<typename T>
	auto sf_range(const T &begin, const T &end, const T &step)
	{
		return sf_range_impl__<decltype(begin + step)>{begin, end, step};
	}

}
