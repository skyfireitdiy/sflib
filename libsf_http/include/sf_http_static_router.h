#pragma once

#include "sf_http_router.h"
#include "sf_define.h"
#include "sf_http_utils.h"
#include "sf_content_type.h"
#include <fstream>
#include "sf_logger.h"


namespace skyfire {
    using namespace std::literals;

    inline std::shared_ptr<sf_http_router>
    make_static_router(std::string path, const std::vector<std::string> &methods = {{"*"s}},
                       std::string charset = "utf-8",
                       bool deflate = true,
                       unsigned long long max_file_size = DEFAULT_HTTP_MAX_FILE_SIZE,
                       int priority = DEFAULT_HTTP_STATIC_PRIORITY

    ) {
        std::string pattern = ".+";
        if (!path.empty()) {
            auto ch = path[path.length() - 1];
            if (ch == '\\' || ch == '/') {
                path.pop_back();
            }
        }
        std::function<void(const sf_http_request &, sf_http_response &, const std::string &)> callback =
                [=](const sf_http_request &req, sf_http_response &res, const std::string &raw_url) {
                    sf_debug(raw_url);
                    std::string url;
                    std::map<std::string, std::string> param;
                    std::string frame;
                    sf_parse_url(raw_url, url, param, frame);
                    auto abspath = path + url;

                    sf_http_header header;
                    byte_array body_data;

                    header.set_header("Date", sf_make_http_time_str());

                    if (sf_is_dir(abspath)) {
                        res.set_status(404);
                        res.set_reason("NOT FOUND");
                        body_data = to_byte_array("<p>" + url + " is a directory</p>");
                        header.set_header("Content-Type", "text/html; charset=" + charset);
                    } else {

                        std::ifstream fi(abspath);
                        if (!fi) {
                            res.set_status(404);
                            res.set_reason("NOT FOUND");
                            body_data = to_byte_array("<p>" + url + " Not Found</p>");
                            header.set_header("Content-Type", "text/html; charset=" + charset);
                        } else {

                            fi.seekg(0, std::ios::end);
                            auto file_size = static_cast<unsigned long long int>(fi.tellg());
                            sf_debug("File Size:", file_size);
                            if (file_size > max_file_size) {
                                res.set_status(403);
                                res.set_reason("FORBIDDEN");
                                header.set_header("Content-Type", "text/html; charset=" + charset);
                                body_data = to_byte_array(
                                        "<p>" + url + " size out of range! (" + std::to_string(max_file_size) +
                                        ")</p>");
                                fi.close();
                            }else {
                                fi.seekg(std::ios::beg);
                                body_data.resize(file_size);
                                fi.read(body_data.data(), file_size);
                                fi.close();
                                std::string suffix = sf_to_lower_string(sf_get_path_ext(abspath));

                                if (sf_http_content_type.count(suffix) != 0) {
                                    header.set_header("Content-Type",
                                                      sf_http_content_type.at(suffix) + "; charset=" + charset);
                                } else {
                                    header.set_header("Content-Type", "Unknown; charset=" + charset);
                                }
                            }
                        }
                    }

                    auto accept = req.get_header().get_header_value("Accept-Encoding", "");
                    auto accept_list = sf_split_string(accept, ",");
                    for (auto &p:accept_list)
                        p = sf_string_trim(p);

                    if (deflate && std::find_if(accept_list.begin(), accept_list.end(), [=](const std::string &str) {
                        return sf_equal_nocase_string(str, "deflate");
                    }) != accept_list.end()) {
                        body_data = sf_deflate_compress(body_data);
                        header.set_header("Content-Encoding", "deflate");
                    }
                    res.set_header(header);
                    res.set_body(body_data);
                };
        return std::make_shared<sf_http_router>(pattern, callback, methods, priority);
    }
}