#pragma once


#include "sf_stdc++.h"
#include "sf_json.h"

namespace skyfire
{
	struct sf_object_factory_config_item_t
	{
		std::string id;
		std::any object;
		bool singleton = false;
		sf_json data;
	};
}