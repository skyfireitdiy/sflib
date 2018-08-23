/*
 * sf_meta_object 元对象
 */

#pragma once

#include <vector>
#include <string>
#include <functional>
#include <tuple>
#include "sf_type.hpp"
#include "sf_serialize_binary.hpp"
#include "sf_meta.hpp"


namespace skyfire
{
    class sf_meta_object
    {
    private:
        std::vector<std::function<void(const std::string &,const byte_array&, void*&)>> __func__vec__;
    public:
        /**
         * @brief reg_meta_type 注册元对象
         * @param name 对象名称
         */
        template <typename  _Type, typename ... _Construct_Type>
        void reg_meta_type(const std::string &name);

        /**
         * @brief make_object 构造对象
         * @param type_name 对象名称
         * @param params 传递给构造函数的参数
         * @return 构造的对象
         */
        template <typename _Type = void, typename ... _Param>
        _Type * make_object(const std::string& type_name, _Param ... params);
    };

}
