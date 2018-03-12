#pragma once

#include "sf_rpcutils.h"

SF_BEGIN_REG_RPC_FUNC
SF_REG_RPC_FUNC("add",add,add,int,int,int);
SF_REG_RPC_FUNC("add_welcome",add_welcome,std::bind(add_welcome,
                                                           this_type::shared_from_this(),
                                                           std::placeholders::_1),
                       std::string,std::string);
SF_REG_RPC_FUNC("sort_vec",sort_vec, sort_vec, vector<int>, vector<int>);
SF_END_REG_RPC_FUNC