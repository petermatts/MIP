add_executable(mip_benchmarks
    benchmark_image.cpp
)

target_link_libraries(mip_benchmarks
    PRIVATE mip::mip
)

target_compile_features(mip_benchmarks PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(mip_benchmarks PRIVATE /W4)
else()
    target_compile_options(mip_benchmarks PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
    )
endif()