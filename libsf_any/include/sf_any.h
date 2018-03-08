#pragma once

#include <memory>

namespace skyfire
{
    class sf_any
    {
    public:
        template<typename T,class = typename std::enable_if<!std::is_same<std::decay<T>,sf_any>::value,T>::type>
        sf_any(T value):m_ptr(std::make_shared<ValueClass<T>>(value)){}
        sf_any(const sf_any& that):m_ptr(that.clone()){}
        sf_any(sf_any && that):m_ptr(std::move(that.m_ptr)){}

        template<typename V>
        operator V()
        {
            return dynamic_cast<ValueClass<V>*>(m_ptr.get())->get();
        }

        template<typename T>
        sf_any operator=(const T &value)
        {
            m_ptr = std::make_shared<ValueClass<T>>(value);
            return *this;
        }

        sf_any operator=(const sf_any &that)
        {
            if(this != &that)
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
            return T(*this)==value;
        }

        bool isNull()
        {
            return m_ptr == nullptr;
        }

    private:

        class Base
        {
        public:
            virtual std::shared_ptr<Base> clone() const = 0 ;
        };

        template<typename T>
        class ValueClass: public Base
        {
        public:
            ValueClass(T v):m_value(v)
            {
            }
            T& get()
            {
                return m_value;
            }
            std::shared_ptr<Base> clone() const override
            {
                return std::shared_ptr<Base>(new ValueClass<T>(m_value));
            }
        private:
            T m_value;
        };

        std::shared_ptr<Base> clone() const
        {
            if(m_ptr != nullptr)
            {
                return m_ptr->clone();
            }
            return nullptr;
        }

        std::shared_ptr<Base> m_ptr {nullptr};
    };
}