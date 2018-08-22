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
#include "sf_meta.h"


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
        void reg_meta_type(const std::string &name)
        {
            static_assert(!std::is_reference<_Type>::value, "Type can't be reference");
            static_assert(!std::is_pointer<_Type>::value, "Type can't be pointer");
            static_assert(!std::disjunction<std::is_reference<_Construct_Type>...>::value, "Param can't be reference");
            static_assert(!std::disjunction<std::is_pointer<_Construct_Type>...>::value, "Param can't be pointer");

            using __Type = typename std::decay<_Type>::type ;

            if constexpr (sizeof...(_Construct_Type) == 0)
            {
                auto f = [=](const std::string &name_str, const byte_array &data, void *&obj)
                {
                    if (name == name_str)
                    {
                        obj = new __Type();
                    }
                };
                __func__vec__.push_back(f);
            }
            else
            {
                using _Param = std::tuple<typename std::decay<_Construct_Type>::type ...>;

                auto f = [=](const std::string &name_str, const byte_array &data, void *&obj)
                {
                    if (name == name_str)
                    {
                        _Param param;
                        sf_deserialize_binary(data, param, 0);
                        obj = sf_make_obj_from_tuple<__Type>(param);
                    }
                };
                __func__vec__.push_back(f);
            }
        }

        /**
         * @brief make_object 构造对象
         * @param type_name 对象名称
         * @param params 传递给构造函数的参数
         * @return 构造的对象
         */
        template <typename _Type = void, typename ... _Param>
        _Type * make_object(const std::string& type_name, _Param ... params)
        {
            byte_array param = sf_serialize_binary(params...);
            void * ret = nullptr;
            for(auto &p : __func__vec__)
            {
                p(type_name, param , ret);
            }
            return reinterpret_cast<_Type*>(ret);
        }
    };


}
