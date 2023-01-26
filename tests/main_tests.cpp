/**
 * @file       main_tests.cpp
 * @brief      Unit tests main file
 * @author     Jean-Roland Gosse
 *
 *             This file is part of LCSF C Stack.
 *
 *             LCSF C Stack is free software: you can redistribute it and/or
 *             modify it under the terms of the GNU General Public License as
 *             published by the Free Software Foundation, either version 3 of
 *             the License, or (at your option) any later version.
 *
 *             LCSF C Stack is distributed in the hope that it will be useful,
 *             but WITHOUT ANY WARRANTY; without even the implied warranty of
 *             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *             General Public License for more details.
 *
 *             You should have received a copy of the GNU General Public License
 *             along with this project. If not, see
 *             <https://www.gnu.org/licenses/>
 */

#include "CppUTest/CommandLineTestRunner.h"

/**
 * @brief      main function
 *
 * @param[in]  argc  The count of arguments
 * @param      argv  The arguments array
 *
 * @return     int: execution status
 */
int main(int argc, const char** argv) {
    return RUN_ALL_TESTS(argc, argv);
}
