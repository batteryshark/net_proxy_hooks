#include <stdint.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include "../dbg/dbg_utils.h"

#include "net.h"





#define NS_INADDRSZ  4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ   2

int inet_pton4(const char *src, char *dst){
    uint8_t tmp[NS_INADDRSZ], *tp;

    int saw_digit = 0;
    int octets = 0;
    *(tp = tmp) = 0;

    int ch;
    while ((ch = *src++) != '\0')
    {
        if (ch >= '0' && ch <= '9')
        {
            uint32_t n = *tp * 10 + (ch - '0');

            if (saw_digit && *tp == 0)
                return 0;

            if (n > 255)
                return 0;

            *tp = n;
            if (!saw_digit)
            {
                if (++octets > 4)
                    return 0;
                saw_digit = 1;
            }
        }
        else if (ch == '.' && saw_digit)
        {
            if (octets == 4)
                return 0;
            *++tp = 0;
            saw_digit = 0;
        }
        else
            return 0;
    }
    if (octets < 4)
        return 0;

    memcpy(dst, tmp, NS_INADDRSZ);

    return 1;
}

int inet_pton6(const char *src, char *dst)
{
    static const char xdigits[] = "0123456789abcdef";
    uint8_t tmp[NS_IN6ADDRSZ];

    uint8_t *tp = (uint8_t*) memset(tmp, '\0', NS_IN6ADDRSZ);
    uint8_t *endp = tp + NS_IN6ADDRSZ;
    uint8_t *colonp = NULL;

    /* Leading :: requires some special handling. */
    if (*src == ':')
    {
        if (*++src != ':')
            return 0;
    }

    const char *curtok = src;
    int saw_xdigit = 0;
    uint32_t val = 0;
    int ch;
    while ((ch = tolower(*src++)) != '\0')
    {
        const char *pch = strchr(xdigits, ch);
        if (pch != NULL)
        {
            val <<= 4;
            val |= (pch - xdigits);
            if (val > 0xffff)
                return 0;
            saw_xdigit = 1;
            continue;
        }
        if (ch == ':')
        {
            curtok = src;
            if (!saw_xdigit)
            {
                if (colonp)
                    return 0;
                colonp = tp;
                continue;
            }
            else if (*src == '\0')
            {
                return 0;
            }
            if (tp + NS_INT16SZ > endp)
                return 0;
            *tp++ = (uint8_t) (val >> 8) & 0xff;
            *tp++ = (uint8_t) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }
        if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
                inet_pton4(curtok, (char*) tp) > 0)
        {
            tp += NS_INADDRSZ;
            saw_xdigit = 0;
            break; /* '\0' was seen by inet_pton4(). */
        }
        return 0;
    }
    if (saw_xdigit)
    {
        if (tp + NS_INT16SZ > endp)
            return 0;
        *tp++ = (uint8_t) (val >> 8) & 0xff;
        *tp++ = (uint8_t) val & 0xff;
    }
    if (colonp != NULL)
    {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int n = tp - colonp;

        if (tp == endp)
            return 0;

        for (int i = 1; i <= n; i++)
        {
            endp[-i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp)
        return 0;

    memcpy(dst, tmp, NS_IN6ADDRSZ);

    return 1;
}

int inet_pton(int af, const char *src, void *dst)
{
    switch (af)
    {
    case AF_INET:
        return inet_pton4(src, dst);
    case AF_INET6:
        return inet_pton6(src, dst);
    default:
        return -1;
    }
}

const char* inet_ntop(int af, const void* src, char* dst, int cnt){
 
    struct sockaddr_in srcaddr;
 
    memset(&srcaddr, 0, sizeof(struct sockaddr_in));
    memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));
 
    srcaddr.sin_family = af;
    if (WSAAddressToString((struct sockaddr*) &srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD) &cnt) != 0) {
        DWORD rv = WSAGetLastError();
        return NULL;
    }
    return dst;
}

	void Net__PrintSockaddr(const sockaddr* res) {
		struct sockaddr_in* addr_in = (struct sockaddr_in*)res;
		char* s = NULL;
		unsigned short port = 0;
		switch (res->sa_family) {
		case AF_INET: {
			struct sockaddr_in* addr_in = (struct sockaddr_in*)res;
			s = (char*)malloc(INET_ADDRSTRLEN);
			inet_ntop(AF_INET, &(addr_in->sin_addr), s, INET_ADDRSTRLEN);
			port = ntohs(addr_in->sin_port);
			break;
		}
		case AF_INET6: {
			struct sockaddr_in6* addr_in6 = (struct sockaddr_in6*)res;
			s = (char*)malloc(INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, &(addr_in6->sin6_addr), s, INET6_ADDRSTRLEN);
			port = ntohs(addr_in6->sin6_port);
			break;
		}
		default:
			break;
		}
		DbgUtils__dbg_printf("%s:%d", s, port);
		free(s);
	}

	bool Net__SocketSetBlocking(SOCKET s) {
		unsigned long mode = 0;
		int result = ioctlsocket(s, FIONBIO, &mode);
		if (result == SOCKET_ERROR) { return false; }
		return true;
	}

	bool Net__SocketSetNonBlocking(SOCKET s) {
		unsigned long mode = 1;
		int result = ioctlsocket(s, FIONBIO, &mode);
		if (result == SOCKET_ERROR) { return false; }
		return true;
	}

	// Connect to an IPv4 or IPv6 Address with a Given Socket (Supports Blocking and Non-Blocking).
	bool Net__ConnectSocket(SOCKET s, const sockaddr* d, unsigned int* is_non_blocking_socket) {
		*is_non_blocking_socket = 0;
		// Check if We Can Connect to the Proxy First.
		DbgUtils__dbg_printf("[Socket: %04X] Connecting to: ", s);
		Net__PrintSockaddr(d);
		int namelen = sizeof(sockaddr_in);
		if (d->sa_family == AF_INET6) {
			namelen = sizeof(sockaddr_in6);
		}
		if (connect(s, d, namelen) != SOCKET_ERROR) {
			DbgUtils__dbg_printf("Proxy Server Connected!");
			return true;
		}

		int ret = WSAGetLastError();
		if (ret != WSAEWOULDBLOCK) {
			DbgUtils__dbg_printf("Could Not Connect to SOCKS5 Proxy Server [%04X]", WSAGetLastError());
			return false;
		}

		// Everything Under Here Involves Non-Blocking Sockets
		fd_set Write, Err;
		FD_ZERO(&Write);
		FD_ZERO(&Err);
		FD_SET(s, &Write);
		FD_SET(s, &Err);

		TIMEVAL Timeout;
		Timeout.tv_sec = 10;
		Timeout.tv_usec = 0;

		ret = select(0, NULL, &Write, &Err, &Timeout);
		if (ret == SOCKET_ERROR) {
			DbgUtils__dbg_printf("Error in Select: %04X", WSAGetLastError());
			return false;
		}

		if (!ret) {
			DbgUtils__dbg_printf("Error in Select: WSAETIMEDOUT");
			return false;
		}



		if (FD_ISSET(s, &Err)) {
			int err;
			int err_len = sizeof(err);
			if (getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)& err, &err_len) == SOCKET_ERROR) {
				DbgUtils__dbg_printf("Error in getsockopt: %04X", WSAGetLastError());
				return false;
			}
			DbgUtils__dbg_printf("Error in FD_ISSET: %04X", (int)err);
			return false;
		}

		Net__SocketSetBlocking(s);
		*is_non_blocking_socket = 1;
		DbgUtils__dbg_printf("Proxy Server Connected [NonBlocking]!");
		return true;
	}

	// Get AF from Generic SockAddr
	bool Net__GetAF(const sockaddr* addr, unsigned int* paf) {
        if(!addr || !paf){return false;}
        *paf = addr->sa_family;
        return true;
    }
    
	// Get a Port Value from Generic SockAddr
	bool Net__GetPort(const sockaddr* addr, unsigned short* pport) {
		if (!addr) {
			DbgUtils__dbg_printf("Error (GetPort): addr is null");
			return false;
		}
		if (!pport) {
			DbgUtils__dbg_printf("Error (GetPort): pport is null");
			return false;
		}
		if (addr->sa_family == AF_INET) {
			const sockaddr_in* in = (const sockaddr_in*)addr;
			*pport = ntohs(in->sin_port);
			return true;
		}
		else if (addr->sa_family == AF_INET6) {
			const sockaddr_in6* in = (const sockaddr_in6*)addr;
			*pport = ntohs(in->sin6_port);
			return true;
		}
		else {
			DbgUtils__dbg_printf("Error [GetPort]: Unrecognized AF (%04X)", addr->sa_family);
		}
		return false;
	}

	bool GetAddressFamily(const char* host_addr, ADDRESS_FAMILY* paf) {
		unsigned char tbuff[32] = { 0x00 };

		if (inet_pton(AF_INET, host_addr, tbuff) == 1) { *paf = AF_INET; return true; }
		if (inet_pton(AF_INET6, host_addr, tbuff) == 1) { *paf = AF_INET6; return true; }

		return false;
	}

	// Interpret some data to create a SOCKADDR_IN/6 Structure.
	bool Net__CreateSockAddr(const char* host_addr, const char* host_port, sockaddr** addr) {
		unsigned short port = atoi(host_port) & 0xFFFF;
		ADDRESS_FAMILY af = 0;
		if (!GetAddressFamily(host_addr, &af)) {
			DbgUtils__dbg_printf("[CreateSockAddr]: Error Getting Address Family.");
			return false;
		}

		if (af == AF_INET) {
			*addr = (sockaddr*)calloc(1, sizeof(sockaddr_in));
			sockaddr_in* in = (sockaddr_in*)* addr;
			inet_pton(af, host_addr, &in->sin_addr);
			in->sin_port = htons(port);
			in->sin_family = af;
		}
		else if (af == AF_INET6) {
			*addr = (sockaddr*)calloc(1, sizeof(sockaddr_in6));
			sockaddr_in6* in = (sockaddr_in6*)* addr;
			inet_pton(af, host_addr, &in->sin6_addr);
			in->sin6_port = htons(port);
			in->sin6_family = af;
		}
		else {
			DbgUtils__dbg_printf("[CreateSockAddr]: Error - Unrecognized AF.");
			return false;
		}
		return true;
	}

	bool Net__IsSocket(SOCKET s) {
		int socktype = NULL;
		int optlen = sizeof(socktype);
		if (!getsockopt(s, SOL_SOCKET, SO_TYPE, (char*)& socktype, &optlen)) { return true; }
		//DbgUtils__dbg_printf("[IsSocket] Fail for %04X: %04X", s, WSAGetLastError());
		return false;
	}

	// Connect TCP Socket to a Given Address
	bool Net__ConnectTCP(const struct sockaddr* dest, SOCKET* s) {
		*s = SOCKET_ERROR;
		*s = socket(dest->sa_family, SOCK_STREAM, IPPROTO_TCP);
		if (*s == SOCKET_ERROR) {return false;}

		int namelen = sizeof(sockaddr_in);
		if (dest->sa_family == AF_INET6) {
			namelen = sizeof(sockaddr_in6);
		}
		if (connect(*s, dest, namelen) != SOCKET_ERROR) {
			return true;
		}
		return false;
	}


	bool Net__ConvertAF(SOCKET s, int dest_af) {
		WSAPROTOCOL_INFOW protocolInfo;

		int protocolInfoSize = sizeof(protocolInfo);

		if (0 != getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFOW, (char*)& protocolInfo, &protocolInfoSize))
		{
			return false;
		}
		if (protocolInfo.iAddressFamily != dest_af) {
			DbgUtils__dbg_printf("[Socket] AF Mismatch - Altering...");
			protocolInfo.iAddressFamily = dest_af;
			if (0 != setsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFOW, (char*)& protocolInfo, protocolInfoSize)) {
				return false;
			}
		}



		return true;
	}

