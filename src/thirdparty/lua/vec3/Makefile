
APPNAME	= Vector3.so

CC		= gcc
CXX		= g++

# 默认选项
LFLAGS		= -ggdb -lpthread
CFLAGS		= -Wall -Wformat=0 -ggdb -fPIC -O2
CXXFLAGS	= -Wall -Wformat=0 -ggdb -fPIC -O2

SOFLAGS		= -shared

# ------------------------------------------------------------------------------
OBJS 	= vec3.o

all : $(APPNAME)

$(APPNAME) : $(OBJS)
	$(CC) $(SOFLAGS) $(LFLAGS) $^ -o $@
clean :
	rm -rf *.o
	rm -rf $(APPNAME)

# --------------------------------------------------------
#
# gmake的规则
#
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@
%.o : %.cc
	$(CC) $(CFLAGS) -c $^ -o $@
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@
