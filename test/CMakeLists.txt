cmake_minimum_required(VERSION 3.16)
project(JasminTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG b514bdc898e2951020cbdca1304b75f5950d1f59 #v1.15.2
  )
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(JasminTests Tests.cpp)
target_link_libraries(JasminTests Jasmin GTest::gtest_main)

file(CREATE_LINK "${PROJECT_SOURCE_DIR}/res" "${PROJECT_BINARY_DIR}/res" SYMBOLIC)
target_compile_definitions(JasminTests PRIVATE RES_DIR="${PROJECT_BINARY_DIR}/res")

include(GoogleTest)
gtest_discover_tests(JasminTests)

