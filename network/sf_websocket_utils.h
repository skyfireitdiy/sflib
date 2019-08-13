
/**
* @version 1.0.0
* @author skyfire
* @mail skyfireitdiy@hotmail.com
* @see http://github.com/skyfireitdiy/sflib
* @file sf_websocket_utils.h

* sflib第一版本发布
* 版本号1.0.0
* 发布日期：2018-10-22
*/

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma once

#include <memory>

#include "core/sf_type.hpp"
#include "network/sf_tcp_utils.hpp"
#include "tools/sf_json.hpp"

#ifdef _WIN32
#include <winsock2.h>
#else

#include <arpa/inet.h>
#endif

namespace skyfire {

#pragma pack(1)

/**
 * @brief  websocket客户端小包头
 */
struct sf_websocket_client_data_1_header_t {
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    // 如果len的值在0-125，则是payload的真实长度。
    unsigned char mask_len;
    // 掩码密钥（当mask为1时有效）
    unsigned char mask_key[4];
};

/**
 * @brief  websocket客户端中包头
 */
struct sf_websocket_client_data_2_header_t {
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    unsigned char mask_len;
    // 如果len的是126，则后面2个字节形成的16bits无符号整型数的值是payload的真实长度。注意，网络字节序，需要转换。
    unsigned char extend_len[2];
    // 掩码密钥（当mask为1时有效）
    unsigned char mask_key[4];
};

/**
 *  @brief websocket客户端大包头
 */
struct sf_websocket_client_data_3_header_t {
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    unsigned char mask_len;
    // 如果len的是127，则后面8个字节形成的64bits无符号整型数的值是payload的真实长度。注意，网络字节序，需要转换。
    unsigned char extend_len[8];
    // 掩码密钥（当mask为1时有效）
    unsigned char mask_key[4];
};

/**
 * @brief  websocket服务器小包头
 */
struct sf_websocket_server_data_1_header_t {
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    // 如果len的值在0-125，则是payload的真实长度。
    unsigned char mask_len;
};

/**
 * @brief  websocket服务器中包头
 */
struct sf_websocket_server_data_2_header_t {
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    unsigned char mask_len;
    // 如果len的是126，则后面2个字节形成的16bits无符号整型数的值是payload的真实长度。注意，网络字节序，需要转换。
    unsigned char extend_len[2];
};

/**
 *  @brief websocket服务器大包头
 */
struct sf_websocket_server_data_3_header_t {
    // 0 bit: FIN标志
    // 1-3 bit: 3位保留
    // 4-7 bit: 操作码
    unsigned char fin_rsv_oc;
    // 0 bit: 掩码
    // 1-7 bit: 数据长度
    unsigned char mask_len;
    // 如果len的是127，则后面8个字节形成的64bits无符号整型数的值是payload的真实长度。注意，网络字节序，需要转换。
    unsigned char extend_len[8];
};

#pragma pack()

class sf_http_server;

/**
 *  @brief websocket数据类型
 */
enum class websocket_data_type {
    TextData,      // 文本数据
    BinaryData,    // 二进制数据
    Open,
    Close,
};

/**
 *  @brief websocket参数
 */
struct sf_websocket_param_t {
    SOCKET sock;                                           // socket
    std::string url;                                       // url
    std::unordered_map<std::string, std::string> param;    // param
    std::string frame;                                     // frame
    websocket_data_type type;                              // 数据类型
    std::string text_msg;    // 文本（当type为websocket_data_type::Text时有效）
    byte_array
        binary_data;    // 二进制（当type为websocket_data_type::BinaryData时有效）
    std::shared_ptr<sf_http_server> p_server;    // server指针
};

/**
 *  @brief Websocket包类型
 */
enum websocket_pkg_type : uint8_t {
    WEBSOCKET_OP_MIDDLE_PKG = 0x0,
    WEBSOCKET_OP_TEXT_PKG = 0x1,
    WEBSOCKET_OP_BINARY_PKG = 0x2,
    WEBSOCKET_OP_DISCONNECT_PKG = 0x08,
    WEBSOCKET_OP_PING_PKG = 0x09,
    WEBSOCKET_OP_PONG_PKG = 0x0A
};

/**
 * 生成server端websocket数据包
 * @tparam T 数据类型
 * @param data 数据
 * @return 字节数组
 */
template <typename T>
byte_array make_server_websocket_data_pkg(const T &data);

/**
 * 判断数据包是否是fin
 * @tparam T 数据包类型（大中小包头）
 * @param header 包头
 * @return 是否是fin
 */
template <typename T>
bool sf_is_fin(const T &header);

/**
 * 判断数据包是否有mask
 * @tparam T 数据包类型（大中小包头）
 * @param header 包头
 * @return 是否有mask
 */
template <typename T>
bool sf_with_mask(const T &header);

/**
 * 获取包长度
 * @param header 小包头
 * @return 包长度
 */
unsigned long long sf_get_size(
    const sf_websocket_client_data_1_header_t &header);

/**
 * 解码数据包
 * @param data 数据
 * @param mask_key 掩码
 */
void sf_decode_websocket_pkg(byte_array &data, const unsigned char *mask_key);

/**
 * 获取包长度
 * @param header 中包头
 * @return 包长度
 */
unsigned long long sf_get_size(
    const sf_websocket_client_data_2_header_t &header);

/**
 * 获取操作类型
 * @tparam T 包头类型（小中大包头）
 * @param header 包头
 * @return 操作类型
 */
template <typename T>
inline int sf_get_op_code(const T &header);

/**
 * 获取包长度
 * @param header 大包头
 * @return 包长度
 */
inline unsigned long long sf_get_size(
    const sf_websocket_client_data_3_header_t &header);
}    // namespace skyfire

#pragma clang diagnostic pop