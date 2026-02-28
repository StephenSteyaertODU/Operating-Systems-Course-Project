include(CTest)

file(GLOB_RECURSE TEST_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(producer_consumer_tests)

target_sources(
    producer_consumer_tests
    PRIVATE
    ${TEST_SOURCES}
)

target_link_libraries(producer_consumer_tests
    PRIVATE producer_consumer::library Catch2::Catch2WithMain
)

target_compile_features(producer_consumer_tests PRIVATE cxx_std_20)

include(Catch)

catch_discover_tests(producer_consumer_tests)