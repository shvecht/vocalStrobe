include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(strobe STATIC ../src/strobe.cpp)

target_include_directories(strobe PUBLIC ../src)

add_executable(test_strobe test_strobe.cpp)
target_link_libraries(test_strobe strobe gtest_main)

add_test(NAME strobe_test COMMAND test_strobe)
