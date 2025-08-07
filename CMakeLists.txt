cmake_minimum_required(VERSION 3.28)
project(dbview LANGUAGES C)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE src/main.c)
target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)
