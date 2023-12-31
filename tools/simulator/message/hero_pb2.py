# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: hero.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import base_pb2 as base__pb2
import type_pb2 as type__pb2
import herotype_pb2 as herotype__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\nhero.proto\x12\x08msg.hero\x1a\nbase.proto\x1a\ntype.proto\x1a\x0eherotype.proto\"!\n\x0fHeroListRequest:\x0e\x88\xb5\x18\x81&\x90\xb5\x18\x82&\x80\xa6\x1d\x02\"<\n\x10HeroListResponse\x12!\n\x05heros\x18\x01 \x03(\x0b\x32\x12.msg.hero.HeroInfo:\x05\x88\xb5\x18\x82&\"\xf3\x01\n\rNewHeroNotify\x12!\n\x05heros\x18\x01 \x03(\x0b\x32\x12.msg.hero.HeroInfo\x12\x32\n\x08\x66raglist\x18\x02 \x03(\x0b\x32 .msg.hero.NewHeroNotify.HeroFrag\x1a)\n\x08\x46ragInfo\x12\x0e\n\x06\x66ragid\x18\x01 \x01(\x05\x12\r\n\x05\x63ount\x18\x02 \x01(\x03\x1aY\n\x08HeroFrag\x12\x0e\n\x06\x62\x61seid\x18\x01 \x01(\x05\x12\x0c\n\x04star\x18\x02 \x01(\x05\x12/\n\x05\x66rags\x18\x03 \x03(\x0b\x32 .msg.hero.NewHeroNotify.FragInfo:\x05\x88\xb5\x18\x83&\"C\n\x12HeroLevelupRequest\x12\x0e\n\x06heroid\x18\x01 \x01(\x04\x12\r\n\x05level\x18\x02 \x01(\x05:\x0e\x88\xb5\x18\x84&\x90\xb5\x18\x85&\x80\xa6\x1d\x02\"J\n\x13HeroLevelupResponse\x12\x0e\n\x06heroid\x18\x01 \x01(\x04\x12\r\n\x05level\x18\x02 \x01(\x05\x12\r\n\x05grade\x18\x03 \x01(\x05:\x05\x88\xb5\x18\x85&\"\x97\x01\n\x17HeroBreakThroughRequest\x12>\n\tbreaklist\x18\x01 \x03(\x0b\x32+.msg.hero.HeroBreakThroughRequest.BreakInfo\x1a,\n\tBreakInfo\x12\x0e\n\x06heroid\x18\x01 \x01(\x04\x12\x0f\n\x07\x63ostids\x18\x02 \x03(\x04:\x0e\x88\xb5\x18\x86&\x90\xb5\x18\x87&\x80\xa6\x1d\x02\"\xb2\x01\n\x18HeroBreakThroughResponse\x12\x39\n\x08herolist\x18\x01 \x03(\x0b\x32\'.msg.hero.HeroBreakThroughResponse.Hero\x12\x0f\n\x07\x63ostids\x18\x02 \x03(\x04\x12\x1e\n\x07rewards\x18\x03 \x03(\x0b\x32\r.msg.Resource\x1a#\n\x04Hero\x12\n\n\x02id\x18\x01 \x01(\x04\x12\x0f\n\x07quality\x18\x02 \x01(\x05:\x05\x88\xb5\x18\x87&\"1\n\x0fHeroLockRequest\x12\x0e\n\x06heroid\x18\x01 \x01(\x04:\x0e\x88\xb5\x18\x88&\x90\xb5\x18\x89&\x80\xa6\x1d\x02\":\n\x10HeroLockResponse\x12\x0e\n\x06heroid\x18\x01 \x01(\x04\x12\x0f\n\x07locking\x18\x02 \x01(\x08:\x05\x88\xb5\x18\x89&*\xa4\x03\n\rErrorcodeHero\x12\x0b\n\x07Success\x10\x00\x12\x16\n\x10LevelupInvalidID\x10\x81\x88L\x12\x14\n\x0eLevelupLimited\x10\x82\x88L\x12\x16\n\x10LevelupLimitHero\x10\x83\x88L\x12\x19\n\x13LevelupLimitQuality\x10\x84\x88L\x12\x16\n\x10LevelupConfError\x10\x85\x88L\x12\x14\n\x0eLevelupCostNot\x10\x86\x88L\x12\x1b\n\x15\x42reakThroughInvalidID\x10\x81\x8cL\x12\x1c\n\x16\x42reakThroughParamError\x10\x82\x8cL\x12\x1b\n\x15\x42reakThroughTypeError\x10\x83\x8cL\x12\x1b\n\x15\x42reakThroughConfError\x10\x84\x8cL\x12\x19\n\x13\x42reakThroughCostNot\x10\x85\x8cL\x12\x19\n\x13\x42reakThroughBagFull\x10\x86\x8cL\x12\x19\n\x13\x42reakThroughLimited\x10\x87\x8cL\x12\x1c\n\x16\x42reakThroughHeroLocked\x10\x88\x8cL\x12\x13\n\rLockInvalidID\x10\x81\x90Lb\x06proto3')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'hero_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _HEROLISTREQUEST._options = None
  _HEROLISTREQUEST._serialized_options = b'\210\265\030\201&\220\265\030\202&\200\246\035\002'
  _HEROLISTRESPONSE._options = None
  _HEROLISTRESPONSE._serialized_options = b'\210\265\030\202&'
  _NEWHERONOTIFY._options = None
  _NEWHERONOTIFY._serialized_options = b'\210\265\030\203&'
  _HEROLEVELUPREQUEST._options = None
  _HEROLEVELUPREQUEST._serialized_options = b'\210\265\030\204&\220\265\030\205&\200\246\035\002'
  _HEROLEVELUPRESPONSE._options = None
  _HEROLEVELUPRESPONSE._serialized_options = b'\210\265\030\205&'
  _HEROBREAKTHROUGHREQUEST._options = None
  _HEROBREAKTHROUGHREQUEST._serialized_options = b'\210\265\030\206&\220\265\030\207&\200\246\035\002'
  _HEROBREAKTHROUGHRESPONSE._options = None
  _HEROBREAKTHROUGHRESPONSE._serialized_options = b'\210\265\030\207&'
  _HEROLOCKREQUEST._options = None
  _HEROLOCKREQUEST._serialized_options = b'\210\265\030\210&\220\265\030\211&\200\246\035\002'
  _HEROLOCKRESPONSE._options = None
  _HEROLOCKRESPONSE._serialized_options = b'\210\265\030\211&'
  _ERRORCODEHERO._serialized_start=999
  _ERRORCODEHERO._serialized_end=1419
  _HEROLISTREQUEST._serialized_start=64
  _HEROLISTREQUEST._serialized_end=97
  _HEROLISTRESPONSE._serialized_start=99
  _HEROLISTRESPONSE._serialized_end=159
  _NEWHERONOTIFY._serialized_start=162
  _NEWHERONOTIFY._serialized_end=405
  _NEWHERONOTIFY_FRAGINFO._serialized_start=266
  _NEWHERONOTIFY_FRAGINFO._serialized_end=307
  _NEWHERONOTIFY_HEROFRAG._serialized_start=309
  _NEWHERONOTIFY_HEROFRAG._serialized_end=398
  _HEROLEVELUPREQUEST._serialized_start=407
  _HEROLEVELUPREQUEST._serialized_end=474
  _HEROLEVELUPRESPONSE._serialized_start=476
  _HEROLEVELUPRESPONSE._serialized_end=550
  _HEROBREAKTHROUGHREQUEST._serialized_start=553
  _HEROBREAKTHROUGHREQUEST._serialized_end=704
  _HEROBREAKTHROUGHREQUEST_BREAKINFO._serialized_start=644
  _HEROBREAKTHROUGHREQUEST_BREAKINFO._serialized_end=688
  _HEROBREAKTHROUGHRESPONSE._serialized_start=707
  _HEROBREAKTHROUGHRESPONSE._serialized_end=885
  _HEROBREAKTHROUGHRESPONSE_HERO._serialized_start=843
  _HEROBREAKTHROUGHRESPONSE_HERO._serialized_end=878
  _HEROLOCKREQUEST._serialized_start=887
  _HEROLOCKREQUEST._serialized_end=936
  _HEROLOCKRESPONSE._serialized_start=938
  _HEROLOCKRESPONSE._serialized_end=996
# @@protoc_insertion_point(module_scope)
