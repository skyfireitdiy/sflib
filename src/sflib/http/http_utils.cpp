

#include "sflib/http/http_utils.h"
#include "cryptopp/config_int.h"
#include "sflib/core/type.h"
#include "sflib/http/http_server_req_multipart.h"
#include <cryptopp/sha.h>
#include <cryptopp/base64.h>
#include <zconf.h>
#include <zlib.h>

namespace skyfire
{
unsigned char to_hex(const unsigned char x)
{
    return static_cast<unsigned char>(x > 9 ? x + 55 : x + 48);
}
unsigned char from_hex(unsigned char x)
{
    unsigned char y = 0;
    if (x >= 'A' && x <= 'Z')
        y = static_cast<unsigned char>(x - 'A' + 10);
    else if (x >= 'a' && x <= 'z')
        y = static_cast<unsigned char>(x - 'a' + 10);
    else if (x >= '0' && x <= '9')
        y = x - '0';
    return y;
}
std::string url_encode(const std::string &str, bool encode_plus)
{
    std::string strTemp {};
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) || (str[i] == '-') || (str[i] == '_') || (str[i] == '.') || (str[i] == '~') || (str[i] == '+'))
            strTemp += str[i];
        else if (str[i] == ' ' && encode_plus)
        {
            strTemp += "+";
        }
        else
        {
            strTemp += '%';
            strTemp += to_hex((unsigned char)str[i] >> 4);
            strTemp += to_hex(
                static_cast<const unsigned char>((unsigned char)str[i] % 16));
        }
    }
    return strTemp;
}
std::string url_decode(const std::string &str, bool decode_plus)
{
    std::string strTemp {};
    const auto length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == '%')
        {
            const auto high = from_hex(static_cast<unsigned char>(str[++i]));
            const auto low = from_hex(static_cast<unsigned char>(str[++i]));
            strTemp += static_cast<char>(
                high * 16 + low); // NOLINT(bugprone-string-integer-assignment)
        }
        else if (str[i] == '+' && decode_plus)
        {
            strTemp += ' ';
        }
        else
        {
            strTemp += str[i];
        }
    }
    return strTemp;
}
http_param_t parse_param(std::string param_str)
{
    std::unordered_map<std::string, std::string> param;
    unsigned long url_pos;
    while ((url_pos = param_str.find('&')) != std::string::npos)
    {
        auto tmp_param = std::string(param_str.begin(), param_str.begin() + url_pos);
        param_str = std::string(param_str.begin() + url_pos + 1, param_str.end());
        if (tmp_param.empty())
            continue;
        if ((url_pos = tmp_param.find('=')) == std::string::npos)
            continue;
        auto key = url_decode(
            std::string(tmp_param.begin(), tmp_param.begin() + url_pos));
        const auto value = url_decode(
            std::string(tmp_param.begin() + url_pos + 1, tmp_param.end()));
        param[key] = value;
    }
    if (param_str.empty())
        return param;
    if ((url_pos = param_str.find('=')) == std::string::npos)
        return param;
    const auto key = url_decode(
        std::string(param_str.begin(), param_str.begin() + url_pos));
    const auto value = url_decode(
        std::string(param_str.begin() + url_pos + 1, param_str.end()));
    param[key] = value;
    return param;
}
void parse_server_req_url(const std::string &raw_url, std::string &url,
                          http_param_t &param, std::string &frame)
{
    const auto frame_pos = raw_url.find('#');
    std::string raw_url_without_frame;
    if (frame_pos == std::string::npos)
    {
        raw_url_without_frame = url_decode(raw_url);
        frame = "";
    }
    else
    {
        raw_url_without_frame = url_decode(
            std::string(raw_url.begin(), raw_url.begin() + frame_pos));
        frame = url_decode(
            std::string(raw_url.begin() + frame_pos + 1, raw_url.end()));
    }
    const auto url_pos = raw_url_without_frame.find('?');
    if (url_pos == std::string::npos)
    {
        url = url_decode(raw_url_without_frame, false);
        return;
    }
    url = url_decode(std::string(raw_url_without_frame.begin(),
                                 raw_url_without_frame.begin() + url_pos),
                     false);
    // NOTE param不用url decode，在parse_param里面会decode
    const auto param_str = std::string(raw_url_without_frame.begin() + url_pos + 1,
                                       raw_url_without_frame.end());
    param = parse_param(param_str);
}
void parse_client_req_url(std::string raw_url,
                          std::string &agreement,
                          std::string &host,
                          short &port,
                          std::string &resource)
{
    agreement = "http";
    host = "";
    port = 80;
    resource = "/";
    const auto agreement_end_pos = raw_url.find("://");
    if (agreement_end_pos != std::string::npos)
    {
        agreement = to_lower_string(raw_url.substr(0, agreement_end_pos));
        raw_url = raw_url.substr(agreement_end_pos + 3);
    }
    else
    {
        const auto host_end_pos = raw_url.find("//");
        if (host_end_pos != std::string::npos)
        {
            raw_url = raw_url.substr(host_end_pos + 2);
        }
    }
    auto host_end_pos = raw_url.find(":");
    if (host_end_pos != std::string::npos)
    {
        host = raw_url.substr(0, host_end_pos);
        raw_url = raw_url.substr(host_end_pos + 1);
        const auto port_end_pos = raw_url.find("/");
        if (port_end_pos != std::string::npos)
        {
            port = atoi(raw_url.substr(0, port_end_pos).c_str());
            resource = raw_url.substr(port_end_pos);
        }
        else
        {
            port = atoi(raw_url.c_str());
            resource = "/";
        }
    }
    else
    {
        if (agreement == "http")
        {
            port = 80;
        }
        else
        {
            // FIXME 支持其他协议？
        }
        const auto host_end_pos = raw_url.find("/");
        if (host_end_pos != std::string::npos)
        {
            host = raw_url.substr(0, host_end_pos);
            resource = raw_url.substr(host_end_pos);
        }
        else
        {
            host = raw_url;
            resource = "/";
        }
    }
}
std::string to_header_key_format(std::string key)
{
    auto flag = false;
    for (auto &k : key)
    {
        if (0 != isalnum(k))
        {
            if (!flag)
                k = static_cast<char>(toupper(k));
            flag = true;
        }
        else
        {
            flag = false;
        }
    }
    return key;
}
std::string make_http_time_str(
    const std::chrono::system_clock::time_point &tp)
{
    auto raw_time = std::chrono::system_clock::to_time_t(
        tp); // NOLINT(cppcoreguidelines-pro-type-member-init)
#ifdef _MSC_VER
    std::tm _tmp_tm {};
    const auto time_info = &_tmp_tm;
    localtime_s(time_info, &raw_time);
#else
    std::tm *time_info = std::localtime(&raw_time);
#endif // _MSC_VER
    std::string ret(128, '\0');
    strftime(ret.data(), 128, "%a, %d %b %Y %T GMT", time_info);
    ret.resize(strlen(ret.c_str()));
    return ret;
}
byte_array read_file(const std::string &filename, size_t max_size)
{
    byte_array data;
    std::ifstream fi(filename, std::ios::in | std::ios::binary);
    if (!fi)
    {
        return data;
    }
    fi.seekg(0, std::ios::end);
    size_t file_size = fi.tellg();
    if (file_size > max_size)
    {
        file_size = max_size;
    }
    fi.seekg(0, std::ios::beg);
    data.resize(static_cast<unsigned int>(file_size));
    fi.read(data.data(), file_size);
    fi.close();
    return data;
}
std::string base64_encode(const byte_array &data)
{
    CryptoPP::Base64Encoder encoder;
    std::stringstream ss;
    encoder.Put((CryptoPP::byte *)data.data(), data.size());
    encoder.MessageEnd();
    std::string result(encoder.MaxRetrievable(), 0);
    encoder.Get((CryptoPP::byte *)result.data(), result.size());
    return result;
}
byte_array base64_decode(const std::string &data)
{
    CryptoPP::Base64Decoder decoder;
    decoder.Put((CryptoPP::byte *)data.data(), data.size());
    decoder.MessageEnd();
    byte_array result(decoder.MaxRetrievable(), 0);
    decoder.Get((CryptoPP::byte *)result.data(), result.size());
    return result;
}

byte_array sha1_encode(const byte_array &data)
{
    // 创建SHA-1散列对象
    CryptoPP::SHA1 sha1;
    // 散列结果存储区
    byte_array result(CryptoPP::SHA1::DIGESTSIZE, 0);
    sha1.CalculateDigest((CryptoPP::byte *)result.data(), (CryptoPP::byte *)data.data(), data.size());
    return result;
}

byte_array deflate_compress(const byte_array &input_buffer)
{
    auto max_len = compressBound(input_buffer.size());
    byte_array ret(max_len);
    uLong size = max_len;
    compress(reinterpret_cast<Bytef *>(ret.data()), &size,
             reinterpret_cast<const Bytef *>(input_buffer.data()),
             input_buffer.size());
    ret.resize(size);
    return ret;
}
} // namespace skyfire
