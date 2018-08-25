#pragma once

#include <map>
#include <sf_meta.h>
#include <dbm.h>

namespace skyfire {
    template<typename Key_Type, typename Value_Type>
    class sf_bimap {
    private:
        std::map<Key_Type, Value_Type> left__;
        std::map<Value_Type, Key_Type> right__;
    public:
        void insert(const Key_Type &key, const Value_Type &value);

        void remove_key(const Key_Type &key);

        void remove_value(const Value_Type &value);

        std::map<Key_Type,Value_Type> left();

        std::map<Value_Type,Key_Type> right();
    };

}