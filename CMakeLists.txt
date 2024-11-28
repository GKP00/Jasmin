cmake_minimum_required(VERSION 3.16)

project(Jasmin)
set(CMAKE_CXX_STANDARD 17)

add_compile_options("-pedantic")
add_compile_options("-Wall")


include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt 
  GIT_TAG f5e54359df4c26b6230fc61d38aa294581393084 #v10.1.1
  )

FetchContent_MakeAvailable(fmt)

add_library(Jasmin "src/Lexer.cpp")

target_include_directories(Jasmin PUBLIC "include")
target_link_libraries(Jasmin PRIVATE fmt)

add_subdirectory("deps/ClassFile/")
target_link_libraries(Jasmin PUBLIC ClassFile)

option(BUILD_TESTS "build tests" OFF)
if(BUILD_TESTS)
  add_subdirectory(test)
endif()
