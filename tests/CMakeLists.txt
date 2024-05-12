# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/src/*.cpp)
# Exclude main.cpp from the sources
list(FILTER SOURCES EXCLUDE REGEX ".*main.cpp$")

# Set the output path for the experiments
string(
    CONCAT EXPERIMENT_OUTPUT_PATH
    ${PROJECT_SOURCE_DIR}/bin/
    ${CMAKE_BUILD_TYPE}/
    ${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}/
)
message(STATUS "Experiments output path: ${EXPERIMENT_OUTPUT_PATH}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${EXPERIMENT_OUTPUT_PATH})

# Add the experiments executables
add_executable(exp01 ${SOURCES} "./experiments/exp01-recognize-identifier_main.cpp")
add_executable(exp02 ${SOURCES} "./experiments/exp02-analyze-lexical_main.cpp")
add_executable(exp03 ${SOURCES} "./experiments/exp03-analyze-syntax_main.cpp")
add_executable(exp04 ${SOURCES} "./experiments/exp04-analyze-semantics_main.cpp")
add_executable(exp06 ${SOURCES} "./experiments/exp06-optimize-code_main.cpp")