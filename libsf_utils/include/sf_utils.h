#pragma once

#include <string>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <zlib.h>
#include <zconf.h>
#include <fstream>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

#include "sf_type.hpp"

namespace skyfire
{
    inline std::string sf_string_trim(const std::string& str);
    inline std::string sf_to_lower_string(std::string str);
    inline std::string sf_get_path_ext(const std::string &path);

    inline bool sf_equal_nocase_string(const std::string& str1,const std::string& str2);

    inline std::string sf_base64_encode(const byte_array &data);

    inline byte_array sf_base64_decode(const std::string& data);

    inline byte_array sf_sha1_encode(const byte_array &data);
    inline std::vector<std::string> sf_split_string(std::string str, const std::string& split_str);
    inline byte_array sf_deflate_compress(const byte_array &input_buffer);
    inline bool sf_is_dir(const std::string& path);


    template <typename ... Str>
    std::string sf_path_join(std::string str1, Str... str2);

    inline std::string sf_path_join(std::string path1, const std::string path2);

}