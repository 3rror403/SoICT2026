cmake_minimum_required(VERSION 3.20)
project(SoICT2026 LANGUAGES CXX)

add_library(soict_core
    src/model/Instance.cpp
    src/model/Solution.cpp
    src/io/InstanceIO.cpp
    src/io/SyntheticGenerator.cpp)
target_compile_features(soict_core PUBLIC cxx_std_20)
target_include_directories(soict_core PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(SoICT2026 src/main.cpp)
target_link_libraries(SoICT2026 PRIVATE soict_core)

include(CTest)
if(BUILD_TESTING)
    add_executable(soict_core_test tests/CoreTest.cpp)
    target_link_libraries(soict_core_test PRIVATE soict_core)
    add_test(NAME soict_core_test COMMAND soict_core_test)
endif()
