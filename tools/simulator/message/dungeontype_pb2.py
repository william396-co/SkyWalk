# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: dungeontype.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import base_pb2 as base__pb2
import type_pb2 as type__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x11\x64ungeontype.proto\x12\x0bmsg.dungeon\x1a\nbase.proto\x1a\ntype.proto\",\n\rDungeonRecord\x12\x0c\n\x04type\x18\x01 \x01(\x05\x12\r\n\x05value\x18\x02 \x01(\x03\"\x9d\x01\n\x07\x46ighter\x12\x0e\n\x06roleid\x18\x01 \x01(\x04\x12\x0c\n\x04side\x18\x02 \x01(\x05\x12\x10\n\x08rolename\x18\x03 \x01(\t\x12\r\n\x05level\x18\x04 \x01(\x05\x12\x10\n\x08viplevel\x18\x05 \x01(\x05\x12\x0e\n\x06\x61vatar\x18\x06 \x01(\r\x12\x13\n\x0b\x62\x61ttlepoint\x18\x07 \x01(\x03\x12\x1c\n\x05heros\x18\x08 \x03(\x0b\x32\r.msg.HeroInfob\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'dungeontype_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _DUNGEONRECORD._serialized_start=58
  _DUNGEONRECORD._serialized_end=102
  _FIGHTER._serialized_start=105
  _FIGHTER._serialized_end=262
# @@protoc_insertion_point(module_scope)
