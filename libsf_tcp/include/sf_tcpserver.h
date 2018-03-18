#pragma once

#ifdef _WIN32
#include "sf_tcpserver_win.h"
#else
#include "sf_tcpserver_linux.h"
#endif