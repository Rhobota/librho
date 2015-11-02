#include "../_pre.h"
#include <rho/ip/udp/tSocket.h>
#include <rho/ip/ebIP.h>


namespace rho
{
namespace ip
{
namespace udp
{


#if __linux__ || __APPLE__ || __CYGWIN__
static const int kInvalidSocket = -1;
#elif __MINGW32__
static const int kInvalidSocket = INVALID_SOCKET;
#else
#error What platform are you on!?
#endif


tSocket::tSocket()
    : m_fd(kInvalidSocket)
{
}


tSocket::tSocket(u16 port)
    : m_fd(kInvalidSocket)
{
}


tSocket::tSocket(const tAddr& addr, u16 port)
    : m_fd(kInvalidSocket)
{
}


tSocket::~tSocket()
{
}


}  // namespace udp
}  // namespace ip
}  // namespace rho
