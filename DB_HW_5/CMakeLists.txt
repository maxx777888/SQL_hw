cmake_minimum_required(VERSION 3.2)

project(DB_HW_5)

set(PostgreSQL_ADDITIONAL_VERSIONS "15")

add_subdirectory(../libpqxx-7.7.5 libpqxx-build)

add_executable(DB_HW_5 main.cpp)

target_compile_features(DB_HW_5 PRIVATE cxx_std_17)

target_link_libraries(DB_HW_5 pqxx)
