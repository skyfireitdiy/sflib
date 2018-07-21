#pragma once

#include "sf_http_router.h"
#include "sf_define.h"
#include "sf_http_utils.h"
#include "sf_content_type.h"
#include <fstream>

#include <iostream>

using namespace std;

namespace skyfire
{
    inline std::shared_ptr<sf_http_router> make_static_router(std::string path,const std::vector<std::string>& methods = {{"*"s}},
                                      std::string charset = "utf-8",
                                      bool gzip = true,
                                             unsigned long long max_file_size = DEFAULT_HTTP_MAX_FILE_SIZE,
                                             int priority = DEFAULT_HTTP_STATIC_PRIORITY

    ){
        std::string pattern = ".+";
        if(!path.empty())
        {
            auto ch = path[path.length()-1];
            if(ch=='\\'||ch=='/'){
                path.pop_back();
            }
        }
        std::function<void(const sf_http_request &,sf_http_response&,const std::string&)> callback=
                [=](const sf_http_request &req,sf_http_response&res,const std::string& raw_url) {
                    cout<<"raw_url:"<<raw_url<<endl;
                    std::string url;
                    std::map<std::string, std::string> param;
                    std::string frame;
                    sf_parse_url(raw_url, url, param, frame);
                    auto abspath = path + url;

                    sf_http_header header;

                    header.set_header("Date", sf_make_http_time_str());

                    // TODO 判断路径是否是目录

                    ifstream fi(abspath);
                    if(!fi)
                    {
                        res.set_status(404);
                        res.set_reason("NOT FOUND");
                        res.set_body(to_byte_array("<p>"+url + " Not Found</p>"));
                        header.set_header("Content-Type", "text/html; charset=" + charset);
                        res.set_header(header);
                        return;
                    }

                    fi.seekg(0, ios::end);
                    unsigned long long file_size = fi.tellg();
                    cout<<"file size:"<<file_size<<endl;
                    if (file_size > max_file_size) {
                        res.set_status(403);
                        res.set_reason("FORBIDDEN");
                        header.set_header("Content-Type", "text/html; charset=" + charset);
                        res.set_header(header);
                        res.set_body(to_byte_array("<p>" + url + " size out of range! (" + std::to_string(max_file_size) + ")</p>"));
                        fi.close();
                        return;
                    }
                    fi.seekg(ios::beg);
                    byte_array data(file_size);
                    fi.read(data.data(), file_size);
                    fi.close();
                    if (gzip) {
                        header.set_header("content-encoding", "gzip");
                        // 进行gzip压缩
                    }

                    std::string suffix = sf_to_lower_string(sf_get_path_ext(abspath));

                    if (sf_http_content_type.count(suffix) != 0) {
                        header.set_header("Content-Type", sf_http_content_type.at(suffix) + "; charset=" + charset);
                    } else {
                        header.set_header("Content-Type", "Unknown; charset=" + charset);
                    }

                    res.set_header(header);
                    res.set_body(data);
                };
        return std::make_shared<sf_http_router>(pattern,callback,methods,priority);
    }
}