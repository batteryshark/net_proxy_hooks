#include <windows.h>

#include "utils.h"

typedef int CURLcode;
typedef int CURLoption;

#define CURLOPT_PROXY 4

typedef CURLcode (*pcurl_easy_setopt)(void *handle, CURLoption option, char *proxy);
typedef CURLcode (*pcurl_easy_perform)(void * easy_handle );
pcurl_easy_setopt curl_easy_setopt = NULL;
pcurl_easy_perform curl_easy_perform = NULL;




// TODO: Save original Proxy.
// TODO: Additional Methods to Intercept
CURLcode HK_curl_easy_perform(void * easy_handle ){
    curl_easy_setopt(easy_handle, CURLOPT_PROXY, PROXY_STRINGA);
    return curl_easy_perform(easy_handle);
}


// Entry-Point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DBG_printf("WebHook Startup...!");
        curl_easy_setopt = dyn_bind("libcurl.dll","curl_easy_setopt");
        curl_easy_perform = dyn_bind("libcurl.dll","curl_easy_perform");
        // Silently Die if Not
        if(!curl_easy_setopt){return TRUE;}
        Hook_IAT_Name("libcurl.dll","curl_easy_perform",  &HK_curl_easy_perform);

    }
    return TRUE;
}
