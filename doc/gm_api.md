# YYH-GM工具API

### 1.角色封号

##### 请求格式:POST
* url: /role/banned?roleid=&method=&seconds=
* roleid:角色id
* method:方式1-查询;2-更新
* seconds:封号时间(单位:秒)

##### 返回格式:json

* result : 操作结果,0-成功
* roleid : 角色ID
* expiretime: 封号解禁时间

> 范例 : {"result":0,"roleid":111111111,"expiretime":1510041452}

-------

### 2.角色禁言

##### 请求格式:POST
* url :  /role/shutup?roleid=&method=&seconds=
* roleid:禁言角色id
* method:方式1-查询;2-更新
* seconds:禁言时间(单位:秒)

##### 返回格式:json

* result : 操作结果,0-成功
* roleid:角色id
* expiretime:禁言解禁时间

> 范例:{"result":0,"roleid":111111111,"expiretime":1510041452}

-------

### 3.加载配置

##### 请求格式:POST
* url :  /reload?method=
* method:0-非强更 1-资源强制更新 2-运营活动强制更新 3-运营活动和资源都强制更新

##### 返回格式:json
> 范例:{"result":0}

-------

###4.刷新指定角色数据

##### 请求格式:POST
* url: /role/flush?roleid=&dropcache=&tables=
* roleid:角色id
* tables:更新的表名, JSON数组
* dropcache:是否丢弃缓存数据

##### 返回格式:json
> 范例:{"result":0}

-------

### 5.查询角色数据

##### 请求格式:POST
* url: /role/query?roleid=&rolename=&tablename=&schemever=
* roleid:修改角色相关数据
* rolename: 角色名(可选)
* tablename:表名
* schemever:scheme版本号
##### 返回格式:StreamBuf
> 范例: roleid(uint64)，tablename(string)，result(int), datas(slice)
> 
> roleid  	- 查询的角色ID
> tablename	- 查询的表名
> result 	- 操作是否成功 0表示成功 >0errocode
> datas     - 返回结果是二进制数据
-------

### 6.修改角色数据

##### 请求格式:POST
* url: /role/modify?roleid=&tablename=&method=&schemever=
* roleid:修改角色相关数据
* tablename:表名
* method: 1 增加，3 更新， 4 删除
* schemever:scheme脚本版本号
* HTTP_BODY: 用于存放单行记录，StreamBuf序列化的二进制数据

##### 返回格式:json
> 范例:{"result":0}

-------


###7.获取服务器状态

##### 请求格式:POST
* url:  /status

##### 返回格式:json
> 范例: 咨询张磊

-------

###8.添加邮件

##### 请求格式:POST
* url:  /mail/add?id=&starttime=&expiretime&condition=&tag=&keepdays=&sendername=&title=&content=&attachment=
* id:邮件id
* starttime:开始时间
* expiretime:过期时间
* condition:条件(JSON) { "type":1, "condition":"" }
    * 空:无条件
    * type=1,在线玩家
    * type=2,指定角色列表;condition:"[roleid1,roleid2....]"
    * type=3,指定条件, 待定
    * type=4,指定公会ID;condition:"[guildid1,guildid2...]"
* tag:邮件标签
* keepdays:保持天数
* sendername:发送者名字
* title:标题
* content:内容
* attachment:附件内容,JSON数组
    * type:资源类型，1-物品；3-伙伴；4-属性；5-效果类资源
    * object:资源ID
    * count:资源数量

##### 返回格式: json
> 范例:{"result":0}

-------

###9.删除邮件

##### 请求格式:POST
* url: /mail/remove?id=
* id:邮件id

##### 返回格式: json
> 范例:{"result":0}

-------

###10.添加公告

##### 请求格式:POST
* url: /bulletin/add?id=&style=&interval=&priority=&cycletimes=&showplace=&starttime=&endtime=&content=&paramlist=
* id:公告id
* style:样式
* interval:发送间隔
* priority:优先级
* cycletimes:轮播次数
* showplace:显示位置
* starttime:开始时间
* endtime:结束时间
* content:消息内容
* paramlist:参数列表

##### 返回格式:json
> 范例:{"result":0, "id":}

-------

###11.删除公告

##### 请求格式:POST
* url: /bulletin/remove?id=
* id:公告id

##### 返回格式:json
> 范例:{"result":0, "id":}

-------

### 12.查询角色列表

##### 请求格式:POST
* url: /role/acquire?roleid=&rolename=
* roleid: 角色ID(可选)
* rolename: 角色名(可选)
##### 返回格式: Json

> 范例:{"result":0, "rolelist":[{"roleid":17593259787179,"zoneid":1,"account":"ryan0508","rolename":"ryan0508","level":1,"viplevel":0,"createtime":1651978179,"logintime":1651978180}] }
> rolelist - 数组
> 	roleid:
> 	zoneid:
> 	account:
> 	rolename:
> 	level:
> 	viplevel:
> 	createtime:
>	logintime:

-------

### 13.发送GM指令

##### 请求格式:POST

- url: /shellcmd?cmd=&roleid=
- cmd:GM指令
- roleid:角色ID

##### 返回格式:json

> 范例:{"result":0, "roleid":0, "content":""}

------

### 14.更新调试账号

##### 请求格式:POST

- url: /debugmode?account=&password=&debugseconds=
- account:账号LID
- password:调试密码
- debugseconds:调试时间

##### 返回格式:json

> 范例:{"result":0 }

-------
