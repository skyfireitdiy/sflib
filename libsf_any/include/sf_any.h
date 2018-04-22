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

        template<typename T, class =
                typename std::enable_if<
                        !std::is_same<std::decay<T>, sf_any>::value &&
                        !std::is_pointer<T>::value &&
                        !std::is_reference<T>::value,
                        T
                >::type>
        sf_any(T value):m_ptr(std::make_shared<value_class<T>>(value))
        {}

        sf_any(const sf_any &that) : m_ptr(that.clone())
        {}

        sf_any(sf_any &&that) : m_ptr(std::move(that.m_ptr))
        {}

        template<typename V>
        operator V()
        {
            return dynamic_cast<value_class<V> *>(m_ptr.get())->get();
        }

        template<typename T>
        std::enable_if_t<!std::is_pointer_v<T> && !std::is_reference_v<T>, sf_any>
        operator=(const T &value)
        {
            m_ptr = std::make_shared<value_class<typename std::decay<T>::type>>(value);
            return *this;
        };

        sf_any operator=(const sf_any &that)
        {
            if (this != &that)
            {
                m_ptr = that.clone();
            }
            return *this;
        }

        bool operator==(const sf_any &that) const
        {
            return this == &that;
        }

        template<typename T>
        bool operator==(const T &value)
        {
            return T(*this) == value;
        }

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