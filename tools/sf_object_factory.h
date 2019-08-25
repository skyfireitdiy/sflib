
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#include "core/sf_stdc++.h"
#include "sf_object_factory_utils.h"
#include "tools/sf_json.h"
#include "tools/sf_nocopy.h"

namespace skyfire {
class sf_object_factory final : public sf_nocopy<> {
   public:
    /**
     * @brief 加载配置
     *
     * @param config_str 配置字符串
     * @return true 加载成功
     * @return false 加载失败
     */
    bool load_config(const std::string& config_str);
    /**
     * @brief 加载配置文件
     *
     * @param config_file 配置文件
     * @return true 加载成功
     * @return false 加载失败
     */
    bool load_config_file(const std::string& config_file);
    /**
     * @brief Set the config object设置配置
     *
     * @param config_obj json配置
     * @return true 设置成功
     * @return false 设置失败
     */
    bool set_config(const sf_json& config_obj);

    /**
     * @brief 获取对象
     *
     * @tparam T 类型
     * @tparam ARGS 构造函数参数类型
     * @param obj_id id
     * @param args 参数
     * @return std::shared_ptr<T> 对象
     */
    template <typename T, typename... ARGS>
    std::shared_ptr<T> object(const std::string& obj_id, ARGS&&... args);

   private:
    std::unordered_map<std::string, sf_object_factory_config_item_t>
        object_data__;

    bool load_data__(const sf_json& config_obj);
    sf_json object_data(const std::string& obj_name);
};
}    // namespace skyfire
#pragma clang diagnostic pop