
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_http_utils.hpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_http_multipart.hpp"
#include "sf_http_utils.h"

namespace skyfire {

inline unsigned char sf_to_hex(const unsigned char x) {
    return static_cast<unsigned char>(x > 9 ? x + 55 : x + 48);
}

inline unsigned char sf_from_hex(unsigned char x) {
    unsigned char y = 0;
    if (x >= 'A' && x <= 'Z')
        y = static_cast<unsigned char>(x - 'A' + 10);
    else if (x >= 'a' && x <= 'z')
        y = static_cast<unsigned char>(x - 'a' + 10);
    else if (x >= '0' && x <= '9')
        y = x - '0';
    return y;
}

inline std::string sf_url_encode(const std::string &str, bool encode_plus) {
    std::string strTemp{};
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (isalnum((unsigned char)str[i]) || (str[i] == '-') ||
            (str[i] == '_') || (str[i] == '.') || (str[i] == '~') ||
            (str[i] == '+'))
            strTemp += str[i];
        else if (str[i] == ' ' && encode_plus) {
            strTemp += "+";
        } else {
            strTemp += '%';
            strTemp += sf_to_hex((unsigned char)str[i] >> 4);
            strTemp += sf_to_hex(
                static_cast<const unsigned char>((unsigned char)str[i] % 16));
        }
    }
    return strTemp;
}

inline std::string sf_url_decode(const std::string &str, bool decode_plus) {
    std::string strTemp{};
    const auto length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '%') {
            const auto high = sf_from_hex(static_cast<unsigned char>(str[++i]));
            const auto low = sf_from_hex(static_cast<unsigned char>(str[++i]));
            strTemp += static_cast<char>(
                high * 16 +
                low);    // NOLINT(bugprone-string-integer-assignment)
        } else if (str[i] == '+' && decode_plus) {
            strTemp += ' ';
        } else {
            strTemp += str[i];
        }
    }
    return strTemp;
}

inline sf_http_param_t sf_parse_param(std::string param_str) {
    std::unordered_map<std::string, std::string> param;
    unsigned long url_pos;
    while ((url_pos = param_str.find('&')) != std::string::npos) {
        auto tmp_param =
            std::string(param_str.begin(), param_str.begin() + url_pos);
        param_str =
            std::string(param_str.begin() + url_pos + 1, param_str.end());
        if (tmp_param.empty()) continue;
        if ((url_pos = tmp_param.find('=')) == std::string::npos) continue;
        auto key = sf_url_decode(
            std::string(tmp_param.begin(), tmp_param.begin() + url_pos));
        const auto value = sf_url_decode(
            std::string(tmp_param.begin() + url_pos + 1, tmp_param.end()));
        param[key] = value;
    }
    if (param_str.empty()) return param;
    if ((url_pos = param_str.find('=')) == std::string::npos) return param;
    const auto key = sf_url_decode(
        std::string(param_str.begin(), param_str.begin() + url_pos));
    const auto value = sf_url_decode(
        std::string(param_str.begin() + url_pos + 1, param_str.end()));
    param[key] = value;
    return param;
}

inline void sf_parse_url(const std::string &raw_url, std::string &url,
                         sf_http_param_t &param, std::string &frame) {
    const auto frame_pos = raw_url.find('#');
    std::string raw_url_without_frame;
    if (frame_pos == std::string::npos) {
        raw_url_without_frame = sf_url_decode(raw_url);
        frame = "";
    } else {
        raw_url_without_frame = sf_url_decode(
            std::string(raw_url.begin(), raw_url.begin() + frame_pos));
        frame = sf_url_decode(
            std::string(raw_url.begin() + frame_pos + 1, raw_url.end()));
    }
    const auto url_pos = raw_url_without_frame.find('?');
    if (url_pos == std::string::npos) {
        url = sf_url_decode(raw_url_without_frame, false);
        return;
    }
    url = sf_url_decode(std::string(raw_url_without_frame.begin(),
                                    raw_url_without_frame.begin() + url_pos),
                        false);
    // NOTE param不用url decode，在sf_parse_param里面会decode
    const auto param_str =
        std::string(raw_url_without_frame.begin() + url_pos + 1,
                    raw_url_without_frame.end());
    param = sf_parse_param(param_str);
}

inline std::string sf_to_header_key_format(std::string key) {
    auto flag = false;
    for (auto &k : key) {
        if (0 != isalnum(k)) {
            if (!flag) k = static_cast<char>(toupper(k));
            flag = true;
        } else {
            flag = false;
        }
    }
    return key;
}

inline std::string sf_make_http_time_str(
    const std::chrono::system_clock::time_point &tp) {
    auto raw_time = std::chrono::system_clock::to_time_t(
        tp);    // NOLINT(cppcoreguidelines-pro-type-member-init)
#ifdef _MSC_VER
    std::tm _tmp_tm{};
    const auto time_info = &_tmp_tm;
    localtime_s(time_info, &raw_time);
#else
    std::tm *time_info = std::localtime(&raw_time);
#endif    // _MSC_VER
    std::string ret(128, '\0');
    strftime(ret.data(), 128, "%a, %d %b %Y %T GMT", time_info);
    ret.resize(strlen(ret.c_str()));
    return ret;
}

inline byte_array read_file(const std::string &filename, size_t max_size) {
    byte_array data;
    std::ifstream fi(filename, std::ios::in | std::ios::binary);
    if (!fi) {
        return data;
    }
    fi.seekg(0, std::ios::end);
    auto file_size = fi.tellg();
    if (file_size > max_size) {
        file_size = max_size;
    }
    fi.seekg(0, std::ios::beg);
    data.resize(static_cast<unsigned int>(file_size));
    fi.read(data.data(), file_size);
    fi.close();
    return data;
}

inline std::string sf_base64_encode(const byte_array &data) {
    BIO *b64, *bio;
    BUF_MEM *bptr = nullptr;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_write(bio, data.data(), static_cast<int>(data.size()));
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bptr);
    std::string out_str(bptr->length, '\0');
    memcpy(out_str.data(), bptr->data, bptr->length);
    BIO_free_all(bio);
    return out_str;
}

inline byte_array sf_base64_decode(const std::string &data) {
    BIO *b64, *bio;
    int size = 0;
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new_mem_buf(const_cast<char *>(data.data()),
                          static_cast<int>(data.size()));
    bio = BIO_push(b64, bio);
    byte_array out_str(data.size(), '\0');
    size = BIO_read(bio, out_str.data(), static_cast<int>(data.size()));
    out_str.resize(static_cast<unsigned long>(size));
    BIO_free_all(bio);
    return out_str;
}

inline byte_array sf_sha1_encode(const byte_array &data) {
    SHA_CTX sha_ctx;
    // NOTE 20表示sha1的长度
    byte_array ret(20, '\0');
    if (!SHA1_Init(&sha_ctx)) {
        return ret;
    }
    SHA1_Update(&sha_ctx, data.data(), data.size());
    SHA1_Final(reinterpret_cast<unsigned char *>(ret.data()), &sha_ctx);
    OPENSSL_cleanse(&sha_ctx, sizeof(sha_ctx));
    return ret;
}

inline byte_array sf_deflate_compress(const byte_array &input_buffer) {
    auto max_len = compressBound(input_buffer.size());
    byte_array ret(max_len);
    uLong size = max_len;
    compress(reinterpret_cast<Bytef *>(ret.data()), &size,
             reinterpret_cast<const Bytef *>(input_buffer.data()),
             input_buffer.size());
    ret.resize(size);
    return ret;
}

}    // namespace skyfire

#pragma clang diagnostic pop
