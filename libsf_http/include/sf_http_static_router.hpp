#pragma once

#include "sf_http_static_router.h"

namespace skyfire
{
    using namespace std::literals;

    inline std::shared_ptr<sf_http_router>
    make_static_router(std::string path, const std::vector<std::string> &methods, std::string charset, bool deflate,
                       unsigned long long int max_file_size, int priority)
    {
        std::string pattern = ".+";
        if (!path.empty())
        {
            auto ch = path[path.length() - 1];
            if (ch == '\\' || ch == '/')
            {
                path.pop_back();
            }
        }
        std::function<void(const sf_http_request &, sf_http_response &, const std::string &)> callback =
                [=](const sf_http_request &req, sf_http_response &res, const std::string &raw_url)
                {
                    // sf_debug(raw_url);
                    std::string url;
                    std::map<std::string, std::string> param;
                    std::string frame;
                    sf_parse_url(raw_url, url, param, frame);
                    auto abspath = path + url;

                    sf_http_header header;
                    byte_array body_data;

                    header.set_header("Date", sf_make_http_time_str());

                    auto _401_res = [&]
                    {
                        res.set_status(401);
                        res.set_reason("BAD REQUEST");
                        header.set_header("Content-Type", "text/html; charset=" + charset);
                        body_data = to_byte_array(
                                "<p>" + url + " bad request! (" + std::to_string(max_file_size) +
                                ")</p>");
                    };
                    auto _403_res = [&]
                    {
                        res.set_status(403);
                        res.set_reason("FORBIDDEN");
                        header.set_header("Content-Type", "text/html; charset=" + charset);
                        body_data = to_byte_array(
                                "<p>" + url + " size out of range! (" + std::to_string(max_file_size) +
                                ")</p>");
                    };
                    auto _404_res = [&]
                    {
                        res.set_status(404);
                        res.set_reason("NOT FOUND");
                        body_data = to_byte_array("<p>" + url + " is a directory</p>");
                        header.set_header("Content-Type", "text/html; charset=" + charset);
                    };
                    auto _416_res = [&]
                    {
                        res.set_status(404);
                        res.set_reason("Requested Range Not Satisfiable");
                        body_data = to_byte_array("<p>" + url + " Requested Range Not Satisfiable</p>");
                        header.set_header("Content-Type", "text/html; charset=" + charset);
                    };


                    if (sf_is_dir(abspath))
                    {
                        _404_res();
                    } else
                    {

                        std::ifstream fi(abspath);
                        if (!fi)
                        {
                            _404_res();
                        } else
                        {
                            fi.seekg(0, std::ios::end);
                            auto file_size = static_cast<unsigned long long int>(fi.tellg());
                            fi.seekg(std::ios::beg);
                            // sf_debug("File Size:", file_size);

                            if (header.has_key("Range"))
                            {
                                auto range_header = header.get_header_value("Range", "");
                                auto range_content_list = sf_split_string(range_header, "=");
                                if (range_content_list.size() < 2)
                                {
                                    _401_res();
                                } else
                                {
                                    auto range_list = sf_split_string(range_content_list[1], ",");
                                    for (auto &p:range_list)
                                    {
                                        p = sf_string_trim(p);
                                    }
                                    if (range_list.size() > 1)
                                    {
                                        // TODO multipart
                                    } else
                                    {
                                        int start, end;
                                        if (sscanf(range_list[0].c_str(), "%d-%d", &start, &end) != 2)
                                        {
                                            _401_res();
                                        } else
                                        {
                                            if (start > file_size)
                                            {
                                                _416_res();
                                            } else
                                            {
                                                if (end - start > max_file_size)
                                                {
                                                    end = start + max_file_size;
                                                }
                                                if (end > file_size)
                                                {
                                                    end = file_size;
                                                }
                                                fi.seekg(start, std::ios::beg);
                                                body_data.resize(end - start);
                                                fi.read(body_data.data(), end - start);
                                                fi.close();
                                                header.set_header("Content-Range", "bytes " +
                                                                                   std::to_string(start) + "-" +
                                                                                   std::to_string(end) + "/" +
                                                                                   std::to_string(file_size));
                                                if(end == file_size)
                                                {
                                                    res.set_status(200);
                                                }
                                                else
                                                {
                                                    res.set_status(206);
                                                    res.set_reason("Partial Content");
                                                }
                                            }
                                        }
                                    }
                                }
                            } else if (file_size > max_file_size)
                            {
                                int start = 0, end = max_file_size;

                                if (end - start > max_file_size)
                                {
                                    end = start + max_file_size;
                                }
                                if (end > file_size)
                                {
                                    end = file_size;
                                }
                                fi.seekg(start, std::ios::beg);
                                body_data.resize(end - start);
                                fi.read(body_data.data(), end - start);
                                fi.close();
                                header.set_header("Content-Range", "bytes " +
                                                                   std::to_string(start) + "-" +
                                                                   std::to_string(end) + "/" +
                                                                   std::to_string(file_size));
                                if(end == file_size)
                                {
                                    res.set_status(200);
                                }
                                else
                                {
                                    res.set_status(206);
                                    res.set_reason("Partial Content");
                                }
                            } else
                            {
                                body_data.resize(file_size);
                                fi.read(body_data.data(), file_size);
                                std::string suffix = sf_to_lower_string(sf_get_path_ext(abspath));

                                if (sf_http_content_type.count(suffix) != 0)
                                {
                                    header.set_header("Content-Type",
                                                      sf_http_content_type.at(suffix) + "; charset=" + charset);
                                } else
                                {
                                    header.set_header("Content-Type", "Unknown; charset=" + charset);
                                }
                            }
                            fi.close();
                        }
                    }

                    auto accept = req.get_header().get_header_value("Accept-Encoding", "");
                    auto accept_list = sf_split_string(accept, ",");
                    for (auto &p:accept_list)
                        p = sf_string_trim(p);

                    if (deflate && std::find_if(accept_list.begin(), accept_list.end(), [=](const std::string &str)
                    {
                        return sf_equal_nocase_string(str, "deflate");
                    }) != accept_list.end())
                    {
                        body_data = sf_deflate_compress(body_data);
                        header.set_header("Content-Encoding", "deflate");
                    }
                    res.set_header(header);
                    res.set_body(body_data);
                };
        return std::make_shared<sf_http_router>(pattern, callback, methods, priority);
    }
}