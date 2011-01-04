/*仅仅包含系统文件，至于自定义的.h文件还是各自去处理吧*/

//ansi c
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <time.h>

//linux
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//lua
#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <lua5.1/lauxlib.h>

//libevent
#include <event.h>

#define MODE_LOGIN 1

#define EXIT_ERROR 1
