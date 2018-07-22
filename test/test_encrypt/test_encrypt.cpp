#include "sf_utils.h"
#include "sf_type.h"
#include "sf_http_utils.h"

using namespace skyfire;
using namespace std;

int main()
{
    auto src_str = "dGhlIHNhbXBsZSBub25jZQ==" + websocket_sha1_append_str;
    auto sha = sf_sha1_encode(to_byte_array(src_str));
    for(unsigned char p:sha)
        printf("%02x",p);

    auto result = sf_base64_encode(sha);
    cout<<result<<endl;

}