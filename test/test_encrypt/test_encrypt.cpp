
/**
* @version 1.0.0
* @author skyfire
* @email skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_encrypt.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#include "sf_utils.hpp"
#include "sf_type.hpp"
#include "sf_http_utils.hpp"


using namespace skyfire;
using namespace std::literals;

int main()
{
    auto sec_websocket_key = "nA1n3JLq123ENhNkHrhNhQ=="s;
    sec_websocket_key+=websocket_sha1_append_str;
    auto sha1_encoded_key = sf_sha1_encode(to_byte_array(sec_websocket_key));
    auto base64_encoded_key = sf_base64_encode(sha1_encoded_key);
    std::cout<<base64_encoded_key<<std::endl;
}