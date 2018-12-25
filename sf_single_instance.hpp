
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_single_instance.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_single_instance单例支持
 */

#pragma once
#include <mutex>
#include "sf_empty_class.hpp"
#include "sf_single_instance.h"

namespace skyfire
{

	template <typename ThisClass, typename BaseClass>
	ThisClass* sf_single_instance<ThisClass, BaseClass>::get_instance()
	{
		if (instance == nullptr)
		{
			std::lock_guard<std::mutex> lck(mu);
			if (instance == nullptr)
			{
				instance = new ThisClass();
			}
		}
		return instance;
	}

	template <typename ThisClass, typename BaseClass>
	sf_single_instance<ThisClass, BaseClass>::~sf_single_instance()
	{
		if (instance != nullptr)
			delete instance;
	}
}


