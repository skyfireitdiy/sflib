#include "sf_http_multipart_form.h"

namespace skyfire
{
    sf_http_multipart_type sf_http_multipart_form::get_type()
    {
        return sf_http_multipart_type ::form;
    }

    byte_array sf_http_multipart_form::append_data(byte_array data)
    {
        // TODO 增加数据
    }
}