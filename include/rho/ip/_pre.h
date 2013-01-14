#ifndef __rho_ip_pre_h__
#define __rho_ip_pre_h__


#if __linux__ || __APPLE__ || __CYGWIN__

#include <arpa/inet.h>           //
#include <errno.h>               //
#include <fcntl.h>               //
#include <netdb.h>               //
#include <netinet/tcp.h>         // posix headers
#include <signal.h>              //
#include <string.h>              //
#include <sys/socket.h>          //
#include <sys/types.h>           //
#include <unistd.h>              //

#elif __MINGW32__

#include <w32api.h>
#ifndef  _WIN32_WINNT
#define  _WIN32_WINNT    WindowsXP
#endif
#ifndef  _WIN32_WINDOWS
#define  _WIN32_WINDOWS  WindowsXP
#endif
#ifndef  WINVER
#define  WINVER          WindowsXP
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#elif

#error What platform are you on!?

#endif


#endif     // __rho_ip_pre_h__
