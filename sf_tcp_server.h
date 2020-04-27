
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_server.h
*/

#pragma once

#ifdef _WIN32
#include "sf_tcp_server_win.hpp"
#else
#include "sf_tcp_server_linux.hpp"
#endif