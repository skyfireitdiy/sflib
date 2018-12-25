
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_assigned_type.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_assigned_type tri类型
 */

#pragma once

#include "sf_assigned_type.h"

namespace skyfire
{

	template<typename T>
	template<typename... Args>
	sf_assigned_type<T>::sf_assigned_type(Args... a):data__(a...)
	{

	}

	template<typename T>
	sf_assigned_type<T> &sf_assigned_type<T>::operator=(const T &other) {
		if (&other == &data__)
			return *this;
		init__ = true;
		data__ = other;
		return *this;
	}

	template<typename T>
	T &sf_assigned_type<T>::operator*() {
		return data__;
	}

	template<typename T>
	sf_assigned_type<T>::operator bool() const {
		return init__;
	}

    template<typename T>
    T *sf_assigned_type<T>::operator->()
    {
        return &data__;
    }

    inline sf_assigned_type<void>::sf_assigned_type(const bool init) : init__(init)
	{}

	inline sf_assigned_type<void>::operator bool() const {
		return true;
	}

}
