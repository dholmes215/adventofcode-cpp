add_library(md5 md5.c md5.h)
target_include_directories(md5 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
