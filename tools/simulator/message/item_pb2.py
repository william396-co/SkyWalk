# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: item.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import base_pb2 as base__pb2
import type_pb2 as type__pb2
import itemtype_pb2 as itemtype__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\nitem.proto\x12\x08msg.item\x1a\nbase.proto\x1a\ntype.proto\x1a\x0eitemtype.proto\"(\n\x16ItemDetailInfosRequest:\x0e\x88\xb5\x18\x81(\x90\xb5\x18\x82(\x80\xa6\x1d\x02\"I\n\x17ItemDetailInfosResponse\x12\'\n\x05items\x18\x01 \x03(\x0b\x32\x18.msg.item.ItemDetailInfo:\x05\x88\xb5\x18\x82(\"\xe4\x01\n\x13\x42\x61gChangeListNotify\x12)\n\x07\x61\x64\x64list\x18\x01 \x03(\x0b\x32\x18.msg.item.ItemDetailInfo\x12\x12\n\ndeletelist\x18\x02 \x03(\x04\x12\x38\n\nchangelist\x18\x03 \x03(\x0b\x32$.msg.item.BagChangeListNotify.Change\x1aM\n\x06\x43hange\x12\x10\n\x08\x65ntityid\x18\x01 \x01(\x04\x12\r\n\x05\x63ount\x18\x02 \x01(\x03\x12\x12\n\nstoreplace\x18\x03 \x01(\x05\x12\x0e\n\x06heroid\x18\x04 \x01(\x04:\x05\x88\xb5\x18\x83(*\x1c\n\rErrorcodeItem\x12\x0b\n\x07Success\x10\x00\x62\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'item_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _ITEMDETAILINFOSREQUEST._options = None
  _ITEMDETAILINFOSREQUEST._serialized_options = b'\210\265\030\201(\220\265\030\202(\200\246\035\002'
  _ITEMDETAILINFOSRESPONSE._options = None
  _ITEMDETAILINFOSRESPONSE._serialized_options = b'\210\265\030\202('
  _BAGCHANGELISTNOTIFY._options = None
  _BAGCHANGELISTNOTIFY._serialized_options = b'\210\265\030\203('
  _ERRORCODEITEM._serialized_start=412
  _ERRORCODEITEM._serialized_end=440
  _ITEMDETAILINFOSREQUEST._serialized_start=64
  _ITEMDETAILINFOSREQUEST._serialized_end=104
  _ITEMDETAILINFOSRESPONSE._serialized_start=106
  _ITEMDETAILINFOSRESPONSE._serialized_end=179
  _BAGCHANGELISTNOTIFY._serialized_start=182
  _BAGCHANGELISTNOTIFY._serialized_end=410
  _BAGCHANGELISTNOTIFY_CHANGE._serialized_start=326
  _BAGCHANGELISTNOTIFY_CHANGE._serialized_end=403
# @@protoc_insertion_point(module_scope)
