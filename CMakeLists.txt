cmake_minimum_required(VERSION 3.14)
project(ProxyHook C)

set(CMAKE_C_STANDARD 90)

add_library(WinHTTPProxy SHARED winhttp_hook.c utils.c utils.h)

set_target_properties(WinHTTPProxy PROPERTIES PREFIX "")
set_target_properties(WinHTTPProxy PROPERTIES OUTPUT_NAME "hk_winhttp")
set(CMAKE_C_FLAGS "-w -Wl,--kill-at")
target_link_libraries(WinHTTPProxy winhttp.lib)
set_target_properties(WinHTTPProxy
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )


add_library(WininetProxy SHARED wininet_hook.c utils.c utils.h)

set_target_properties(WininetProxy PROPERTIES PREFIX "")
set_target_properties(WininetProxy PROPERTIES OUTPUT_NAME "hk_wininet")
set(CMAKE_C_FLAGS "-w -Wl,--kill-at")
target_link_libraries(WininetProxy wininet.lib)
set_target_properties(WininetProxy
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )


add_library(CurlProxy SHARED curl_hook.c utils.c utils.h)

set_target_properties(CurlProxy PROPERTIES PREFIX "")
set_target_properties(CurlProxy PROPERTIES OUTPUT_NAME "hk_curl")
set(CMAKE_C_FLAGS "-w -Wl,--kill-at")
target_link_libraries(CurlProxy )
set_target_properties(CurlProxy
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )


