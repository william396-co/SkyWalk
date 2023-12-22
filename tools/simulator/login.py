
import json
import patches
import urllib3
patches.patch_prototype()

from google.protobuf import text_format
from gevent import sleep

from service import GatewayChannel
from service import BaseAsyncResult

from message_manager import MessageManager
from prototype.role_pb2 import *
from prototype.type_pb2 import *

def auto_login(host, port, account, planet, rolename, zoneid, version, token, manager=None, method = 1):
    gateway_channel = GatewayChannel(message_manager=manager, host=host, port=port)

    role_list_id = RoleListResponse.DESCRIPTOR.GetOptions().Extensions[msgid]
    new_born_role_id = NewbornResponse.DESCRIPTOR.GetOptions().Extensions[msgid]
    role_list_result = BaseAsyncResult(RoleListResponse)
    new_born_role_result = BaseAsyncResult(NewbornResponse)

    gateway_channel.pending_response[role_list_id] = role_list_result
    gateway_channel.pending_response[new_born_role_id] = new_born_role_result

    message_role_login = RoleLoginRequest()
    message_role_login.account = account
    message_role_login.zoneid = zoneid
    message_role_login.token = token
    message_role_login.appversion = version
    gateway_channel.send(message_role_login)

    message_role_list = role_list_result.get()
    if len(message_role_list.rolelist) == 0:
        message_create_role = NewbornRequest()
        message_create_role.rolename = rolename
        message_create_role.account = account
        message_create_role.zoneid = zoneid
        message_create_role.device = ""
        message_create_role.version = version
        gateway_channel.send(message_create_role)
        message_new_born_role = new_born_role_result.get()
        gateway_channel.rolename = message_new_born_role.rolename
        gateway_channel.unitid = message_new_born_role.roleid
        gateway_channel.account = account
        gateway_channel.version = version
        gateway_channel.actversion = message_role_list.actversion
        gateway_channel.svnrevision = message_role_list.svnrevision
        gateway_channel.excelversion = message_role_list.excelversion
    else:
        role = message_role_list.rolelist[0]
        gateway_channel.rolename = role.rolename
        gateway_channel.unitid = role.roleid
        gateway_channel.account = account
        gateway_channel.version = version
        gateway_channel.actversion = message_role_list.actversion
        gateway_channel.svnrevision = message_role_list.svnrevision
        gateway_channel.excelversion = message_role_list.excelversion

    if method == 1 :
        message_role_enter_game = EnterGameRequest()
        message_role_enter_game.roleid = gateway_channel.unitid
        message_role_enter_game.device = '0'
        message_role_enter_game.version = 'x.x.x'
        gateway_channel.send(message_role_enter_game)

    gateway_channel.host = host
    gateway_channel.port = port
    gateway_channel.zoneid = zoneid
    gateway_channel.login_url = 'http://fight.o.kingnet.com/beta.php?c=test&a=test&server=%s&port=%s&oid2=%s' % (host, port, account)
    return gateway_channel


def login_verify_protobuf( host, port, account, planet, rolename, zoneid, version, manager=None ):
    login_channel = GatewayChannel(message_manager=manager, host=host, port=port)

    verify_id = VerifyResponse.DESCRIPTOR.fields_by_name['id'].default_value
    verify_result = BaseAsyncResult(VerifyResponse)
    login_channel.pending_response[verify_id] = verify_result

    message_verify = VerifyRequest()
    message_verify.account = account
    message_verify.zoneid = zoneid
    message_verify.token = '123456'
    message_verify.appversion = version
    login_channel.send(message_verify)
    message_verify_response = verify_result.get()
    login_channel.socket.shutdown( 2 )
    del login_channel
    gateway_channel = auto_login( message_verify_response.gatehost, message_verify_response.gateport, account, planet, rolename, zoneid, version, message_verify_response.token, manager )

    return gateway_channel

def login_verify_http( host, port, account, planet, rolename, zoneid, version, manager=None ):
    http = urllib3.PoolManager()
    r = http.request( 'GET',
            "http://%s:%d/account/verify?account=%s&token=%s&version=%s&zoneid=%d" % (host, port, account, '123456', version, zoneid) )
    result = json.loads( r.data.decode('utf-8') )['result']
    if result != 0 :
        print( "login failed, %d" % result )
        exit(-1)
    data = r.data.decode( 'utf-8' )
    account = json.loads( data )['account']
    token = json.loads( data )['token']
    gatehost = json.loads( data )['gateaddr']
    gateport = json.loads( data )['gateport']
    gateway_channel = auto_login( gatehost, gateport, account, planet, rolename, zoneid, version, token, manager )
    return gateway_channel
