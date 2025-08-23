cmake_minimum_required(VERSION 3.0...4.0)

#gui库的包含目录位置
set(include ../include)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_NAME "x64")
else()
    set(ARCH_NAME "x86")
endif()


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

# 定义函数: 添加资源打包任务并关联到指定项目
function(add_resource_package TARGET_NAME INPUT_DIR OUTPUT_FILE)
    # 定义始终运行的伪目标
    add_custom_target(
        ${TARGET_NAME}_ResPackage  # 更简洁的目标命名
        COMMAND ${CMAKE_SOURCE_DIR}/ResPackage.exe -package ${CMAKE_CURRENT_SOURCE_DIR}/${INPUT_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${OUTPUT_FILE}
        COMMENT "Packaging resources for ${TARGET_NAME}..."
    )
    # 将目标归类到专用文件夹
    set_target_properties(${TARGET_NAME}_ResPackage PROPERTIES
        FOLDER "EzUIBuildUtilities"  # 在VS中归类到指定文件夹
    )
    # 主目标依赖伪目标
    add_dependencies(${TARGET_NAME} ${TARGET_NAME}_ResPackage)
endfunction()

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
    target_link_directories(kugou PRIVATE ./kugou/lib/${ARCH_NAME}/Common)
    target_link_directories(kugou PRIVATE ./kugou/lib/${ARCH_NAME}/vlc)

    #添加引用
    target_link_libraries(kugou PRIVATE EzUI)
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
