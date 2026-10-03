cmake_minimum_required(VERSION 3.20)
project(SoICT2026 LANGUAGES CXX)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O2)
endif()

add_library(soict_core
    src/model/Instance.cpp
    src/model/Solution.cpp
    src/io/InstanceIO.cpp
    src/io/SolutionIO.cpp
    src/io/SyntheticGenerator.cpp
    src/eval/SolutionValidation.cpp)
target_compile_features(soict_core PUBLIC cxx_std_20)
target_include_directories(soict_core PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(soict_alns
    src/search/alns/RouteCache.cpp
    src/search/alns/SearchState.cpp
    src/search/alns/Balance.cpp
    src/search/alns/ALNS.cpp
    src/search/alns/operators/Destroy/RandomRemoval.cpp
    src/search/alns/operators/Destroy/WorstRemoval.cpp
    src/search/alns/operators/Destroy/RelatedRemoval.cpp
    src/search/alns/operators/Repair/InsertionSearch.cpp
    src/search/alns/operators/Repair/GreedyInsertion.cpp
    src/search/alns/operators/Repair/RegretInsertion.cpp
    src/search/alns/operators/LocalSearch/LocalSearch.cpp
    src/search/alns/operators/LocalSearch/AddRequest.cpp
    src/search/alns/operators/LocalSearch/RemoveRequest.cpp
    src/search/alns/operators/LocalSearch/PassengerRelocate.cpp
    src/search/alns/operators/LocalSearch/ParcelRelocate.cpp
    src/search/alns/operators/LocalSearch/CleanCut2OptStar.cpp
    src/search/alns/operators/LocalSearch/ReplaceRequest.cpp)
target_link_libraries(soict_alns PUBLIC soict_core)
target_compile_features(soict_alns PUBLIC cxx_std_20)

add_executable(SoICT2026 src/main.cpp)
target_link_libraries(SoICT2026 PRIVATE soict_core soict_alns)

if(DEFINED ENV{CPLEX_STUDIO_DIR2211})
    set(_cplex_default_root "$ENV{CPLEX_STUDIO_DIR2211}")
elseif(WIN32)
    set(_cplex_default_root "C:/Program Files/IBM/ILOG/CPLEX_Studio2211")
else()
    set(_cplex_default_root "/opt/ibm/ILOG/CPLEX_Studio2211")
endif()
set(CPLEX_ROOT "${_cplex_default_root}" CACHE PATH "IBM ILOG CPLEX Studio root")

set(_cplex_found TRUE)
if(NOT EXISTS "${CPLEX_ROOT}/cplex/include/ilcplex/ilocplex.h" OR
   NOT EXISTS "${CPLEX_ROOT}/concert/include/ilconcert/iloenv.h")
    set(_cplex_found FALSE)
endif()

if(WIN32 AND MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_cplex_port x64_windows_msvc14)
    set(_cplex_release_dir "${CPLEX_ROOT}/cplex/lib/${_cplex_port}/stat_mda")
    set(_concert_release_dir "${CPLEX_ROOT}/concert/lib/${_cplex_port}/stat_mda")
    set(_cplex_debug_dir "${CPLEX_ROOT}/cplex/lib/${_cplex_port}/stat_mdd")
    set(_concert_debug_dir "${CPLEX_ROOT}/concert/lib/${_cplex_port}/stat_mdd")
    set(_cplex_dll "${CPLEX_ROOT}/cplex/bin/${_cplex_port}/cplex2211.dll")
    set(_cplex_libraries
        "${_cplex_release_dir}/ilocplex.lib"
        "${_concert_release_dir}/concert.lib"
        "${_cplex_release_dir}/cplex2211.lib"
        "${_cplex_debug_dir}/ilocplex.lib"
        "${_concert_debug_dir}/concert.lib"
        "${_cplex_debug_dir}/cplex2211.lib"
        "${_cplex_dll}")
elseif(UNIX AND NOT APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)$")
    set(_cplex_port x86-64_linux/static_pic)
    set(_cplex_libraries
        "${CPLEX_ROOT}/cplex/lib/${_cplex_port}/libilocplex.a"
        "${CPLEX_ROOT}/concert/lib/${_cplex_port}/libconcert.a"
        "${CPLEX_ROOT}/cplex/lib/${_cplex_port}/libcplex.a")
else()
    set(_cplex_found FALSE)
endif()

foreach(_cplex_file IN LISTS _cplex_libraries)
    if(NOT EXISTS "${_cplex_file}")
        set(_cplex_found FALSE)
    endif()
endforeach()

if(_cplex_found)
    add_library(soict_mip src/search/mip/MIPSolver.cpp)
    target_compile_features(soict_mip PUBLIC cxx_std_20)
    target_compile_definitions(soict_mip PRIVATE IL_STD)
    target_include_directories(soict_mip PRIVATE
        "${CPLEX_ROOT}/cplex/include"
        "${CPLEX_ROOT}/concert/include")
    target_include_directories(soict_mip PUBLIC ${PROJECT_SOURCE_DIR}/include)
    target_link_libraries(soict_mip PUBLIC soict_core)
    if(WIN32)
        target_link_libraries(soict_mip PUBLIC
            optimized "${_cplex_release_dir}/ilocplex.lib"
            debug "${_cplex_debug_dir}/ilocplex.lib"
            optimized "${_concert_release_dir}/concert.lib"
            debug "${_concert_debug_dir}/concert.lib"
            optimized "${_cplex_release_dir}/cplex2211.lib"
            debug "${_cplex_debug_dir}/cplex2211.lib")
    else()
        find_package(Threads REQUIRED)
        target_link_libraries(soict_mip PUBLIC
            ${_cplex_libraries} Threads::Threads ${CMAKE_DL_LIBS} m)
    endif()
    target_link_libraries(SoICT2026 PRIVATE soict_mip)
    target_compile_definitions(SoICT2026 PRIVATE SOICT_WITH_CPLEX=1)
    if(WIN32)
        add_custom_command(TARGET SoICT2026 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_cplex_dll}" "$<TARGET_FILE_DIR:SoICT2026>")
    endif()
    message(STATUS "CPLEX MIP enabled from ${CPLEX_ROOT}")
else()
    message(STATUS "CPLEX headers or libraries not found for this platform; MIP target disabled")
endif()

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    add_executable(soict_core_test tests/CoreTest.cpp)
    target_link_libraries(soict_core_test PRIVATE soict_core)
    add_test(NAME soict_core_test COMMAND soict_core_test)
    add_executable(soict_alns_test tests/ALNSTest.cpp)
    target_link_libraries(soict_alns_test PRIVATE soict_alns)
    add_test(NAME soict_alns_test COMMAND soict_alns_test)
    add_executable(soict_clean_cut_test tests/CleanCut2OptStarTest.cpp)
    target_link_libraries(soict_clean_cut_test PRIVATE soict_alns)
    add_test(NAME soict_clean_cut_test COMMAND soict_clean_cut_test)
    add_executable(soict_replace_request_test tests/ReplaceRequestTest.cpp)
    target_link_libraries(soict_replace_request_test PRIVATE soict_alns)
    add_test(NAME soict_replace_request_test COMMAND soict_replace_request_test)
    if(Python3_Interpreter_FOUND)
        add_test(NAME soict_evaluator_test
            COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/tests/EvaluatorTest.py" "$<TARGET_FILE:SoICT2026>")
        add_test(NAME soict_probation_io_test
            COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/tests/ProbationIOTest.py" "$<TARGET_FILE:SoICT2026>")
    endif()
    if(TARGET soict_mip)
        add_executable(soict_mip_test tests/MIPTest.cpp)
        target_link_libraries(soict_mip_test PRIVATE soict_mip)
        if(WIN32)
            add_custom_command(TARGET soict_mip_test POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${_cplex_dll}" "$<TARGET_FILE_DIR:soict_mip_test>")
        endif()
        add_test(NAME soict_mip_test COMMAND soict_mip_test)
    endif()
endif()
