
#include <Windows.h>
#include <winhttp.h>

#include "utils.h"


BOOL HK_WinHttpSendRequest(HINTERNET hRequest,LPCWSTR lpszHeaders,DWORD dwHeadersLength,LPVOID lpOptional,DWORD dwOptionalLength,DWORD dwTotalLength,DWORD_PTR dwContext) {
    // Back up Old Proxy Settings
    WINHTTP_PROXY_INFO old_proxy_info;
    DWORD old_proxy_size = sizeof(WINHTTP_PROXY_INFO);
    BOOL no_proxy_found = !WinHttpQueryOption(hRequest, WINHTTP_OPTION_PROXY, &old_proxy_info, &old_proxy_size) || old_proxy_info.dwAccessType == WINHTTP_ACCESS_TYPE_NO_PROXY;

    // Set Our Proxy
    WinHttpSetOption(hRequest,WINHTTP_OPTION_PROXY,&proxy_info,sizeof(PROXY_INFO));

    // Send Our Request
    BOOL result = WinHttpSendRequest(hRequest,lpszHeaders,dwHeadersLength,lpOptional,dwOptionalLength,dwTotalLength,dwContext);

    // If there was a proxy set originally, put it back (shhhh :3 )
    if(!no_proxy_found){ WinHttpSetOption(hRequest,WINHTTP_OPTION_PROXY,&old_proxy_info,old_proxy_size);}

    // Clean up After Ourselves:
    // When retrieving proxy data, an application must free the lpszProxy and lpszProxyBypass strings contained
    // in this structure (if they are non-NULL) using the GlobalFree function.
    // https://docs.microsoft.com/en-us/windows/desktop/WinHttp/option-flags
    GF_IFEXIST(old_proxy_info.lpszProxy);
    GF_IFEXIST(old_proxy_info.lpszProxyBypass);
    return result;
}

// Entry-Point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DBG_printf("WebHook Startup...!");
        Hook_IAT_Name("winhttp.dll","WinHttpSendRequest",&HK_WinHttpSendRequest);
        return TRUE;
    }
    return TRUE;
}
