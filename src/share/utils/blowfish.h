
#ifndef __SRC_UTILS_BLOWFISH_H__
#define __SRC_UTILS_BLOWFISH_H__

namespace utils {

#define DWORD unsigned int //
#define WORD unsigned short //
#define BYTE unsigned char //

#define MAXKEYBYTES 56 // 448 bits max
#define NPASS 16 // SBox passes

//
class Blowfish
{
public:
    Blowfish( BYTE * key, int keylength );
    ~Blowfish();

public:
    // 获得加密后的长度
    DWORD getOutLength( DWORD inlength );

    // 加密解密接口
    DWORD encrypt( BYTE * input, BYTE * output, DWORD inlength );
    DWORD decrypt( BYTE * input, BYTE * output, DWORD inlength );

private:
    // 初始化
    void Blowfish_init( BYTE * key, int keybytes );

    // 加密解密算法
    void Blowfish_encipher( DWORD * xl, DWORD * xr );
    void Blowfish_decipher( DWORD * xl, DWORD * xr );

private:
    DWORD * m_PArray;
    DWORD ( *m_SBoxes ) [256];
};

} // namespace utils

#endif
