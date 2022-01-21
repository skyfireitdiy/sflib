
#pragma once
#include "http_server_req_header.h"
#include "type.h"
namespace skyfire
{
class http_server_req_multipart final
{
private:
    http_server_req_header         header__;
    bool                           end__    = { false };
    bool                           finish__ = { false };
    std::string                    boundary_str__;
    std::shared_ptr<std::ofstream> fp__;
    bool                           first_block = true;
    std::string                    filename__;

public:
    http_server_req_multipart(const std::string& boundary_str,
                              const std::string& tmp_path);
    http_server_req_header header() const;
    bool                   is_end() const;
    bool                   is_finished() const;
    std::string            filename() const;
    bool                   append_data(const byte_array& data, byte_array& ret);
};
} // namespace skyfire
