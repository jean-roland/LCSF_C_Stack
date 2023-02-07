# This file is part of LCSF C Stack.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>
#
# Author: Jean-Roland Gosse

# Add CXX for CppUtest
enable_language(CXX)

# Retrieve CppUtest libs
find_library(CPPUTEST_LIB NAMES libCppUTest.a PATHS ${CPPUTEST_BIN_PATH})
find_library(CPPUTEST_EXT_LIB NAMES libCppUTestExt.a PATHS ${CPPUTEST_BIN_PATH})

# Abort if libs not found
if(NOT CPPUTEST_LIB OR NOT CPPUTEST_EXT_LIB)
  message("libCppUTest.a or libCppUTestExt.a not found, aborting build!")
  return()
endif()

# Debug message
# message("Found libCppUTest.a at ${CPPUTEST_LIB} and libCppUTestExt.a at ${CPPUTEST_EXT_LIB}")

# Tests lib + A targets
add_executable(${TESTS_A_BRIDGE_BIN_NAME} main_tests.cpp test_Filo.cpp test_LCSF_Transcoder.cpp test_A_LCSF_Bridge_Test.cpp)
target_include_directories(${TESTS_A_BRIDGE_BIN_NAME} PUBLIC "../include/" "../include/lib/" "../include/protocols/" "../src/protocols_a/" "${CPPUTEST_INC_PATH}")
target_link_libraries(${TESTS_A_BRIDGE_BIN_NAME} PRIVATE ${CORE_LIB_NAME} ${PROT_TEST_A_LIB_NAME} ${CPPUTEST_LIB} ${CPPUTEST_EXT_LIB})

add_executable(${TESTS_A_MAIN_BIN_NAME} main_tests.cpp test_LCSF_Validator.cpp test_A_Test_Main.cpp)
target_include_directories(${TESTS_A_BRIDGE_BIN_NAME} PUBLIC "../include/" "../include/lib/" "../include/protocols/" "../src/protocols_a/" "${CPPUTEST_INC_PATH}")
target_link_libraries(${TESTS_A_MAIN_BIN_NAME} PRIVATE ${CORE_LIB_NAME} ${PROT_EXAMPLE_LIB_NAME} ${PROT_TEST_A_LIB_NAME} ${CPPUTEST_LIB} ${CPPUTEST_EXT_LIB})

add_executable(${TESTS_A_FULLSTACK_BIN_NAME} main_tests.cpp test_A_Test_FullStack.cpp)
target_include_directories(${TESTS_A_BRIDGE_BIN_NAME} PUBLIC "../include/" "../include/lib/" "../src/protocols_a/" "${CPPUTEST_INC_PATH}")
target_link_libraries(${TESTS_A_FULLSTACK_BIN_NAME} PRIVATE ${CORE_LIB_NAME} ${PROT_TEST_A_LIB_NAME} ${CPPUTEST_LIB} ${CPPUTEST_EXT_LIB})

# Tests B targets
add_executable(${TESTS_B_BRIDGE_BIN_NAME} main_tests.cpp test_B_LCSF_Bridge_Test.cpp)
target_include_directories(${TESTS_B_BRIDGE_BIN_NAME} PUBLIC "../include/" "../include/lib/" "../src/protocols_b/" "${CPPUTEST_INC_PATH}")
target_link_libraries(${TESTS_B_BRIDGE_BIN_NAME} PRIVATE ${CORE_LIB_NAME} ${PROT_TEST_B_LIB_NAME} ${CPPUTEST_LIB} ${CPPUTEST_EXT_LIB})

add_executable(${TESTS_B_MAIN_BIN_NAME} main_tests.cpp test_B_Test_Main.cpp)
target_include_directories(${TESTS_B_MAIN_BIN_NAME} PUBLIC "../include/" "../include/lib/" "../src/protocols_b/" "${CPPUTEST_INC_PATH}")
target_link_libraries(${TESTS_B_MAIN_BIN_NAME} PRIVATE ${CORE_LIB_NAME} ${PROT_TEST_B_LIB_NAME} ${CPPUTEST_LIB} ${CPPUTEST_EXT_LIB})

add_executable(${TESTS_B_FULLSTACK_BIN_NAME} main_tests.cpp test_B_Test_FullStack.cpp)
target_include_directories(${TESTS_B_FULLSTACK_BIN_NAME} PUBLIC "../include/" "../include/lib/" "../src/protocols_b/" "${CPPUTEST_INC_PATH}")
target_link_libraries(${TESTS_B_FULLSTACK_BIN_NAME} PRIVATE ${CORE_LIB_NAME} ${PROT_TEST_B_LIB_NAME} ${CPPUTEST_LIB} ${CPPUTEST_EXT_LIB})

# Custom target to run tests
add_custom_target(run_tests
  COMMAND ./${TESTS_A_BRIDGE_BIN_NAME} -c -v
  COMMAND ./${TESTS_A_MAIN_BIN_NAME} -c -v
  COMMAND ./${TESTS_A_FULLSTACK_BIN_NAME} -c -v
  COMMAND ./${TESTS_B_BRIDGE_BIN_NAME} -c -v
  COMMAND ./${TESTS_B_MAIN_BIN_NAME} -c -v
  COMMAND ./${TESTS_B_FULLSTACK_BIN_NAME} -c -v
  COMMENT "Run tests"
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
# Target depends on test binary
add_dependencies(run_tests ${TESTS_A_BRIDGE_BIN_NAME} ${TESTS_A_MAIN_BIN_NAME} ${TESTS_A_FULLSTACK_BIN_NAME}
 ${TESTS_B_BRIDGE_BIN_NAME} ${TESTS_B_MAIN_BIN_NAME} ${TESTS_B_FULLSTACK_BIN_NAME})

