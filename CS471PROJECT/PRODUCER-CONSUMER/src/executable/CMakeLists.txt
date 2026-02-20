add_executable(producer_consumer)

target_sources(
    producer_consumer
    PRIVATE
    main.cpp
)

target_link_libraries(
    smplscript
    PRIVATE 
    producer_consumer::library
)

target_compile_features(producer_consumer PRIVATE cxx_std_20)