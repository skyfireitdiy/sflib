
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_tcpserver.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma once

#ifdef _WIN32
#include "sf_tcpserver_win.hpp"
#else
#include "sf_tcpserver_linux.hpp"
#endif