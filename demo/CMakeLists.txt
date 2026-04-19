cmake_minimum_required(VERSION 3.0...9.0)

#gui库的包含目录位置
set(include ../include)



#一个简单的helloworld
project(helloWorld)
file(GLOB src helloWorld/*.*)
add_executable(helloWorld WIN32 ${src})
target_include_directories(helloWorld PRIVATE  ${include})
add_ezui_lib(helloWorld)
set_property(TARGET helloWorld PROPERTY FOLDER "demo")


#资源打包器
project(ResPackage)
file(GLOB src ./ResPackage/*.*)
add_executable(ResPackage WIN32 ${src} )
target_include_directories(ResPackage PRIVATE  ${include})
add_ezui_lib(ResPackage)


#XML预览工具
project(XMLPreview)
file(GLOB src ./XMLPreview/*.*)
add_executable(XMLPreview WIN32 ${src} )
target_include_directories(XMLPreview PRIVATE  ${include})
add_ezui_lib(XMLPreview)
	
#UI速查
project(QuickUI)
    #主程序
file(GLOB src CONFIGURE_DEPENDS
    "./QuickUI/*.h"
    "./QuickUI/*.hpp"
    "./QuickUI/*.c"
    "./QuickUI/*.cpp"
    "./QuickUI/*.rc"
    )	
	
add_executable(QuickUI WIN32 ${src})
target_include_directories(QuickUI PRIVATE  ${include})
add_ezui_lib(QuickUI)
# 添加资源文件到"Res"筛选项下
file(GLOB_RECURSE res_files CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/QuickUI/res/*.*")
target_sources(QuickUI PRIVATE ${res_files})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/QuickUI/res" PREFIX "res" FILES ${res_files})
add_resource_package(QuickUI ./QuickUI/res ./QuickUI/QuickUI.res)
set_property(TARGET QuickUI PROPERTY FOLDER "demo")



#QQ音乐
project(QQMusic)
    #主程序
file(GLOB src CONFIGURE_DEPENDS
    "./QQMusic/*.h"
    "./QQMusic/*.hpp"
    "./QQMusic/*.c"
    "./QQMusic/*.cpp"
    "./QQMusic/*.rc"
    )	
	
add_executable(QQMusic WIN32 ${src})
target_include_directories(QQMusic PRIVATE  ${include})
add_ezui_lib(QQMusic)
# 添加资源文件到"Res"筛选项下
file(GLOB_RECURSE res_files CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/QQMusic/res/*.*")
target_sources(QQMusic PRIVATE ${res_files})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/QQMusic/res" PREFIX "res" FILES ${res_files})
add_resource_package(QQMusic ./QQMusic/res ./QQMusic/QQMusic.res)
set_property(TARGET QQMusic PROPERTY FOLDER "demo")
#添加常用库依赖
target_include_directories(QQMusic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/kugou/include/Common/include)
target_include_directories(QQMusic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/kugou/include/vlc/include)
#添加vlc解码器依赖
target_link_directories(QQMusic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/kugou/lib)
target_link_directories(QQMusic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/kugou/lib)
#执行解压vlc的解码库
add_custom_command(
        TARGET QQMusic
        POST_BUILD
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/kugou/extract${ARCH_NAME}.bat" "$<TARGET_FILE_DIR:QQMusic>"
        COMMENT "Running extract.bat to conditionally extract dll.zip"
)


#迅雷下载 Demo
project(Xunlei)
file(GLOB src CONFIGURE_DEPENDS
    "./Xunlei/*.h"
    "./Xunlei/*.hpp"
    "./Xunlei/*.c"
    "./Xunlei/*.cpp"
    "./Xunlei/*.rc"
    )

add_executable(Xunlei WIN32 ${src})
target_include_directories(Xunlei PRIVATE ${include})
add_ezui_lib(Xunlei)
file(GLOB_RECURSE res_files CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Xunlei/res/*.*")
target_sources(Xunlei PRIVATE ${res_files})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Xunlei/res" PREFIX "res" FILES ${res_files})
add_resource_package(Xunlei ./Xunlei/res ./Xunlei/Xunlei.res)
set_property(TARGET Xunlei PROPERTY FOLDER "demo")

	
#仿QQ登录界面
project(QQ)
    #主程序
file(GLOB src CONFIGURE_DEPENDS
    "./QQ/*.h"
    "./QQ/*.hpp"
    "./QQ/*.c"
    "./QQ/*.cpp"
    "./QQ/*.rc"
    )	
	
add_executable(QQTest WIN32 ${src})
target_include_directories(QQTest PRIVATE  ${include})
add_ezui_lib(QQTest)
# 添加资源文件到"Res"筛选项下
file(GLOB_RECURSE res_files CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/QQ/res/*.*")
target_sources(QQTest PRIVATE ${res_files})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/QQ/res" PREFIX "res" FILES ${res_files})
add_resource_package(QQTest ./QQ/res ./QQ/QQ.res)
set_property(TARGET QQTest PROPERTY FOLDER "demo")


    #模仿酷狗音乐demo
    project(kugou)
    #主程序
    file(GLOB src CONFIGURE_DEPENDS
    "./kugou/*.h"
    "./kugou/*.hpp"
    "./kugou/*.c"
    "./kugou/*.cpp"
    "./kugou/*.rc"
    )
    add_executable(kugou WIN32 ${src})

    # 添加资源文件到"Res"筛选项下
    file(GLOB_RECURSE res_files CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/kugou/res/*.*")
    target_sources(kugou PRIVATE ${res_files})
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/kugou/res" PREFIX "res" FILES ${res_files})

    target_include_directories(kugou PRIVATE  ${include})
    #添加常用库依赖
    target_include_directories(kugou PRIVATE ./kugou/include/Common/include)
    target_include_directories(kugou PRIVATE ./kugou/include/vlc/include)
    #添加vlc解码器依赖
    target_link_directories(kugou PRIVATE ./kugou/lib)
    target_link_directories(kugou PRIVATE ./kugou/lib)

    set_property(TARGET kugou PROPERTY FOLDER "demo")
    #添加引用
    add_ezui_lib(kugou)
    #解决 "模块对于 SAFESEH 映像是不安全的" 的报错
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    #指定为c++编译
    set_target_properties(kugou PROPERTIES LINKER_LANGUAGE CXX)
    target_compile_definitions(kugou PRIVATE WIN32_LEAN_AND_MEAN)

    #打包kugou/res文件夹下的所有资源 输出至kugou/kugou.res
    add_resource_package(kugou ./kugou/res ./kugou/kugou.res)

    #执行解压vlc的解码库
    add_custom_command(
        TARGET kugou
        POST_BUILD
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/kugou/extract${ARCH_NAME}.bat" "$<TARGET_FILE_DIR:kugou>"
        COMMENT "Running extract.bat to conditionally extract dll.zip"
    )
