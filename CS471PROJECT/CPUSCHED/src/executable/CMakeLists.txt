add_executable(cpu_sched)

target_sources(
    cpu_sched
    PRIVATE
    main.cpp
)

target_link_libraries(
    smplscript
    PRIVATE 
    cpu_sched::library
)

target_compile_features(cpu_sched PRIVATE cxx_std_20)