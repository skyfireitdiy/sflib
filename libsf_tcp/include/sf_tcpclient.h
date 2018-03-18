#pragma once

#ifdef _WIN32
#include "sf_tcpclient_win.h"
#else
#include "sf_tcpclient_linux.h"
#endif