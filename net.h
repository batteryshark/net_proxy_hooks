#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>

#define bswap16(x) ((unsigned short)((((unsigned short) (x) & 0xff00) >> 8) | \
                               (((unsigned short) (x) & 0x00ff) << 8)))

#define bswap32(x) ((unsigned int)((((unsigned int) (x) & 0xff000000) >> 24) | \
                               (((unsigned int) (x) & 0x00ff0000) >> 8) | \
                               (((unsigned int) (x) & 0x0000ff00) << 8) | \
                               (((unsigned int) (x) & 0x000000ff) << 24)))


#ifdef __cplusplus
extern "C" {
#endif
    bool Net__GetAF(const sockaddr* addr, unsigned int* paf);
	bool Net__SocketSetNonBlocking(SOCKET s);
	bool Net__SocketSetBlocking(SOCKET s);
	bool Net__ConnectSocket(SOCKET s, const sockaddr* d, unsigned int* is_non_blocking_socket);
	void Net__PrintSockaddr(const sockaddr* res);
	bool Net__GetPort(const sockaddr* addr, unsigned short* pport);
	bool Net__CreateSockAddr(const char* host_addr, const char* host_port, sockaddr** addr);
	bool Net__IsSocket(SOCKET s);
	bool Net__ConvertAF(SOCKET s, int dest_af);
	bool Net__ConnectTCP(const struct sockaddr* dest, SOCKET* s);
#ifdef __cplusplus
}
#endif


