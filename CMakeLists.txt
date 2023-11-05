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

# CMake version
cmake_minimum_required(VERSION 3.14)

# Project
project(lcsf_stack VERSION 0.1 LANGUAGES C)

# Includes
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Build system debug option (optional)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Global output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compilation flags
set(CMAKE_C_FLAGS "-Wpedantic -Werror -Wall -Wextra -Wshadow -Wundef")

# Debug symbol if building as debug (-DCMAKE_BUILD_TYPE="Debug")
add_compile_options("$<$<CONFIG:DEBUG>:-D_APP_DEBUG>")

# Target names
set(EXAMPLE_APP_BIN_NAME "lcsf_stack_example")
set(CORE_LIB_NAME "lcsf_stack_lib")
set(PROT_EXAMPLE_LIB_NAME "lcsf_prop_example_lib")
set(PROT_TEST_A_LIB_NAME "lcsf_prot_test_a_lib")
set(PROT_TEST_B_LIB_NAME "lcsf_prot_test_b_lib")

# Test targets names, bin/lib are split because of link conflicts between mock and real functions
set(TESTS_A_BRIDGE_BIN_NAME "lcsf_stack_tests_a_bridge_bin")
set(TESTS_A_MAIN_BIN_NAME "lcsf_stack_tests_a_main_bin")
set(TESTS_A_FULLSTACK_BIN_NAME "lcsf_stack_tests_a_fullstack_bin")
set(TESTS_B_BRIDGE_BIN_NAME "lcsf_stack_tests_b_bridge_bin")
set(TESTS_B_MAIN_BIN_NAME "lcsf_stack_tests_b_main_bin")
set(TESTS_B_FULLSTACK_BIN_NAME "lcsf_stack_tests_b_fullstack_bin")

# CppUtest paths (configure as needed)
set(CPPUTEST_INC_PATH "/usr/include/CppUtest/")
set(CPPUTEST_BIN_PATH "/usr/bin/x86_64-linux-gnu")

# Doxygen vars
set(DOXYGEN_CFG_FILE "${PROJECT_SOURCE_DIR}/doc/cfg/doxcfg.cfg")
set(DOXYGEN_INDEX "${PROJECT_SOURCE_DIR}/doc/html/index.html")
set(DOXYGEN_SHORTCUT "${PROJECT_SOURCE_DIR}/doc/linux_index.html")

# Project subdirs
add_subdirectory(src)
add_subdirectory(tests)

# Doxygen target
add_custom_target(doc
    COMMAND doxygen ${DOXYGEN_CFG_FILE}
    COMMAND ln -sf ${DOXYGEN_INDEX} ${DOXYGEN_SHORTCUT}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Build doxygen doc")

# Clang-format target (requires v14)
add_custom_target(format_style
    COMMAND find . -iname "*.h" -o -iname "*.c" | xargs clang-format-14 -i --style=file
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Format code style")

add_custom_target(check_style
    COMMAND find . -iname "*.h" -o -iname "*.c" | xargs clang-format-14 -n --Werror --style=file
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Check code style")