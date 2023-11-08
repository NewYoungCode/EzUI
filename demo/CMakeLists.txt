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
project(kugou)
#主程序
file(GLOB src ./kugou/*.*)
add_executable(kugou WIN32 ${src})
#程序输出目录
set_target_properties(kugou PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../${tag})
#添加常用库依赖
target_include_directories(kugou PRIVATE  ${include})
target_include_directories(kugou PRIVATE ./kugou/lib/Common/include)
target_include_directories(kugou PRIVATE ./kugou/lib/vlc/include)
#添加vlc解码器依赖
target_link_directories(kugou PRIVATE ./kugou/lib/Common/lib)
target_link_directories(kugou PRIVATE ./kugou/lib/vlc/lib)
#添加引用
target_link_libraries(kugou PRIVATE EzUI)
#解决 "模块对于 SAFESEH 映像是不安全的" 的报错
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
#指定为c++编译
set_target_properties(kugou PROPERTIES LINKER_LANGUAGE CXX)
#指定文件输出位置
# set_target_properties(kugou PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ./bin
# )
target_compile_definitions(kugou PRIVATE WIN32_LEAN_AND_MEAN)
#target_sources(kugou PRIVATE ./kugouDemo/kugouDemo.rc)
