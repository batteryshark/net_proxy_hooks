#pragma once

#define ENABLE_DEBUG
#define EXPORTABLE __declspec(dllexport)
#define GF_IFEXIST(x) if(x)GlobalFree(x)

BOOL Hook_IAT_Name (char* dll_name, char* func_name,DWORD replacement_function_ptr);
void DBG_printf(const char* format, ...);
void DBG_print_buffer(unsigned char* data, size_t length);
void* dyn_bind(const char* lib_name, const char* func_name);

// Some Proxy Internals and Settings
typedef struct {
    DWORD  dwAccessType;
    LPWSTR lpszProxy;
    LPWSTR lpszProxyBypass;
} PROXY_INFO;

#define WINHTTP_NO_PROXY_BYPASS NULL
#define WINHTTP_ACCESS_TYPE_NAMED_PROXY 3
#define PROXY_STRING L"http://10.0.0.131:55573"
#define PROXY_STRINGA "http://10.0.0.131:55573"
PROXY_INFO proxy_info;