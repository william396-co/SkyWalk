# 报文定义

### 1. 报文定义

|      |  Head  |  Head  |  Head  |        Body        |
| :--: | :----: | :----: | :----: | :----------------: |
|  字段  |  Cmd   | Flags  |  Size  |        Body        |
|  说明  |  消息ID  |   标志   | Body长度 | 消息包体（ProtoBuf序列化出） |
|  类型  | Uint16 | Uint16 | Uint16 |        Byte        |



### 2. 包头说明

- 对齐方式：1字节对齐
- 字节序：大端字节序（网络字节序）
- Flags字段详细说明
  - 低8位定义
    - 0x00 - 数据结束END
    - 0x01 - 更多数据MORE
    - 0x02 - 压缩ZIP
    - 0x04 - 加密ENCRYPT

  - 高8位定义（校验和）

    `chksum = CRC16.calc( BODY );`

    `chksum %= 8520; chksum *= 1087;`

    `(uint8_t)( chksum % 255)`

    定时更新8520和1087掩码



### 3. 协议定义(详见protocol目录)
