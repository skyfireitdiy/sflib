#include "sf_utils.h"
#include "sf_type.h"
#include "sf_http_utils.h"

using namespace skyfire;
using namespace std;

int main()
{
    auto sec_websocket_key = "nA1n3JLq123ENhNkHrhNhQ=="s;
    sec_websocket_key+=websocket_sha1_append_str;
    auto sha1_encoded_key = sf_sha1_encode(to_byte_array(sec_websocket_key));
    auto base64_encoded_key = sf_base64_encode(sha1_encoded_key);
    cout<<base64_encoded_key<<endl;
}