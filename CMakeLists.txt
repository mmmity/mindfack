cmake_minimum_required(VERSION 3.10)
project(mindfack VERSION 0.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 20)


set(EXTERNAL_INSTALL_LOCATION ${Testing_SOURCE_DIR}/3rdparty)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
find_package(Threads REQUIRED)  


SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")


if (APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20") 
endif()

SET(CPR_USE_SYSTEM_GTEST ON)


include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)

if (APPLE)

	set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl@1.1" CACHE INTERNAL "")
	set(OPENSSL_LIBRARIES "/usr/local/opt/openssl@1.1/lib" CACHE INTERNAL "")
    FetchContent_Declare(
		cpr_repo
		GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 11e5954
	)
else()
    FetchContent_Declare(
		cpr_repo
		GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 11e5954
	)
endif()

FetchContent_MakeAvailable(cpr_repo googletest)


add_executable(
    main 
    ${PROJECT_SOURCE_DIR}/src/main.cpp

    ${PROJECT_SOURCE_DIR}/src/automata/nfa.cpp
    ${PROJECT_SOURCE_DIR}/src/automata/dfa.cpp
    ${PROJECT_SOURCE_DIR}/src/automata/rfa.cpp
    ${PROJECT_SOURCE_DIR}/src/regular/reg_parser.cpp
)

target_include_directories(
    main
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

include(cmake/CodeCoverage.cmake)
add_custom_target(coverage_report)

function(run_coverage test_sources sources coverage_target target_name coverage_name)
    message("INFO test sources ${test_sources}")
    set(test_name "${target_name}-test")
    message("INFO sources ${sources}")
    add_executable(${test_name} ${test_sources} ${sources})

    # target_compile_options(${test_name} PRIVATE "-g" PRIVATE "-O0" PRIVATE "-fprofile-arcs" PRIVATE "-ftest-coverage" PRIVATE "--coverage")
    # target_link_options(${test_name} PRIVATE "${CMAKE_EXE_LINKER_FLAGS}" PRIVATE "-fprofile-arcs" PRIVATE "-ftest-coverage")
    target_include_directories(${test_name} PRIVATE ${PROJECT_SOURCE_DIR}/include)


    target_link_libraries(
		${test_name} gtest_main gtest gmock Threads::Threads cpr curl
	)

    setup_target_for_coverage(
            ${coverage_target} tests/${target_name} ${test_name} ${coverage_name}
    )

    add_dependencies(coverage_report ${coverage_target})
endfunction()



add_subdirectory(tests/unit)