cmake_minimum_required(VERSION 3.20)

project(PA2 C)

set(CMAKE_C_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(thread_launcher thread_launcher.c)
target_compile_options(thread_launcher PRIVATE -Wall -Wextra -pedantic)
target_link_libraries(thread_launcher PRIVATE Threads::Threads)