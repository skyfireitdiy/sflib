#pragma once

#ifdef _WIN32
#include "sf_tcpclient_win.hpp"
#else
#include "sf_tcpclient_linux.hpp"
#endif