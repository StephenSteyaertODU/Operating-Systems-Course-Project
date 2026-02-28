file(GLOB_RECURSE LIB_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(cpu_sched_library)

target_sources(cpu_sched_library PRIVATE ${LIB_SOURCES})

target_include_directories(cpu_sched_library PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(cpu_sched_library PUBLIC cxx_std_20)

target_precompile_headers(cpu_sched_library PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/common/common.hpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(cpu_sched_library PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_library(cpu_sched::library ALIAS cpu_sched_library)