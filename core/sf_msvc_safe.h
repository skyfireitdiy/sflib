#pragma once

#ifdef _MSC_VER

#define sf_safe_sprintf(_Buffer, _BufferCount, _Format, ...) \
    sprintf_s(_Buffer, _BufferCount, _Format, __VA_ARGS__)
#define sf_safe_inet_ntoa(family, ip, addr) \
    ip.resize(32);                          \
    inet_ntop(family, (const void *)&addr, (char *)ip.c_str(), 32)
#define sf_safe_inet_addr(family, addr_str, addr_buf) \
    inet_pton(family, addr_str, &addr_buf)

#else

#define sf_safe_sprintf(_Buffer, _BufferCount, _Format, ...) \
    sprintf(_Buffer, _Format, __VA_ARGS__)
#define sf_safe_inet_ntoa(family, ip, addr) ip = inet_ntoa(addr)
#define sf_safe_inet_addr(family, addr_str, addr_buf) \
    addr_buf = inet_addr(addr_str)

#endif    // DEBUG
