set(TARGET_NAME "unit")
set(COVERAGE_NAME "unit-coverage")
set(COVERAGE_TARGET "unit-coverage")
set(SOURCE_DIR_MAIN ${PROJECT_SOURCE_DIR}/src)
set(SOURCES ${PROJECT_SOURCE_DIR}/src/automata/nfa.cpp ${PROJECT_SOURCE_DIR}/src/automata/dfa.cpp ${PROJECT_SOURCE_DIR}/src/automata/rfa.cpp ${PROJECT_SOURCE_DIR}/src/regular/reg_parser.cpp )

SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")


message(INFO ${SOURCES})

set(TEST_SOURCES nfa_test.cpp dfa_test.cpp regular_parser_test.cpp rfa_test.cpp)
run_coverage(
    "${TEST_SOURCES}"
    "${SOURCES}"
    "${COVERAGE_TARGET}"
    "${TARGET_NAME}"
    "${COVERAGE_NAME}"
)
