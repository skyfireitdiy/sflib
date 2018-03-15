#pragma once

#include <vector>
#include <string>
#include <functional>
#include <tuple>
#include "sf_type.h"
#include "sf_serialize.h"
#include "sf_meta.h"


namespace skyfire
{
    class sf_meta_object
    {
    public:
        std::vector<std::function<void(const std::string &,const byte_array&, void*&)>> __func__vec__;
        template <typename  _Type, typename ... _Construct_Type>
        void reg_meta_type(const std::string &name)
        {
            if constexpr (sizeof...(_Construct_Type) == 0)
            {
                auto f = [=](const std::string &name_str, const byte_array &data, void *&obj)
                {
                    if (name == name_str)
                    {
                        obj = new _Type();
                    }
                };
                __func__vec__.push_back(f);
            }
            else
            {
                using _Param = std::tuple<_Construct_Type...>;

                auto f = [=](const std::string &name_str, const byte_array &data, void *&obj)
                {
                    if (name == name_str)
                    {
                        _Param param;
                        sf_deserialize(data, param, 0);
                        obj = sf_make_obj_from_tuple<_Type>(param);
                    }
                };
                __func__vec__.push_back(f);
            }
        }

        template <typename _Type, typename ... _Param>
        _Type * make_object(const std::string& type_name, _Param ... params)
        {
            byte_array param;
            ((param += sf_serialize(params)), ...);
            void * ret = nullptr;
            for(auto &p : __func__vec__)
            {
                p(type_name, param , ret);
            }
            return reinterpret_cast<_Type*>(ret);
        }
    };
}