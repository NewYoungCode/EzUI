cmake_minimum_required(VERSION 3.0...4.0)

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

if (WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(STATUS "Windows x86 (32-bit) build detected")
    #模仿酷狗音乐demo
    project(kugou)
    
    #主程序
    file(GLOB src ./kugou/*.*)
    add_executable(kugou WIN32 ${src})

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
    target_compile_definitions(kugou PRIVATE WIN32_LEAN_AND_MEAN)

    #打包kugou/res文件夹下的所有资源 输出至kugou/kugou.res
    add_resource_package(kugou ./kugou/res ./kugou/kugou.res)

    #执行解压vlc的解码库
    add_custom_command(
        TARGET kugou
        POST_BUILD
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/kugou/extract.bat" "$<TARGET_FILE_DIR:kugou>"
        COMMENT "Running extract.bat to conditionally extract dll.zip"
    )

endif()





