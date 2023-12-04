# SkyWalk
This is a Game Server FrameWork use by Modern C++


PRINCIPLE:
	COMPLETE IS BETTER THAN PERFECT
	MADE IT BETTER THAN BEFORE


STEP BY STEP:

First Stage(Basic): 
	1 made it run(delete some unuseful/bad-smell/olde-style code pieces like pthread Unordered_Map and so on)
	2 add necessary component(io/framework/log/protobuf/json/xml/redis/mysql)
	3 some useful code(base/utils/domain/thirdparty)
	4 gate/gs/db/world/master
	5 server framework

Second Stage(Refactor):
	1 use modern cpp style modify some code
	2 replace log library(or rewrite)
	3 lua/protobuf/json/xml 
	4 sql/redis interface
	5 framework refactor
	6 refactor dbserver use by redis+mysql+caches

Third Stage(Promotion):
	1 use your own net library(cross platform, iocp/epoll/kcp/quic and so on)
	2 use your owner framework
	3 made framework changeable/reuseable/adapterable
