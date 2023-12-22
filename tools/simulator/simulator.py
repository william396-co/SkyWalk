#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import readline
import codecs
from configparser import ConfigParser

import patches
patches.patch_prototype()

from crc16 import *
from message_manager import MessageManager
from gevent import sleep
from gevent.socket import wait_read
import login


class MessageCompleter(object):

    def __init__(self, message_manager):
        self.message_manager = message_manager

    def complete(self, text, state):
        text = text.strip()
        response = None
        if state == 0:
            self.matches = []
            if text:
                self.matches.extend(s for s in self.message_manager._fuzzy_search.keys() if s.startswith(text))
                self.matches.extend(f for f in os.listdir('.') if f.startswith(text) and f.endswith('.py'))
        try:
            response = self.matches[state]
        except:
            pass
        return response

def main():
    config = ConfigParser(inline_comment_prefixes=';')
    config.read_file(codecs.open('config.ini', 'r'))
    manager = MessageManager()
    manager._ignore_messages.extend(message.strip() for message in config.get('Message', 'ignore').split(','))
    manager._skip_fields.extend(field.strip() for field in config.get('Message', 'skip').split(','))
    manager.scan_messages('message/*.py')

    readline.parse_and_bind("tab: complete")
    readline.set_completer(MessageCompleter(manager).complete)
    history_file = config.get('History', 'file')
    open(history_file, 'w').close()
    readline.read_history_file(history_file)

    gateway_channel = login.login_verify_http(
            host=str(config.get('GatewayServer', 'host')),
            port=int(config.get('GatewayServer', 'port')),
            account=config.get('GatewayServer', 'account'),
            rolename=config.get('GatewayServer', 'rolename'),
            planet=int(config.get('GatewayServer', 'planet')),
            zoneid=int(config.get('GatewayServer', 'zoneid')),
            version=str(config.get('Global', 'version')),
            manager=manager)

    while True:
        sleep(0.1)
        message_name = ''
        try:
            print('GATESERVER:[%s::%d], ACCOUNT:[%s], ROLEID:[%d], ROLENAME:[%s], VERSION:[x.x.%s.%s],[%d.%d]' % (gateway_channel.host, gateway_channel.port, gateway_channel.account, gateway_channel.unitid, gateway_channel.rolename, gateway_channel.version, gateway_channel.actversion, gateway_channel.svnrevision, gateway_channel.excelversion ))
            message_name = input('Enter the MessageName(Full): ')
            message_name = message_name.strip()
            if message_name in manager._fuzzy_search.keys():
                message_full_names = manager._fuzzy_search[message_name]
                if len(message_full_names) > 1:
                    print("%s in %s means:%s" % (message_name, len(message_full_names), message_full_names))
                message_name = message_full_names[0]

            if message_name == 'msg.role.RoleLoginRequest':
                #gateway_channel.socket.shutdown( 2 )
                del gateway_channel

                gateway_channel = login.auto_login(
                        host = str(login.logininfo['host']),
                        port = int(login.logininfo['port']),
                        account=config.get('GatewayServer', 'account'),
                        rolename=config.get('GatewayServer', 'rolename'),
                        planet=int(config.get('GatewayServer', 'planet', 10001)),
                        zoneid=int(config.get('GatewayServer', 'zoneid', 0)),
                        version=str(config.get('Global', 'version', '10.10.10')),
                        token=str(login.logininfo['token']),
                        manager=manager,
                        method = 2)
            elif message_name in manager._message_classes.keys():
                message = manager.build_message(message_name)
                gateway_channel.send(message)
            elif message_name.startswith('#'):
                message_cls = manager._message_classes.get('msg.utils.ShellCommandMessage', None)
                if message_cls:
                    message = message_cls()
                    #message.channel = 1
                    message.content = message_name
                    gateway_channel.send(message)
                else:
                    print('gmcommand not support')
            elif message_name.startswith('$'):
                if message_name == '$sleep':
                    try:
                        while True:
                            sleep(1)
                    except:
                        pass
                elif message_name.startswith('$run '):
                    try:
                        file_name = message_name.split(' ')[1]
                        print('run:', file_name)
                        execfile(file_name)
                    except Exception as e:
                        print(e, type(e))
            else:
                if message_name:
                    print('%s: not found' % message_name)

        except EOFError as e:
            print('\n')
            exit()
        except Exception as e:
            print('error:', str(e))
            pass
        finally:
            readline.write_history_file(history_file)

if __name__ == "__main__":
    main()
