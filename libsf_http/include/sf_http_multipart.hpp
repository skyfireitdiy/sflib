#include "sf_http_multipart.h"


namespace skyfire
{
    sf_http_multipart::sf_http_multipart(sf_http_multipart_type type):
    {
    }

    sf_http_header_t sf_http_multipart::get_header() const
    {
        return header__;
    }

    bool sf_http_multipart::is_end() const
    {
        return end__;
    }
}