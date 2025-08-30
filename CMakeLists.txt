cmake_minimum_required(VERSION 3.0...9.0)

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

#添加子项目
add_subdirectory(./demo)
