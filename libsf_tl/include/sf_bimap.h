#include <map>

namespace skyfire {
    template<typename Key_Type, typename Value_Type>
    class sf_bimap {
    private:
        std::map<Key_Type, Value_Type> left__;
        std::map<Value_Type, Key_Type> right__;
    public:
        void insert(const Key_Type &key, const Value_Type &value) {
            left__.insert({key, value});
            right__.insert({value, key});
        }

        void remove_key(const Key_Type &key) {
            if (left__.count(key) != 0) {
                Value_Type v = left__[key];
                left__.erase(key);
                right__.erase(value);
            }
        }

        void remove_value(const Value_Type &value) {
            if (right__.count(value) != 0) {
                Key_Type k = right__[value];
                left__.erase(key);
                right__.erase(value);
            }
        }

        std::map left() {
            return left__;
        }

        std::map right() {
            return right__;
        }
    };
}