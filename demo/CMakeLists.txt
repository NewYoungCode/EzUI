cmake_minimum_required(VERSION 3.5)

#gui库的包含目录位置
set(include ../include)

#一个简单的helloworld
project(helloWorld)
file(GLOB src helloWorld/*.*)
add_executable(helloWorld WIN32 ${src})
target_include_directories(helloWorld PRIVATE  ${include})
target_link_libraries(helloWorld PRIVATE EzUI)

#资源打包器
project(ResPackage)
file(GLOB src ./ResPackage/*.*)
add_executable(ResPackage WIN32 ${src} )
target_include_directories(ResPackage PRIVATE  ${include})
target_link_libraries(ResPackage PRIVATE EzUI)
set_target_properties(ResPackage PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../${tag})


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


set(commonInclude D:/C++/Common/include)
set(commonLib D:/C++/Common/lib)
#Android-tools
project(Android-tools)
file(GLOB src ./Android-tools/*.*)
add_executable(Android-tools WIN32 ${src} )
target_include_directories(Android-tools PRIVATE  ${include} ${commonInclude})
target_link_libraries(Android-tools PRIVATE EzUI)
set_target_properties(Android-tools PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../${tag})
set_target_properties(Android-tools PROPERTIES
    LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
)
target_link_directories(Android-tools PRIVATE ${commonLib})
target_link_libraries(Android-tools PRIVATE
    $<$<CONFIG:Debug>:D:/C++/Common/build/Debug/Common.lib>
    $<$<CONFIG:Release>:D:/C++/Common/build/Release/Common.lib>
)


