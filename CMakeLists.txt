cmake_minimum_required(VERSION 3.2)

if(UNIX)
   add_compile_options(-finput-charset=UTF-8)
   add_compile_options(-fexec-charset=UTF-8)
elseif(WIN32)
   add_compile_options(/utf-8)
endif()

set(tag $<IF:$<CONFIG:Debug>,Debug,Release>)

#让所有项目都使用UNICODE
add_definitions(-D_UNICODE -DUNICODE)

project(EzUI)
file(GLOB src ./include/EzUI/*.* ./sources/*.* )
# 添加一个选项，供用户选择构建静态库或动态库
option(BUILD_SHARED_LIBS "Build shared library instead of static library" OFF)
if(BUILD_SHARED_LIBS)
    add_library(EzUI SHARED ${src})
else()
    add_library(EzUI STATIC ${src})
endif()
target_include_directories(EzUI PRIVATE ./include/EzUI)
set_target_properties(EzUI PROPERTIES LINKER_LANGUAGE CXX)
#set_target_properties(EzUI PROPERTIES DEBUG_POSTFIX "_D")

#添加子项目
add_subdirectory(./demo)
