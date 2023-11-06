cmake_minimum_required(VERSION 3.5)

#gui库的包含目录位置
set(include ../src/include)

#一个简单的helloworld
project(helloWorld)
file(GLOB src helloWorld/*.*)
add_executable(helloWorld WIN32 ${src})
target_include_directories(helloWorld PRIVATE  ${include})
target_link_libraries(helloWorld PRIVATE EzUI)

#Designer设计器
project(designer)
file(GLOB src ./designer/*.*)
add_executable(designer WIN32 ${src} )
target_include_directories(designer PRIVATE  ${include})
target_link_libraries(designer PRIVATE EzUI)

#模仿酷狗音乐demo
project(kugouDemo)
#主程序
file(GLOB src ./kugou/*.*)
add_executable(kugouDemo WIN32 ${src})
#添加常用库依赖
target_include_directories(kugouDemo PRIVATE  ${include})
target_include_directories(kugouDemo PRIVATE ./kugou/lib/Common/include)
target_include_directories(kugouDemo PRIVATE ./kugou/lib/vlc/include)
#添加vlc解码器依赖
target_link_directories(kugouDemo PRIVATE ./kugou/lib/Common/lib)
target_link_directories(kugouDemo PRIVATE ./kugou/lib/vlc/lib)
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
target_compile_definitions(kugouDemo PRIVATE WIN32_LEAN_AND_MEAN)
#target_sources(kugouDemo PRIVATE ./kugouDemo/kugouDemo.rc)
