#pragma once

#include "sf_bimap.h"

namespace skyfire {

    template<typename Key_Type, typename Value_Type>
    void sf_bimap<Key_Type, Value_Type>::insert(const Key_Type &key, const Value_Type &value) {
        left__.insert({key, value});
        right__.insert({value, key});
    }

    template<typename Key_Type, typename Value_Type>
    void sf_bimap<Key_Type, Value_Type>::remove_key(const Key_Type &key) {
        if (left__.count(key) != 0) {
            Value_Type value = left__[key];
            left__.erase(key);
            right__.erase(value);
        }
    }

    template<typename Key_Type, typename Value_Type>
    void sf_bimap<Key_Type, Value_Type>::remove_value(const Value_Type &value) {
        if (right__.count(value) != 0) {
            Key_Type key = right__[value];
            left__.erase(key);
            right__.erase(value);
        }
    }

    template<typename Key_Type, typename Value_Type>
    std::map<Key_Type, Value_Type> sf_bimap<Key_Type, Value_Type>::left() {
        return left__;
    }

    template<typename Key_Type, typename Value_Type>
    std::map<Value_Type, Key_Type> sf_bimap<Key_Type, Value_Type>::right() {
        return right__;
    }
}