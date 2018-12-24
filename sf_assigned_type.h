
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_assigned_type.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

/*
 * sf_assigned_type tri类型
 */

#pragma once

namespace skyfire
{
	/**
	 * tri类型
	 * @tparam T 包装类型
	 */
	template<typename T>
	class sf_assigned_type
	{
	public:
        /**
         * @brief sf_assigned_type 创建tri类型
         */
		sf_assigned_type();

        /**
         * @brief sf_assigned_type 构造函数
         * @param a 要保存的对象
         */
		template<typename ... Args>
		explicit sf_assigned_type(Args...a);

        /**
         * @brief operator = 赋值运算符
         * @param other 其他类型
         * @return sf_assigned_type对象
         */
		sf_assigned_type& operator=(const T &other);

        /**
         * @brief operator * 解引用操作符
         * @return 类型对象
         */
		T& operator*();

        /**
         * 解引用操作符
         * @return 类型对象指针
         */
        T *operator->();

        /**
         * @brief operator bool 判断是否赋值
         */
		operator bool() const;
	private:
		T data__;
		bool init__{ false };
	};


	template <>
	class sf_assigned_type<void>
	{
	public:
		sf_assigned_type(bool init = false);
		operator bool() const;

	private:
		bool init__{false};
	};


}
