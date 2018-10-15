/*
 * sf_single_instance单例支持
 */

#pragma once
#include <mutex>
#include "sf_empty_class.hpp"


namespace skyfire
{

	template<typename ThisClass,typename BaseClass=sf_empty_class>
	class sf_single_instance:public BaseClass
	{
	public:

        /**
         * @brief get_instance 获取单例对象
         * @return
         */
		static ThisClass* get_instance();

		~sf_single_instance();

	private:
		sf_single_instance() = default;
		static ThisClass * instance;
		static std::mutex mu;
		friend ThisClass;
	};

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

	template<typename ThisClass, typename BaseClass>
	ThisClass* sf_single_instance<ThisClass, BaseClass>::instance = nullptr;
	template<typename ThisClass, typename BaseClass>
	std::mutex sf_single_instance<ThisClass, BaseClass>::mu;
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


