package utils

import (
	"bytes"
	"encoding/binary"
	"errors"
	"reflect"
)

// 数据流对象
type stream struct {
	buf      []byte // 字节数组
	encoding string // 编码格式
	endian   string // 字节序
	offset   uint64 // 偏移量
	count    uint64
}

// EncodeStream 编码对象
type EncodeStream struct {
	stream
}

// DecodeStream 解码对象
type DecodeStream struct {
	stream
}

// NewDecodeStream 构造解码方法
func NewDecodeStream(buf *[]byte) *DecodeStream {
	return &DecodeStream{stream{
		buf:      *buf,
		encoding: "utf8",
		endian:   "B",
		offset:   0,
	}}
}

// NewEncodeStream 构造编码方法
func NewEncodeStream(buf *[]byte) *EncodeStream {
	return &EncodeStream{
		stream: stream{
			buf:      *buf,
			encoding: "utf8",
			offset:   0,
			endian:   "B",
		},
	}
}

// 统一解码方法
var decodeFunc = func(val *[]byte, endian string, v interface{}) (err error) {
	if endian == "B" {
		err = binary.Read(bytes.NewReader(*val), binary.BigEndian, v)
	} else {
		err = binary.Read(bytes.NewReader(*val), binary.LittleEndian, v)
	}
	return err
}

// 统一加密方法
var encodeFunc = func(val *[]byte, endian string, v interface{}) (err error) {
	if endian == "B" {
		err = binary.Write(bytes.NewBuffer(*val), binary.BigEndian, v)
	} else {
		err = binary.Write(bytes.NewBuffer(*val), binary.LittleEndian, v)
	}
	return err
}

// Encoding 指定文字编码
func (s *stream) Encoding(encode string) {
	s.encoding = encode
}

// BigEndian 指定字节序 为BigEndian
func (s *stream) BigEndian() {
	s.endian = "B"
}

// LittleEndian 指定字节序 为LittleEndian
func (s *stream) LittleEndian() {
	s.endian = "L"
}

// Decode 根据传入的变量类型解码并赋值,必须是指针类型
func (s *DecodeStream) Decode(v interface{}) (err error) {
	if reflect.TypeOf(v).Kind().String() != "ptr" {
		return errors.New("必须是指针类型")
	}
	switch reflect.TypeOf(v).String() {
	case "*int8":
		fallthrough
	case "*uint8":
		err = s.dInt8(v)
	case "*int16":
		fallthrough
	case "*uint16":
		err = s.dInt16(v)
	case "*int32":
		fallthrough
	case "*uint32":
		err = s.dInt32(v)
	case "*int64":
		fallthrough
	case "*uint64":
		err = s.dInt64(v)
	case "*string":
		err = s.dString(v)
	default:
		err = errors.New("类型不匹配")
	}
	return err
}

// Encode 根据传入的变量类型加密,必须是指针类型
func (s *EncodeStream) Encode(v interface{}) (err error) {
	if reflect.TypeOf(v).Kind().String() != "ptr" {
		return errors.New("必须是指针类型")
	}
	switch reflect.TypeOf(v).String() {
	case "*int8":
		fallthrough
	case "*uint8":
		err = s.eInt8(v)
	case "*int16":
		fallthrough
	case "*uint16":
		err = s.eInt16(v)
	case "*int32":
		fallthrough
	case "*uint32":
		err = s.eInt32(v)
	case "*int64":
		fallthrough
	case "*uint64":
		err = s.eInt64(v)
	case "*string":
		err = s.eString(v)
	default:
		err = errors.New("类型不匹配")
	}
	return err
}

func (s *stream) dInt8(v interface{}) (err error) {
	if s.offset+1 > uint64(len(s.buf)) {
		return errors.New("data decode error : int8")
	}
	val := s.buf[s.offset : s.offset+1]
	s.offset += 1
	return decodeFunc(&val, s.endian, v)
}

func (s *stream) eInt8(v interface{}) (err error) {
	//在offset处扩容数组
	if s.offset+1 >= uint64(len(s.buf)) {
		s.append(1)
	}
	val := s.buf[s.offset:s.offset]
	err = encodeFunc(&val, s.endian, v)
	s.offset += 1
	s.count += 1
	return err
}

