include(CTest)

file(GLOB_RECURSE TEST_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(cpu_sched_tests)

target_sources(
    cpu_sched_tests
    PRIVATE
    ${TEST_SOURCES}
)

target_link_libraries(cpu_sched_tests
    PRIVATE cpu_sched::library Catch2::Catch2WithMain
)

target_compile_features(cpu_sched_tests PRIVATE cxx_std_20)

include(Catch)

catch_discover_tests(cpu_sched_tests)