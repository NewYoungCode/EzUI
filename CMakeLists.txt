cmake_minimum_required(VERSION 3.0...9.0)

if(UNIX)
   add_compile_options(-finput-charset=UTF-8)
   add_compile_options(-fexec-charset=UTF-8)
elseif(WIN32)
   add_compile_options(/utf-8)
endif()

#强制检查架构类型
if(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
    set(ARCH_NAME "x64")
else()
    set(ARCH_NAME "x86")
endif()

#链接类型
set(LINK_TYPE "static")
#运行时
set(RUNTIME "MD")
#构建类型
set(BUILD_TYPE $<IF:$<CONFIG:Debug>,Debug,Release>)

#让所有项目都使用UNICODE
add_definitions(-D_UNICODE -DUNICODE)

project(EzUI_v2.0)

# 添加一个选项，供用户选择构建demo
option(BUILD_EZUI "Build BUILD_EZUI_PROJECT" ON)
if(BUILD_EZUI)
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
        else()
endif()


# 定义函数: 添加资源打包任务并关联到指定项目
function(add_resource_package TARGET_NAME INPUT_DIR OUTPUT_FILE)
    set (ProjectName  ${TARGET_NAME}_EzUI_ResourcePackager)
    # 定义始终运行的伪目标
    add_custom_target(
        ${ProjectName}  # 更简洁的目标命名
        COMMAND ${CMAKE_SOURCE_DIR}/ResPackage.exe -package ${CMAKE_CURRENT_SOURCE_DIR}/${INPUT_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${OUTPUT_FILE}
        COMMENT "Packaging resources for ${TARGET_NAME}..."
    )
    # 将目标归类到专用文件夹
    set_target_properties(${ProjectName} PROPERTIES
        FOLDER "CMakePredefinedTargets"  # 在VS中归类到指定文件夹
    )
    # 主目标依赖伪目标
    add_dependencies(${TARGET_NAME} ${ProjectName})
endfunction()


# 定义函数:添加ezui的inlucde和lib输入
function(add_ezui_lib TARGET_NAME)

if(BUILD_EZUI)
    #直接添加依赖
    target_link_libraries(${TARGET_NAME} PRIVATE EzUI)
else()
    target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    # 拼接为 CONFIG_NAME
    set(CONFIG_NAME "${ARCH_NAME}_${LINK_TYPE}_${RUNTIME}")
    message("CONFIG_NAME: ${CONFIG_NAME}")
    target_link_libraries(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/build/${CONFIG_NAME}/${BUILD_TYPE}/EzUI.lib)
endif()

endfunction()

# 添加一个选项，供用户选择构建demo
option(BUILD_DEMO "Build demo program" ON)
if(BUILD_DEMO)
    add_subdirectory(./demo)#添加子项目
else()
endif()

