cmake_minimum_required(VERSION 3.16)
project(mc_pricer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_library(pricer src/bs.cpp src/mc.cpp)
target_include_directories(pricer PUBLIC include)
target_compile_options(pricer PRIVATE -Wall -Wextra)

add_executable(mc_pricer src/main.cpp)
target_link_libraries(mc_pricer PRIVATE pricer)

enable_testing()
add_executable(tests test/test.cpp)
target_link_libraries(tests PRIVATE pricer)
add_test(NAME pricer_tests COMMAND tests)