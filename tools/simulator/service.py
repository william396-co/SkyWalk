#!/usr/bin/env python
# -*- coding: utf-8 -*-

import struct
import binascii
import json
import time
import importlib

from crc16 import *
from google.protobuf import text_format

import gevent
from gevent import core
from gevent import Greenlet
from gevent import Timeout
from gevent import socket
from gevent import sleep
from gevent.hub import get_hub
from gevent.event import AsyncResult
from gevent.event import Event
from gevent.baseserver import BaseServer

from prototype import type_pb2
from prototype import utils_pb2
from pb_to_dict import protobuf_to_dict

transitmessage=[]

class BaseAsyncResult(AsyncResult):

    def __init__(self, response_class, *args, **kwargs):
        self.response_class = response_class
        super(BaseAsyncResult, self).__init__(*args, **kwargs)

    def set(self, value):
        response = self.response_class()
        response.ParseFromString(value)
        super(BaseAsyncResult, self).set(response)


class GatewayChannel(object):
    def __init__(self, host=None, port=None, service=None, **kwargs):
        self.pending_response = {}
        self.host = host
        self.port = port
        self.hub = get_hub()
        self.loop = self.hub.loop
        self.message_buf = bytes()
        self.max_length = 5000
        self.header_length = struct.calcsize('!HHH')
        self.socket = None
        self.unitid = 0
        self.message_manager = kwargs.pop('message_manager', None)
        gevent.spawn(self._heartbeat)

    def check_connection(self):
        if self.socket is None:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.setblocking(0)
            while True:
                result = self.socket.connect_ex((self.host, self.port))
                if result == 0 or result == socket.EALREADY: # connected
                    break
                if result == socket.EINPROGRESS: # connection in progress
                    continue
                else:
                    raise Exception('%d: connection failed' % result)
            self._read_watcher = self.loop.io(self.socket.fileno(), 1) # read
            self._read_watcher.start(self._do_read)

    def _do_read(self):
        self.message_buf += self.socket.recv(self.max_length)
        while len(self.message_buf) >= self.header_length:
            self._do_handle()

    def print_message(self, message, prefix='Message:', **kwargs):
        message_dict = protobuf_to_dict(message)
        message_dict.update(kwargs)
        print(prefix, message.DESCRIPTOR.name, json.dumps(message_dict, indent=4, ensure_ascii=False))

    def get_checksum(self, message):
        checksum = binascii.crc32( message ) & 0xffffffff
        checksum = checksum % 8520
        checksum = checksum * 1087
        checksum = checksum % 255
        return checksum

    def _do_handle(self):
        """
        there is no label/tag or any other thing to ensure wether message is a request or response.
        so, check response first, otherwise, check handle as request
        """
        head_buf = self.message_buf[:self.header_length]
        message_id, flags, length = struct.unpack_from('!HHH', head_buf)
        if length + self.header_length > len(self.message_buf):
            return
        body_buf = self.message_buf[self.header_length: self.header_length + length]
        self.message_buf = self.message_buf[self.header_length + length:]
        if message_id == type_pb2.Errorcode_S: # error
            error_message = utils_pb2.ErrorcodeNotify()
            error_message.ParseFromString(body_buf)
            result_name = self.message_manager._errors.get(error_message.result, hex(error_message.result))
            self.print_message(error_message, 'Error:', result=result_name)
        else:
            response = self.pending_response.get(message_id, None)
            if response:
                response.set(body_buf)
            else:
                if self.message_manager is None:
                    print("Response:", (hex(message_id), "no 'message_manager' supplied"))
                    return
                message_cls = self.message_manager._message_ids.get(message_id, None)
                if message_cls is None:
                    return
                message = message_cls()
                message.ParseFromString(body_buf)
                self.print_message(message, 'Response:')
                #if message_id == battle_pb2.BattleDataNotify_S and message.port != 0:
                #    # 连接中转服务器
                #    self.transitchannel = TransitChannel( host=message.host, port=message.port, message_manager=self.message_manager )

    def send(self, request):
        self.check_connection()
        self.print_message(request, 'Request:')
        serializered_message = request.SerializeToString()
        flags = checksum( serializered_message )
        msgid = request.DESCRIPTOR.GetOptions().Extensions[type_pb2.msgid]
        head = struct.pack('!HHH', msgid, flags, len(serializered_message))
        try :
            self.socket.sendall(head + serializered_message)
        except BlockingIOError as err :
            time.sleep(0.05)
            self.socket.sendall(head + serializered_message)

    def _heartbeat(self):
        while True :
            if self.unitid != 0 :
                ping = utils_pb2.UtilsPingMessage()
                msgid = ping.DESCRIPTOR.GetOptions().Extensions[type_pb2.msgid]
                ping.timestamp = int(time.time())
                ping_message = ping.SerializeToString()
                flags = checksum( ping_message )
                head = struct.pack('!HHH', msgid, flags, len(ping_message))
                self.check_connection()
                self.socket.sendall(head + ping_message)
            sleep(5)
