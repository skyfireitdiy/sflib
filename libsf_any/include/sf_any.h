/*
 * sf_any提供一个通用容器，可以存放任意类型的数据
 */

#pragma once

#include <memory>

namespace skyfire
{
    class sf_any
    {
    private:

        template<typename T>
        class value_class;

    public:
        /**
         * @brief sf_any 构造函数
         * @param value 要存储的zhi值
         */
        template<typename T, class =
                typename std::enable_if<
                        !std::is_same<std::decay<T>, sf_any>::value &&
                        !std::is_pointer<T>::value &&
                        !std::is_reference<T>::value,
                        T
                >::type>
        sf_any(T value):m_ptr(std::make_shared<value_class<T>>(value))
        {}

        /**
         * @brief sf_any 拷贝构造函数
         * @param that 另一个sf_any对象
         */
        sf_any(const sf_any &that) : m_ptr(that.clone())
        {}

        /**
         * @brief sf_any 转移构造函数
         * @param that 另一个sf_any对象
         */
        sf_any(sf_any &&that) : m_ptr(std::move(that.m_ptr))
        {}


        /**
         * @brief operator V 类型转换操作符
         * @return 类型（如果类型不兼容，则抛出转换异常）
         */
        template<typename V>
        operator V()
        {
            return dynamic_cast<value_class<V> *>(m_ptr.get())->get();
        }

        /**
         * @brief operator = 赋值操作符
         * @param value 赋值对象（其他类型的对象）
         * @return sf_any对象
         */
        template<typename T>
        std::enable_if_t<!std::is_pointer_v<T> && !std::is_reference_v<T>, sf_any>
        operator=(const T &value)
        {
            m_ptr = std::make_shared<value_class<typename std::decay<T>::type>>(value);
            return *this;
        }

        /**
         * @brief operator = 赋值操作符
         * @param that 赋值对象（sf_any对象）
         * @return sf_any对象
         */
        sf_any operator=(const sf_any &that)
        {
            if (this != &that)
            {
                m_ptr = that.clone();
            }
            return *this;
        }

        /**
         * @brief operator == 比较操作符
         * @param that 要比较的对象
         * @return 是否相等（必须为相同对象才是相等）
         */
        bool operator==(const sf_any &that) const
        {
            return this == &that;
        }

        /**
         * @brief operator == 比较操作符
         * @param value 要比较的对象
         * @return 是否相等（类型不兼容会抛出异常）
         */
        template<typename T>
        bool operator==(const T &value)
        {
            return T(*this) == value;
        }

        /**
         * @brief is_null 判断是否有值
         * @return 是否有值
         */
        bool is_null()
        {
            return m_ptr == nullptr;
        }

    private:

        class base_class
        {
        public:
            virtual std::shared_ptr<base_class> clone() const = 0;
        };

        template<typename T>
        class value_class : public base_class
        {
        public:
            value_class(T v) : m_value(v)
            {
            }

            T &get()
            {
                return m_value;
            }

            std::shared_ptr<base_class> clone() const override
            {
                return std::shared_ptr<base_class>(new value_class<T>(m_value));
            }

        private:
            T m_value;
        };

        std::shared_ptr<base_class> clone() const
        {
            if (m_ptr != nullptr)
            {
                return m_ptr->clone();
            }
            return nullptr;
        }

        std::shared_ptr<base_class> m_ptr{nullptr};
    };
}
