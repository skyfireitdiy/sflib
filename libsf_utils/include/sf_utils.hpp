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
    inline std::string sf_string_trim(const std::string& str)
    {
        if(str.length() == 0)
            return "";
        unsigned long begin = 0;
        for(unsigned long i = 0;i<str.length();++i)
        {
            if(!isspace(str[i]))
            {
                begin = i;
                break;
            }
        }
        unsigned long end = begin;
        for(unsigned long i = str.length();i>begin;--i)
        {
            if(!isspace(str[i-1]))
            {
                end = i;
                break;
            }
        }
        return {str.begin()+begin, str.begin()+end};
    }

    inline std::string sf_to_lower_string(std::string str)
    {
        for(auto &p:str)
            p = static_cast<char>(tolower(p));
        return str;
    }

    inline std::string sf_get_path_ext(const std::string &path)
    {
        auto pos = path.rfind('.');
        if(pos == -1)
            return "";
        return std::string(path.begin()+pos+1,path.end());
    }

    inline bool sf_equal_nocase_string(const std::string& str1,const std::string& str2)
    {
        return sf_to_lower_string(str1) == sf_to_lower_string(str2);
    }

    inline std::string sf_base64_encode(const byte_array &data){
        BIO *b64, *bio;
        BUF_MEM *bptr = nullptr;
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);
        BIO_write(bio, data.data(), static_cast<int>(data.size()));
        BIO_flush(bio);
        BIO_get_mem_ptr(bio, &bptr);
        std::string out_str(bptr->length,'\0');
        memcpy(out_str.data(), bptr->data, bptr->length);
        BIO_free_all(bio);
        return out_str;
    }

    inline byte_array sf_base64_decode(const std::string& data)
    {
        BIO *b64, *bio;
        int size = 0;
        b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        bio = BIO_new_mem_buf(const_cast<char*>(data.data()), static_cast<int>(data.size()));
        bio = BIO_push(b64, bio);
        byte_array out_str(data.size(),'\0');
        size = BIO_read(bio, out_str.data(), static_cast<int>(data.size()));
        out_str.resize(static_cast<unsigned long>(size));
        BIO_free_all(bio);
        return out_str;
    }

    inline byte_array sf_sha1_encode(const byte_array &data)
    {
        SHA_CTX sha_ctx;
        // NOTE 20表示sha1的长度
        byte_array ret(20,'\0');
        if(!SHA1_Init(&sha_ctx))
        {
            return ret;
        }
        SHA1_Update(&sha_ctx,data.data(),data.size());
        SHA1_Final(reinterpret_cast<unsigned char *>(ret.data()), &sha_ctx);
        OPENSSL_cleanse(&sha_ctx, sizeof(sha_ctx));
        return ret;
    }

    inline std::vector<std::string> sf_split_string(std::string str, const std::string& split_str)
    {
        std::vector<std::string> ret;
        unsigned long pos = 0;
        while(std::string::npos != (pos =str.find(split_str)))
        {
            ret.emplace_back(str.begin(),str.begin()+pos);
            str = std::string(str.begin()+pos+split_str.length(),str.end());
        }
        ret.push_back(str);
        return ret;
    }



    inline byte_array sf_deflate_compress(const byte_array &input_buffer)
    {
        auto max_len = compressBound(input_buffer.size());
        byte_array ret(max_len);
        uLong size = max_len;
        compress (reinterpret_cast<Bytef*>(ret.data()),&size,
                  reinterpret_cast<const Bytef*>(input_buffer.data()),
                  input_buffer.size());
        ret.resize(size);
        return  ret;
    }


    inline bool sf_is_dir(const std::string& path) {
        struct stat buf = {0};
        stat(path.c_str(), &buf);
        return static_cast<bool>(buf.st_mode & S_IFDIR);
    }

}