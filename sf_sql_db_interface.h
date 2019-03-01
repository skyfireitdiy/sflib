#pragma once
#include "sf_stdc++.h"
#include "sf_sql_table_interface.h"

namespace skyfire
{
	class sf_sql_db_interface
	{
	private:
		virtual std::shared_ptr<sf_sql_table_interface> get_table(const std::string& table_name) = 0;
	};
}
