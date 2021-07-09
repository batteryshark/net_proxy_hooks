#include <Windows.h>
#include <wininet.h>

#include "utils.h"


BOOL HK_HttpSendRequestA(HINTERNET hRequest,LPCSTR lpszHeaders,DWORD dwHeadersLength,LPVOID lpOptional,DWORD dwOptionalLength){
    // Back up Old Proxy Settings
    INTERNET_PROXY_INFO old_proxy_info;
    DWORD old_proxy_size = sizeof(INTERNET_PROXY_INFO);
    BOOL no_proxy_found = !InternetQueryOptionW(hRequest, INTERNET_OPTION_PROXY, &old_proxy_info, &old_proxy_size) || old_proxy_info.dwAccessType == INTERNET_OPEN_TYPE_DIRECT;

    // Set Our Proxy
    InternetSetOptionW(hRequest,INTERNET_OPTION_PROXY,&proxy_info,sizeof(PROXY_INFO));

    // Send Our Request
    BOOL result = HttpSendRequestA(hRequest,lpszHeaders,dwHeadersLength,lpOptional,dwOptionalLength);

    // If there was a proxy set originally, put it back (shhhh :3 )
    if(!no_proxy_found){ InternetSetOptionW(hRequest,INTERNET_OPTION_PROXY,&old_proxy_info,old_proxy_size);}

    return result;
}

BOOL HK_HttpSendRequestW(HINTERNET hRequest,LPCWSTR lpszHeaders,DWORD dwHeadersLength,LPVOID lpOptional,DWORD dwOptionalLength){
    // Back up Old Proxy Settings
    INTERNET_PROXY_INFO old_proxy_info;
    DWORD old_proxy_size = sizeof(INTERNET_PROXY_INFO);
    BOOL no_proxy_found = !InternetQueryOptionW(hRequest, INTERNET_OPTION_PROXY, &old_proxy_info, &old_proxy_size) || old_proxy_info.dwAccessType == INTERNET_OPEN_TYPE_DIRECT;

    // Set Our Proxy
    InternetSetOptionW(hRequest,INTERNET_OPTION_PROXY,&proxy_info,sizeof(PROXY_INFO));

    // Send Our Request
    BOOL result = HttpSendRequestW(hRequest,lpszHeaders,dwHeadersLength,lpOptional,dwOptionalLength);

    // If there was a proxy set originally, put it back (shhhh :3 )
    if(!no_proxy_found){ InternetSetOptionW(hRequest,INTERNET_OPTION_PROXY,&old_proxy_info,old_proxy_size);}

    return result;
}

BOOL HK_HttpSendRequestExA(HINTERNET hRequest,LPINTERNET_BUFFERSA lpBuffersIn,LPINTERNET_BUFFERSA lpBuffersOut,DWORD dwFlags,DWORD_PTR dwContext){
    // Back up Old Proxy Settings
    INTERNET_PROXY_INFO old_proxy_info;
    DWORD old_proxy_size = sizeof(INTERNET_PROXY_INFO);
    BOOL no_proxy_found = !InternetQueryOptionW(hRequest, INTERNET_OPTION_PROXY, &old_proxy_info, &old_proxy_size) || old_proxy_info.dwAccessType == INTERNET_OPEN_TYPE_DIRECT;

    // Set Our Proxy
    InternetSetOptionW(hRequest,INTERNET_OPTION_PROXY,&proxy_info,sizeof(PROXY_INFO));

    // Send Our Request
    BOOL result = HttpSendRequestExA(hRequest,lpBuffersIn,lpBuffersOut,dwFlags,dwContext);

    // If there was a proxy set originally, put it back (shhhh :3 )
    if(!no_proxy_found){ InternetSetOptionW(hRequest,INTERNET_OPTION_PROXY,&old_proxy_info,old_proxy_size);}

    return result;
}


BOOL HK_HttpSendRequestExW(HINTERNET hRequest,LPINTERNET_BUFFERSW lpBuffersIn,LPINTERNET_BUFFERSW lpBuffersOut,DWORD dwFlags,DWORD_PTR dwContext){
    // Back up Old Proxy Settings
    INTERNET_PROXY_INFO old_proxy_info;
    DWORD old_proxy_size = sizeof(INTERNET_PROXY_INFO);
    BOOL no_proxy_found = !InternetQueryOptionW(hRequest, INTERNET_OPTION_PROXY, &old_proxy_info, &old_proxy_size) || old_proxy_info.dwAccessType == INTERNET_OPEN_TYPE_DIRECT;

    // Set Our Proxy
    InternetSetOptionW(hRequest,INTERNET_OPTION_PROXY,&proxy_info,sizeof(PROXY_INFO));

    // Send Our Request
    BOOL result = HttpSendRequestExW(hRequest,lpBuffersIn,lpBuffersOut,dwFlags,dwContext);

    // If there was a proxy set originally, put it back (shhhh :3 )
    if(!no_proxy_found){ InternetSetOptionW(hRequest,INTERNET_OPTION_PROXY,&old_proxy_info,old_proxy_size);}

    return result;
}


// Entry-Point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DBG_printf("WebHook Startup...!");
        Hook_IAT_Name("wininet.dll","HttpSendRequestA",  &HK_HttpSendRequestA);
        Hook_IAT_Name("wininet.dll","HttpSendRequestW",  &HK_HttpSendRequestW);
        Hook_IAT_Name("wininet.dll","HttpSendRequestExA",&HK_HttpSendRequestExA);
        Hook_IAT_Name("wininet.dll","HttpSendRequestExW",&HK_HttpSendRequestExW);
        return TRUE;
    }
    return TRUE;
}
