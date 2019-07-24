#pragma once


#include "sf_json.h"


namespace skyfire
{
    struct sf_template_info_t{
        int pos;
        int len;
        std::string data;
    };

    SF_JSONIFY(sf_template_info_t, pos, len, data)


    struct sf_template_tree_node_t{
        std::string operation;
        std::vector<std::shared_ptr<sf_template_tree_node_t>> param;
        std::string data;
    };


    std::string sf_render(std::string content, const sf_json &js, const std::pair<std::string,std::string> &sep = {"{$", "$}"});

    std::vector<sf_template_info_t> sf_extract__(const std::string &content,
                                                 const std::pair<std::string, std::string> &sep);


    std::string sf_template_make_tree__(std::string content, const sf_json &js = sf_json());

}