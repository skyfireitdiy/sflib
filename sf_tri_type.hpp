
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tri_type.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_tri_type tri类型
 */

#pragma once

#include "sf_tri_type.h"

namespace skyfire
{
	template<typename T>
	sf_tri_type<T>::sf_tri_type() {}

	template<typename T>
	template<typename... Args>
	sf_tri_type<T>::sf_tri_type(Args... a):data__(a...)
	{

	}

	template<typename T>
	sf_tri_type<T> &sf_tri_type<T>::operator=(const T &other) {
		if (&other == &data__)
			return *this;
		init__ = true;
		data__ = other;
		return *this;
	}

	template<typename T>
	T &sf_tri_type<T>::operator*() {
		return data__;
	}

	template<typename T>
	sf_tri_type<T>::operator bool() const {
		return init__;
	}

	inline sf_tri_type<void>::sf_tri_type(bool init) : init__(init)
	{}

	inline sf_tri_type<void>::operator bool() const {
		return true;
	}

}
