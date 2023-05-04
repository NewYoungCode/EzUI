cmake_minimum_required(VERSION 3.2)

# if(UNIX)
#     add_compile_options(-finput-charset=UTF-8)
#     add_compile_options(-fexec-charset=UTF-8)
# elseif(WIN32)
#     add_compile_options(/utf-8)
# endif()

#让所有项目都使用UNICODE
add_definitions(-D_UNICODE -DUNICODE)

project(EzUI)
set(libBasePath ./EzUI)
include_directories(${libBasePath}/include)

file(GLOB ezuiHeads ${libBasePath}/include/*.h)

file(GLOB ezuiSrc ${libBasePath}/sources/*.cpp)
file(GLOB tinyxmlSrc ${libBasePath}/sources/tinyxml/*.cpp)
# target_include_directories(EzUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(EzUI STATIC ${ezuiSrc} ${tinyxmlSrc} ${ezuiHeads})
set_target_properties(EzUI PROPERTIES LINKER_LANGUAGE CXX)


#一个简单的helloworld
project(helloWorld)
add_executable(helloWorld WIN32 ./helloWorld/main.cpp)
target_link_libraries(helloWorld PRIVATE EzUI)
set_target_properties(helloWorld PROPERTIES LINKER_LANGUAGE CXX)


#Designer设计器
project(Designer)
file(GLOB DesignerSrc ./Designer/*.cpp)
file(GLOB DesignerH ./Designer/*.h)
add_executable(Designer WIN32 ${DesignerH} ${DesignerSrc})
target_link_libraries(Designer PRIVATE EzUI)
set_target_properties(EzUI PROPERTIES LINKER_LANGUAGE CXX)


#模仿酷狗音乐demo
project(kugouDemo)
include_directories(./kugou)
#添加常用库依赖
include_directories(./kugou/lib/Common/include)
include_directories(./kugou/lib/vlc/include)
#添加vlc解码器依赖
link_directories(./kugou/lib/Common/lib)
link_directories(./kugou/lib/vlc/lib)
#主程序
file(GLOB kugouHeads ./kugou/*.h)
file(GLOB kugouSrc ./kugou/*.cpp)
add_executable(kugouDemo WIN32 ${kugouSrc} ${kugouHeads})
#添加引用
target_link_libraries(kugouDemo PRIVATE EzUI)
#解决 "模块对于 SAFESEH 映像是不安全的" 的报错
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
#指定为c++编译
set_target_properties(kugouDemo PROPERTIES LINKER_LANGUAGE CXX)
#指定文件输出位置
# set_target_properties(kugou PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ./bin
# )
