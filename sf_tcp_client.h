
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_client.h
*/

#pragma once

#ifdef _WIN32
#include "sf_tcp_client_win.hpp"
#else
#include "sf_tcp_client_linux.hpp"
#endif