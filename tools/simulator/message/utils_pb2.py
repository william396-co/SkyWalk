# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: utils.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import base_pb2 as base__pb2
import type_pb2 as type__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x0butils.proto\x12\tmsg.utils\x1a\nbase.proto\x1a\ntype.proto\"5\n\x0f\x45rrorcodeNotify\x12\x0b\n\x03\x63md\x18\x01 \x01(\r\x12\x0e\n\x06result\x18\x02 \x01(\r:\x05\x88\xb5\x18\x81 \"S\n\x10UtilsPingMessage\x12\x11\n\ttimestamp\x18\x01 \x01(\x03\x12\x15\n\x0brttimestamp\x18\x02 \x01(\x03H\x00:\t\x88\xb5\x18\x82 \x80\xa6\x1d\x01\x42\n\n\x08rtt_test\"\x1e\n\x11GetVersionRequest:\t\x88\xb5\x18\x83 \x80\xa6\x1d\x03\"O\n\x12GetVersionResponse\x12\x0c\n\x04mode\x18\x01 \x01(\r\x12\x10\n\x08versions\x18\x02 \x03(\t\x12\x12\n\nactversion\x18\x03 \x01(\t:\x05\x88\xb5\x18\x84 \"6\n\x13ShellCommandMessage\x12\x0f\n\x07\x63ontent\x18\x01 \x01(\t:\x0e\x88\xb5\x18\x86 \x80\xa6\x1d\x01\x90\xb5\x18\x86 \")\n\rSkipdayNotify\x12\x11\n\ttimestamp\x18\x01 \x01(\x03:\x05\x88\xb5\x18\x85 \"K\n\x0cOrderRequest\x12\x10\n\x08packages\x18\x01 \x01(\t\x12\x0e\n\x06number\x18\x02 \x01(\r\x12\x0e\n\x06goodid\x18\x03 \x01(\r:\t\x88\xb5\x18\x87 \x80\xa6\x1d\x02\"G\n\rOrderResponse\x12\x10\n\x08packages\x18\x01 \x01(\t\x12\x0e\n\x06number\x18\x02 \x01(\r\x12\r\n\x05order\x18\x03 \x01(\t:\x05\x88\xb5\x18\x88 \"m\n\x10PayDeliverNotify\x12\x0e\n\x06method\x18\x01 \x01(\r\x12\r\n\x05order\x18\x02 \x01(\t\x12\x10\n\x08packages\x18\x03 \x01(\t\x12\r\n\x05times\x18\x04 \x01(\r\x12\x12\n\nresetstate\x18\x05 \x01(\x08:\x05\x88\xb5\x18\x89 \"#\n\x11GetPaymentRequest:\x0e\x88\xb5\x18\x8d \x90\xb5\x18\x8e \x80\xa6\x1d\x02\"\xc5\x01\n\x12GetPaymentResponse\x12\x10\n\x08vipstate\x18\x01 \x01(\r\x12\x11\n\tpay1state\x18\x02 \x01(\r\x12>\n\nrecordlist\x18\x03 \x03(\x0b\x32*.msg.utils.GetPaymentResponse.ChargeRecord\x1a\x43\n\x0c\x43hargeRecord\x12\x10\n\x08packages\x18\x01 \x01(\t\x12\r\n\x05times\x18\x02 \x01(\r\x12\x12\n\nresetstate\x18\x03 \x01(\x08:\x05\x88\xb5\x18\x8e \"G\n\x18GetLotteryRecordsRequest\x12\x0e\n\x06system\x18\x01 \x01(\r\x12\x10\n\x08\x63\x61tegory\x18\x02 \x01(\r:\t\x88\xb5\x18\x8a \x80\xa6\x1d\x03\"\xe2\x01\n\x19GetLotteryRecordsResponse\x12\x0e\n\x06system\x18\x01 \x01(\r\x12\x10\n\x08\x63\x61tegory\x18\x02 \x01(\r\x12?\n\nrecordlist\x18\x03 \x03(\x0b\x32+.msg.utils.GetLotteryRecordsResponse.Record\x1a[\n\x06Record\x12\x10\n\x08rewardid\x18\x01 \x01(\r\x12\x0c\n\x04name\x18\x02 \x01(\x0c\x12\x11\n\ttimestamp\x18\x03 \x01(\x03\x12\x1e\n\x07rewards\x18\x04 \x01(\x0b\x32\r.msg.Resource:\x05\x88\xb5\x18\x8b \"I\n\x0e\x42ulletinNotify\x12\x0e\n\x06\x62\x61seid\x18\x01 \x01(\r\x12\x11\n\tparamlist\x18\x02 \x03(\t\x12\r\n\x05style\x18\x03 \x01(\r:\x05\x88\xb5\x18\x8c *\xb7\x02\n\x0e\x45rrorcodeUtils\x12\x0b\n\x07Success\x10\x00\x12\x15\n\x11NotSupportRequest\x10\x01\x12\x15\n\x11LoadFailureFromDB\x10\x02\x12\x0c\n\x08NoRouter\x10\x03\x12\x0f\n\x0bParseFailed\x10\x04\x12\x15\n\x0fOrderNotSupport\x10\x81\x8e@\x12\x19\n\x13OrderPackageInvalid\x10\x82\x8e@\x12\x1e\n\x18OrderPackagePriceInvalid\x10\x83\x8e@\x12\x17\n\x11OrderCreateFailed\x10\x84\x8e@\x12\x18\n\x12OrderNumberInvalid\x10\x85\x8e@\x12\x15\n\x0fOrderIDConflict\x10\x86\x8e@\x12\x15\n\x0fOrderCanotFound\x10\x87\x8e@\x12\x18\n\x12OrderDeliverFailed\x10\x88\x8e@b\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'utils_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _ERRORCODENOTIFY._options = None
  _ERRORCODENOTIFY._serialized_options = b'\210\265\030\201 '
  _UTILSPINGMESSAGE._options = None
  _UTILSPINGMESSAGE._serialized_options = b'\210\265\030\202 \200\246\035\001'
  _GETVERSIONREQUEST._options = None
  _GETVERSIONREQUEST._serialized_options = b'\210\265\030\203 \200\246\035\003'
  _GETVERSIONRESPONSE._options = None
  _GETVERSIONRESPONSE._serialized_options = b'\210\265\030\204 '
  _SHELLCOMMANDMESSAGE._options = None
  _SHELLCOMMANDMESSAGE._serialized_options = b'\210\265\030\206 \200\246\035\001\220\265\030\206 '
  _SKIPDAYNOTIFY._options = None
  _SKIPDAYNOTIFY._serialized_options = b'\210\265\030\205 '
  _ORDERREQUEST._options = None
  _ORDERREQUEST._serialized_options = b'\210\265\030\207 \200\246\035\002'
  _ORDERRESPONSE._options = None
  _ORDERRESPONSE._serialized_options = b'\210\265\030\210 '
  _PAYDELIVERNOTIFY._options = None
  _PAYDELIVERNOTIFY._serialized_options = b'\210\265\030\211 '
  _GETPAYMENTREQUEST._options = None
  _GETPAYMENTREQUEST._serialized_options = b'\210\265\030\215 \220\265\030\216 \200\246\035\002'
  _GETPAYMENTRESPONSE._options = None
  _GETPAYMENTRESPONSE._serialized_options = b'\210\265\030\216 '
  _GETLOTTERYRECORDSREQUEST._options = None
  _GETLOTTERYRECORDSREQUEST._serialized_options = b'\210\265\030\212 \200\246\035\003'
  _GETLOTTERYRECORDSRESPONSE._options = None
  _GETLOTTERYRECORDSRESPONSE._serialized_options = b'\210\265\030\213 '
  _BULLETINNOTIFY._options = None
  _BULLETINNOTIFY._serialized_options = b'\210\265\030\214 '
  _ERRORCODEUTILS._serialized_start=1278
  _ERRORCODEUTILS._serialized_end=1589
  _ERRORCODENOTIFY._serialized_start=50
  _ERRORCODENOTIFY._serialized_end=103
  _UTILSPINGMESSAGE._serialized_start=105
  _UTILSPINGMESSAGE._serialized_end=188
  _GETVERSIONREQUEST._serialized_start=190
  _GETVERSIONREQUEST._serialized_end=220
  _GETVERSIONRESPONSE._serialized_start=222
  _GETVERSIONRESPONSE._serialized_end=301
  _SHELLCOMMANDMESSAGE._serialized_start=303
  _SHELLCOMMANDMESSAGE._serialized_end=357
  _SKIPDAYNOTIFY._serialized_start=359
  _SKIPDAYNOTIFY._serialized_end=400
  _ORDERREQUEST._serialized_start=402
  _ORDERREQUEST._serialized_end=477
  _ORDERRESPONSE._serialized_start=479
  _ORDERRESPONSE._serialized_end=550
  _PAYDELIVERNOTIFY._serialized_start=552
  _PAYDELIVERNOTIFY._serialized_end=661
  _GETPAYMENTREQUEST._serialized_start=663
  _GETPAYMENTREQUEST._serialized_end=698
  _GETPAYMENTRESPONSE._serialized_start=701
  _GETPAYMENTRESPONSE._serialized_end=898
  _GETPAYMENTRESPONSE_CHARGERECORD._serialized_start=824
  _GETPAYMENTRESPONSE_CHARGERECORD._serialized_end=891
  _GETLOTTERYRECORDSREQUEST._serialized_start=900
  _GETLOTTERYRECORDSREQUEST._serialized_end=971
  _GETLOTTERYRECORDSRESPONSE._serialized_start=974
  _GETLOTTERYRECORDSRESPONSE._serialized_end=1200
  _GETLOTTERYRECORDSRESPONSE_RECORD._serialized_start=1102
  _GETLOTTERYRECORDSRESPONSE_RECORD._serialized_end=1193
  _BULLETINNOTIFY._serialized_start=1202
  _BULLETINNOTIFY._serialized_end=1275
# @@protoc_insertion_point(module_scope)