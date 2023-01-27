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

# Test protocol library target
add_library(${PROT_TEST_B_LIB_NAME} STATIC LCSF_Bridge_Test.c LCSF_Desc_Test.c Test_Main.c)
# Dependencies
target_include_directories(${PROT_TEST_B_LIB_NAME} PUBLIC "../../include/" "../../include/lib/" "./")
target_link_libraries(${PROT_TEST_B_LIB_NAME} PRIVATE ${CORE_LIB_NAME})