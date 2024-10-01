#include "tcpclient.h"

#ifdef PLATFORM_WINDOWS
#include "win/win.h"
#elif defined(PLATFORM_LINUX)
#include "lin/lin.h"
#elif defined(PLATFORM_MAC)
#include "mac/mac.h"
#endif

// Factory function that returns the correct implementation based on the OS
TCPClient *createClient()
{
#ifdef PLATFORM_WINDOWS
    return new WinClient();
#elif defined(PLATFORM_LINUX)
    return new LinClient();
#elif defined(PLATFORM_MAC)
    return new MacClient();
#endif
}
