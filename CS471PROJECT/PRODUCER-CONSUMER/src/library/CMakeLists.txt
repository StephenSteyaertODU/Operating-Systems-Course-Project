file(GLOB_RECURSE LIB_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(producer_consumer_library)

target_sources(producer_consumer_library PRIVATE ${LIB_SOURCES})

target_include_directories(producer_consumer_library PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(producer_consumer_library PUBLIC cxx_std_20)

target_precompile_headers(producer_consumer_library PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/common/common.hpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(producer_consumer_library PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_library(producer_consumer::library ALIAS producer_consumer_library)