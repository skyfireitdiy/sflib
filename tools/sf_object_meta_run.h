#pragma once

#include <functional>

namespace skyfire {
/**
 * @brief 辅助函数，让一个函数在main之前运行
 *
 */
class sf_object_meta_run final {
   public:
    /**
     * @brief Construct a new sf object meta run object
     *
     * @param f 要运行的函数
     */
    explicit sf_object_meta_run(std::function<void()> f);
};
}    // namespace skyfire