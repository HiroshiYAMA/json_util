cmake_minimum_required(VERSION 3.0.0)
project(json_util)

set(CMAKE_INSTALL_PREFIX "../install")
set(exe_install_path ".")
set(lib_install_path ".")

set(exe_target "${PROJECT_NAME}")

set(SRC_DIR "src")

# for exe.
add_executable(${exe_target}
    ${SRC_DIR}/json_util.cpp
)

set_target_properties(${exe_target} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
if(APPLE)
    set_target_properties(${exe_target} PROPERTIES
        BUILD_RPATH "@executable_path"
        INSTALL_RPATH "@executable_path"
    )
else(APPLE)
    set_target_properties(${exe_target} PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )
endif(APPLE)

### Linux specific configuration ###
if(UNIX AND NOT APPLE)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
            # Must use std::experimental namespace if older than GCC8
            message("[${PROJECT_NAME}] GCC version less than 8. Using std::experimental namespace.")
            target_compile_definitions(${exe_target} PRIVATE USE_EXPERIMENTAL_FS)
        endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
            # Must explicitly link separate std::filesystem if older than GCC9
            message("[${PROJECT_NAME}] GCC version less than 9. Explicitly linking separate std::filesystem library.")
            target_link_libraries(${exe_target} stdc++fs)
        endif()
    endif()
endif(UNIX AND NOT APPLE)

## Install path defined in parent CMakeLists
install(TARGETS ${exe_target} DESTINATION ${exe_install_path})
