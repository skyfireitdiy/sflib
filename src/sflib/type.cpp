
#include "sflib/core/type.h"
namespace skyfire
{
std::string to_string(const byte_array& data)
{
    return std::string(data.data());
}
byte_array operator+(byte_array b1, const byte_array& b2)
{
    b1.insert(b1.end(), b2.begin(), b2.end());
    return b1;
}
byte_array& operator+=(byte_array& b1, const byte_array& b2)
{
    b1.insert(b1.end(), b2.begin(), b2.end());
    return b1;
}

byte_array to_byte_array(const char* s)
{
    return to_byte_array(std::string(s));
}
byte_array to_byte_array(const std::string& str)
{
    byte_array ret(str.length());
    std::memcpy(ret.data(), str.data(), str.length());
    return ret;
}
bool dump_byte_array(const byte_array& data, const std::string& filename,
                     bool app)
{
    std::ofstream fo(filename, app ? (std::ios::app | std::ios::binary) : (std::ios::trunc | std::ios::out));
    if (!fo)
        return false;
    fo.write(data.data(), data.size());
    return true;
}
} // namespace skyfire
