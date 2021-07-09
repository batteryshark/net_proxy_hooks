#pragma once
#include <winsock2.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
bool SOCKS5__Greeting(SOCKET s, int use_async);
bool SOCKS5__ConnectRequest(SOCKET s, const struct sockaddr* d, int use_async);
bool SOCKS5__Test(const struct sockaddr* dest, const struct sockaddr* proxy);
#ifdef __cplusplus
}
#endif


