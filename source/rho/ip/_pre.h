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

extern const int kSigPipeIgnoreKickoff;

#elif __MINGW32__

#define  NTDDI_VERSION  NTDDI_WINXPSP1
#include <winsock2.h>
#include <ws2tcpip.h>

extern const int kInitForWindowsKickoff;

#else

#error What platform are you on!?

#endif


#endif     // __rho_ip_pre_h__
