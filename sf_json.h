#pragma once

#include "sf_json_utils.h"
#include "sf_lex.h"
#include "sf_yacc.h"

namespace skyfire
{
    /**
     * @brief json库
     */
    class sf_json
    {

    private:
        std::shared_ptr<sf_json_value> value__;

        sf_json(const std::shared_ptr<sf_json_value> &value);

    public:

        static std::string json_string_to_string(std::string json_str);

        static std::string string_to_json_string(std::string str);

        sf_json();

        sf_json(const std::string &str);

        template<typename T>
        sf_json(typename std::enable_if<std::is_arithmetic<T>::value, T> number);


        sf_json(const sf_json &json);

        sf_json(bool boolean_value);

        sf_json(sf_json_type_null null_value);


        void convert_to_object();

        void convert_to_array();

        sf_json_type type() const;

        std::string to_string() const;

        static sf_json from_string(const std::string &str);

        const sf_json operator[](const std::string &key) const;

        sf_json operator[](const std::string &key);

        const sf_json operator[](int key) const;

        explicit operator std::string() const;

        explicit operator bool() const;

        template<typename T>
        explicit operator typename std::enable_if<std::is_arithmetic<T>::value, T>::type() const;

        template<typename T>
        typename std::enable_if<std::is_arithmetic<T>::value, sf_json &>::type operator=(const T &value);

        sf_json &operator=(const std::string &value);

        void append(const sf_json &value);

        sf_json clone() const;

        bool join(const sf_json &other);

        void clear();

        void resize(int size);

        void remove(int pos);

        void remove(int pos, int len);

        void remove(const std::string &key);
    };

}