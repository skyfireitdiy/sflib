
/**
* @version 1.0.0
* @author skyfire
* @file sf_tcp_nat_traversal_utils.h
*/
#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#include "sf_object.hpp"
#include "sf_stdc++.h"
#include "sf_type.hpp"
#include "sf_tcp_client.h"
#include "sf_tcp_server.h"
#include "sf_json.hpp"
#include "sf_nocopy.h"

namespace skyfire {

/**
 * @brief nat穿透连接上下文
 */
struct sf_tcp_nat_traversal_context_t__ final {
    std::string connect_id{};
    unsigned long long src_id{};
    sf_addr_info_t src_addr;
    unsigned long long dest_id{};
    sf_addr_info_t dest_addr;
    int error_code{};
    int step{};
    bool raw{};
};

SF_JSONIFY(sf_tcp_nat_traversal_context_t__, connect_id, src_id, src_addr,
           dest_id, dest_addr, error_code, step, raw)

/**
 *  @brief nat穿透连接类型
 */
enum class sf_tcp_nat_traversal_connection_type {
    type_server_valid,
    type_client_valid
};

class sf_tcp_nat_traversal_client;

/**
 *  @brief nat连接对象
 */
class sf_tcp_nat_traversal_connection : public sf_nocopy<sf_object> {
    /**
     * 数据到来信号
     */
    sf_singal(data_coming, const sf_pkg_header_t &, const byte_array &)
    /**
     * 原始数据到来信号
     */
    sf_singal(raw_data_coming, const byte_array &)
    /**
     * 关闭信号
     */
    sf_singal(closed)

   private:
    std::shared_ptr<sf_tcp_client> client__;
    std::shared_ptr<sf_tcp_server> server__;
    SOCKET sock__;
    std::string connect_id__;
    sf_tcp_nat_traversal_connection_type type__;

    sf_tcp_nat_traversal_connection(std::shared_ptr<sf_tcp_client> client,
                                    std::shared_ptr<sf_tcp_server> server,
                                    int sock, std::string connect_id,
                                    sf_tcp_nat_traversal_connection_type type);

   public:
    /**
     * 发送数据包
     * @param type 类型
     * @param data 数据
     * @return 是否成功
     */
    bool send(int type, const byte_array &data) const;

    /**
     * 发送数据
     * @param data 数据
     * @return 是否成功
     */
    bool send(const byte_array &data) const;

    friend sf_tcp_nat_traversal_client;
};
}    // namespace skyfire

#pragma clang diagnostic pop