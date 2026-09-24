cmake_minimum_required(VERSION 3.20)
project(SoICT2026 LANGUAGES CXX)

add_executable(SoICT2026 src/main.cpp)

target_compile_features(SoICT2026 PRIVATE cxx_std_20)
target_include_directories(SoICT2026 PRIVATE ${PROJECT_SOURCE_DIR}/include)
