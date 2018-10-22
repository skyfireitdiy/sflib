
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_single_instance.h

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


namespace skyfire
{
    /**
     * 单例类
     * @tparam ThisClass 要单例的类
     * @tparam BaseClass 父类
     */
	template<typename ThisClass, typename BaseClass=sf_empty_class>
	class sf_single_instance : public BaseClass
	{
	public:

		/**
         * @brief get_instance 获取单例对象
         * @return
         */
		static ThisClass *get_instance();

		~sf_single_instance();

	private:
		sf_single_instance() = default;

		static ThisClass *instance;
		static std::mutex mu;
		friend ThisClass;
	};

}


/*
 * 侵入式单例宏实现
 */

#define SF_SINGLE_TON(ClassName)								\
ClassName(const ClassName&) = delete;							\
ClassName(ClassName&&) = delete;								\
ClassName& operator=(const ClassName&) = delete;				\
static ClassName* get_instance()								\
{																\
	static std::mutex init_mutex;								\
	static ClassName* instance__{ nullptr };					\
	if(instance__==nullptr){									\
		std::lock_guard<std::mutex> lck(init_mutex);			\
		if (instance__ == nullptr)								\
		{														\
			instance__ = new ClassName;							\
		}														\
	}															\
	return instance__;											\
}																\

