#pragma once

#include <string>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>
#include <zlib.h>
#include <zconf.h>

#include <cstring>

#include "sf_type.h"


#include <iostream>

using namespace std;


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
        bio = BIO_new_mem_buf(data.data(), static_cast<int>(data.size()));
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



    inline bool sf_gzip_uncompress( const byte_array & input_buffer, byte_array & output_buffer ) {
        if ( input_buffer.size() == 0 ) {
            output_buffer = input_buffer ;
            return true ;
        }

        output_buffer.clear() ;

        auto full_length = input_buffer.size();
        auto half_length = input_buffer.size() / 2;

        auto uncomp_length = full_length ;
        byte_array uncomp(uncomp_length, '\0');

        z_stream strm;
        strm.next_in = (Bytef *) input_buffer.data();
        strm.avail_in = static_cast<uInt>(input_buffer.size());
        strm.total_out = 0;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;

        bool done = false ;

        if (inflateInit2(&strm, MAX_WBITS) != Z_OK) {
            return false;
        }

        while (!done) {
            // If our output buffer is too small
            if (strm.total_out >= uncomp_length ) {
                // Increase size of output buffer
                byte_array uncomp2(uncomp_length + half_length , '\0');
                memcpy( uncomp2.data(), uncomp.data(), uncomp_length );
                uncomp_length += half_length ;
                uncomp = uncomp2 ;
            }

            strm.next_out = (Bytef *) (uncomp.data() + strm.total_out);
            strm.avail_out = static_cast<uInt>(uncomp_length - strm.total_out);

            // Inflate another chunk.
            int err = inflate (&strm, Z_SYNC_FLUSH);
            if (err == Z_STREAM_END) {
                done = true;
            } else if (err != Z_OK)  {
                break;
            }
        }

        if (inflateEnd (&strm) != Z_OK) {
            return false;
        }

        for ( size_t i=0; i<strm.total_out; ++i ) {
            output_buffer.push_back(uncomp[i]);
        }
        return true ;
    }


    inline bool sf_gzip_compress(const byte_array & input_buffer, byte_array &output_buffer)
    {
        size_t in_data_size = input_buffer.size();
        byte_array buffer;

        const size_t buf_size = 128 * 1024;
        uint8_t temp_buffer[buf_size];

        z_stream strm;
        strm.zalloc = nullptr;
        strm.zfree = nullptr;
        strm.next_in = (Bytef *)(input_buffer.data());
        strm.avail_in = in_data_size;
        strm.next_out = temp_buffer;
        strm.avail_out = buf_size;

        deflateInit(&strm, Z_BEST_COMPRESSION);

        while (strm.avail_in != 0)
        {
            int res = deflate(&strm, Z_NO_FLUSH);
            if(res != Z_OK)
            {
                return false;
            }
            if (strm.avail_out == 0)
            {
                buffer.insert(buffer.end(), temp_buffer, temp_buffer + buf_size);
                strm.next_out = temp_buffer;
                strm.avail_out = buf_size;
            }
        }

        int deflate_res = Z_OK;
        while (deflate_res == Z_OK)
        {
            if (strm.avail_out == 0)
            {
                buffer.insert(buffer.end(), temp_buffer, temp_buffer + buf_size);
                strm.next_out = temp_buffer;
                strm.avail_out = buf_size;
            }
            deflate_res = deflate(&strm, Z_FINISH);
        }

        if(deflate_res != Z_STREAM_END){
            return false;
        }
        buffer.insert(buffer.end(), temp_buffer, temp_buffer + buf_size - strm.avail_out);
        deflateEnd(&strm);

        output_buffer.swap(buffer);
        return true;
    }

}