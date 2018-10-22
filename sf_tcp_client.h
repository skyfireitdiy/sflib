
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcp_client.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#ifdef _WIN32
#include "sf_tcp_client_win.hpp"
#else
#include "sf_tcp_client_linux.hpp"
#endif