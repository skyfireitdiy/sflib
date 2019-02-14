
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#include "sf_nocopy.h"
#include "sf_stdc++.h"
#include "sf_json.h"
#include "sf_object_factory_utils.h"

namespace skyfire
{
    class sf_object_factory final : public sf_nocopy<>
    {
	public:
		bool load_config(const std::string &config_str);
		bool load_config_file(const std::string &config_file);
		bool set_config(const sf_json &config_obj);

		template<typename T, typename ... ARGS>
		std::shared_ptr<T> get_object(const std::string& obj_id, ARGS&& ... args);

	private:
		std::unordered_map<std::string, sf_object_factory_config_item_t> object_data__;

		bool load_data__(const sf_json& config_obj);
		sf_json get_object_data(const std::string& obj_name);

    };
}
#pragma clang diagnostic pop