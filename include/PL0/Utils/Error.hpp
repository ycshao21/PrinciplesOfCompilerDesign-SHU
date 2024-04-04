#pragma once
#include <format>
#include <iostream>
#include <string>

#define _COLOR_RESET "\033[0m"
#define _COLOR_RED "\033[1;31m"
#define _FMT "ERROR >>> [Line {}] {}: {}\n"

#define SHOW_ERROR(line, type, value) std::cout << std::format(_COLOR_RED _FMT _COLOR_RESET, line, type, value);