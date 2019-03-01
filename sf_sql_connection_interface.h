#pragma once
#include "sf_stdc++.h"
#include "sf_sql_db_interface.h"

namespace skyfire
{
	class sf_sql_connection_interface
	{
	private:
		virtual bool connect(const std::string &host, unsigned short port, const std::string& user, const std::string& password) = 0;
		virtual std::shared_ptr<sf_sql_db_interface> get_db(const std::string& db_name) = 0;
	};
}