func (s *stream) dInt16(v interface{}) (err error) {
	if s.offset+2 > uint64(len(s.buf)) {
		return errors.New("data decode error : int16")
	}
	val := s.buf[s.offset : s.offset+2]
	s.offset += 2
	return decodeFunc(&val, s.endian, v)
}

func (s *stream) eInt16(v interface{}) (err error) {
	if s.offset+2 >= uint64(len(s.buf)) {
		s.append(2)
	}
	val := s.buf[s.offset:s.offset]
	err = encodeFunc(&val, s.endian, v)
	s.offset += 2
	s.count += 2
	return err
}

func (s *stream) dInt32(v interface{}) (err error) {
	if s.offset+4 > uint64(len(s.buf)) {
		return errors.New("data decode error : int32")
	}
	val := s.buf[s.offset : s.offset+4]
	s.offset += 4
	return decodeFunc(&val, s.endian, v)
}

func (s *stream) eInt32(v interface{}) (err error) {
	if s.offset+4 >= uint64(len(s.buf)) {
		s.append(4)
	}
	val := s.buf[s.offset:s.offset]
	err = encodeFunc(&val, s.endian, v)
	s.offset += 4
	s.count += 4
	return err
}

func (s *stream) dInt64(v interface{}) (err error) {
	if s.offset+8 > uint64(len(s.buf)) {
		return errors.New("data decode error : int64")
	}
	val := s.buf[s.offset : s.offset+8]
	s.offset += 8
	return decodeFunc(&val, s.endian, v)
}

func (s *stream) eInt64(v interface{}) (err error) {
	if s.offset+8 >= uint64(len(s.buf)) {
		s.append(8)
	}
	val := s.buf[s.offset:s.offset]
	err = encodeFunc(&val, s.endian, v)
	s.offset += 8
	s.count += 8
	return err
}

func (s *stream) dString(v interface{}) (err error) {
	//数据前8位是长度
	var length uint64
	err = s.dInt64(&length)
	if err != nil {
		return errors.New("data decode error : string_length")

	}

	//根据长度读取字符串
	if s.offset+length > uint64(len(s.buf)) {
		return errors.New("data decode error : string")
	}
	if length == 0 {
		return errors.New("data decode error : string length = 0")
	}
	val := s.buf[s.offset : s.offset+length-1]
	s.offset += length
	//创建字节数组接收字符串数据
	str := make([]byte, length-1)
	err = decodeFunc(&val, s.endian, &str)
	//转换为字符串赋值回去
	*(v.(*string)) = string(str)
	return err
}

func (s *stream) eString(v interface{}) (err error) {
	//数据前8位是长度
	length := uint64(len(*(v.(*string)))) + 1
	err = s.eInt64(&length)
	if err != nil {
		return err
	}
	if s.offset+length >= uint64(len(s.buf)) {
		s.append(int(length))
	}
	str := []byte(*(v.(*string)))
	val := s.buf[s.offset:s.offset]
	err = encodeFunc(&val, s.endian, &str)
	s.offset += length
	s.count += length
	return err
}

// append 扩容buffer
func (s *stream) append(len int) {
	s.buf = append(s.buf, make([]byte, len)...)
}

// GetBuf 获取未读部分的数据
func (s *stream) GetBuf() *[]byte {
	d := s.buf[s.offset:]
	return &d
}

// GetBufLength 获取未读数据长度
func (s *stream) GetBufLength() uint64 {
	return uint64(len(s.buf[s.offset:]))
}

// GetFullBuf 获取全部buf数据
func (s *stream) GetFullBuf() *[]byte {
	return &s.buf
}

// GetWriteBuf 获取写入数据
func (s *stream) GetWriteBuf() *[]byte {
	d := s.buf[:s.offset]
	return &d
}

// GetBufWithLength 获取写入数据
func (s *stream) GetBufWithLength(offset uint64) *[]byte {
	d := s.buf[:offset]
	return &d
}

// GetWriteLength 获取写入数据长度
func (s *stream) GetWriteLength() uint64 {
	return s.count
}

func (s *stream) GetOffset() uint64 {
	return s.offset
}

// GetFullBufLength 获取buf数据长度
func (s *stream) GetFullBufLength() uint64 {
	return uint64(len(s.buf))
}

func (s *stream) Reset() {
	s.offset = 0
}
