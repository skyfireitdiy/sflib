
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file test_http_server.cpp

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

// #define SF_DEBUG
#include "sf_http_server.hpp"

using namespace std::literals;

using namespace skyfire;

void upload_file_route(const sf_http_request &req,sf_http_response& res) {
    // 判断请求类型是否为分块请求
    if(req.is_multipart_data())
    {
        std::string ret_str;
        // 获取请求头
        auto header = req.get_multipart_data_context().header;
        for(auto &p:header)
        {
            ret_str += p.first + ":" + p.second + "\n";
        }
        ret_str += "-----------header end-------------\n";
        // 获取分块
        auto multipart = req.get_multipart_data_context().multipart;
        for (auto &p: multipart)
        {
            // 获取分块请求头
            auto tmp_header = p.get_header().get_header();
            for (auto &q:tmp_header)
            {
                ret_str += q.first + ":" + q.second + "\n";
            }
            ret_str += "temp filename:" + p.get_filename() + "\n--------------------------\n\n";
        }
        // 将上述拼接的头信息拼装至body返回
        res.set_body(to_byte_array(ret_str));
    }
    else
    {
        res.set_body(to_byte_array("upload ok, no file"s));
        sf_http_cookie_t cookie;

        cookie.path = req.get_request_line().url;
        cookie.key="token";
        cookie.value=sf_random::get_instance()->get_uuid_str();
        // 添加cookie
        res.add_cookie(cookie);
        cookie.key="time";
        cookie.value = sf_make_http_time_str();
        cookie.http_only = false;
        res.add_cookie(cookie);
    }

    res.set_status(200);
}

void websocket_route(const sf_websocket_param_t& param)
{
    // 如果类型为文本，返回hello:原内容
    if(param.type == websocket_data_type::TextData){
        std::cout<<"received:"<<param.text_msg<<std::endl;
        param.p_server->send_websocket_data(param.sock,"hello:" + param.text_msg);
    } else{
        std::cout<<"binary data"<<std::endl;
    }
}


int main() {
    // 1. 生成一份http server配置
    sf_http_server_config config;
    config.host = "0.0.0.0";        // 监听ip
    config.port = 8080;             // 端口
    // 2. 根据配置生成一个http server
    auto server = sf_http_server::make_server(config);

    // 3. 添加一个http路由，地址为/upload_file， 回调函数为upload_file_route，方法为所有
    server->add_router(make_http_router(
            "/upload_file"s,
            upload_file_route,
            std::vector<std::string>{{"*"s}}
    ));

    // 4. 同样支持lambda
    server->add_router(make_http_router(
            "/user/(.*)/name"s,
            // 使用lambda时需要使用function包装一下，例子中第一个string参数会接收整个url，第二个会接收(.*?)匹配的url，规则与<regex>相同
            std::function([](const sf_http_request &req,sf_http_response& res, std::string, std::string user){
                res.set_body(to_byte_array(user+"'s name is skyfire"s));
            }),
            std::vector<std::string>{{"GET"s}}
    ));

    // 5. 添加一个websocket路由，地址为/ws，回调函数为websocket_route
    server->add_router(make_websocket_router("/ws", websocket_route));

    // 6. 设置一个静态资源路由，位置为"../example/test_http_server/testWebsite"，请求为所有，默认文件编码为utf-8，启用deflate压缩
    server->add_router(make_static_router(R"(C:\code\sflib\example\test_http_server\testWebsite)",  {{"*"s}}, "utf-8", true));

    // 7. 启动服务
    server->start(); 
}