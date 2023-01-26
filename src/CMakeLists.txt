# This file is part of LCSF C Stack.
#
# LCSF C Stack is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LCSF C Stack is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this project. If not, see <https://www.gnu.org/licenses/>
#
# Author: Jean-Roland Gosse

add_subdirectory(lib)
add_subdirectory(protocols_a)
add_subdirectory(protocols_b)

# App binary target
add_executable(${EXAMPLE_APP_BIN_NAME} custom_main.c)
# Dependencies
target_include_directories(${EXAMPLE_APP_BIN_NAME} PUBLIC "../include/")
target_link_libraries(${EXAMPLE_APP_BIN_NAME} PRIVATE ${CORE_LIB_NAME} ${PROT_TEST_A_LIB_NAME} ${PROT_EXAMPLE_LIB_NAME})
