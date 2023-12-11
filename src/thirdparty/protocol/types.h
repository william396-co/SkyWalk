#pragma once

#include "base/message.h"

//
// 通信包头定义
//

#pragma pack( 1 )

// 客户端和服务器之间通信的包头
struct CSHead
{
    enum FLAGS
    {
        END = 0x0000,     // 结束(默认)
        MORE = 0x0001,    // 更多
        ZIP = 0x0002,     // 压缩
        ENCRYPT = 0x0004, // 加密
    };

    MessageID cmd;  // 消息ID
    uint16_t flags; // 标志
                    // 0x00 - 数据结束
                    // 0x01 - 后续数据
                    // 0x02 - 压缩
                    // 0x04 - 加密
                    // 高8位保留
    uint16_t size;  // 消息包的长度
};

#pragma pack()

// 压缩长度阀值
#define ZIP_THRESHOLD 128

// 客户端消息序列化方法
namespace google {
namespace protobuf {
    class Message;
    template<typename Element> class RepeatedField;
} // namespace protobuf
} // namespace google
typedef google::protobuf::Message GameMessage;
typedef google::protobuf::RepeatedField<uint32_t> ArrayFields;
