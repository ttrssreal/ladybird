set(CMAKE_EXE_LINKER_FLAGS "${ORIGINAL_CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
set(CMAKE_SHARED_LINKER_FLAGS "${ORIGINAL_CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
set(CMAKE_MODULE_LINKER_FLAGS "${ORIGINAL_CMAKE_MODULE_LINKER_FLAGS} -fsanitize=address")

add_executable(FuzzilliJs
        ReadEvalPrintResetLoop.cpp
        Coverage.cpp
        FuzzilliGlobal.cpp)

target_compile_options(FuzzilliJs
        PRIVATE $<$<CXX_COMPILER_ID:Clang>:-g -O1 -fsanitize-coverage=trace-pc-guard>
)

target_link_libraries(FuzzilliJs
        PUBLIC AK LibCore LibJS LibGC
        PRIVATE $<$<CXX_COMPILER_ID:Clang>:-fsanitize-coverage=trace-pc-guard>
)
