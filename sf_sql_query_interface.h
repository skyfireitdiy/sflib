#pragma once

#include "sf_stdc++.h"
#include "sf_json.h"

namespace skyfire
{
	enum class sf_sql_query_condition_type
	{
		Or,
		And,
		Equal,
		Limit,
		Order,
		Like,
		Big,
		Less,
		BigEq,
		LessEq,
		Between
	};

	struct sf_sql_query_condition
	{
		sf_sql_query_condition_type type;
		union 
		{
			std::vector<std::shared_ptr<sf_sql_query_condition>> or_data;
			std::vector<std::shared_ptr<sf_sql_query_condition>> and_data;
			struct
			{
				std::string key;
				sf_json value;
			} equal_data;
			struct
			{
				long long start;
				long long length;
			} limit_data;
			struct
			{
				std::string key;
				bool desc;
			} order_data;
			struct
			{
				std::string key;
				std::string like_str;
			}like_data;
			struct
			{
				std::string key;
				sf_json value;
			}big_data;
			struct
			{
				std::string key;
				sf_json value;
			}less_data;

			struct
			{
				std::string key;
				sf_json value;
			}bigeq_data;
			struct
			{
				std::string key;
				sf_json value;
			}lesseq_data;

			struct
			{
				std::string key;
				sf_json value1;
				sf_json value2;
			};

		} data;
	};

	class sf_sql_query_interface
	{
	public:
		
	};
}