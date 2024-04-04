# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# Exclude main.cpp from the sources
list(FILTER SOURCES EXCLUDE REGEX ".*main.cpp$")

# # Add executable with the project name
# add_executable(PL0 ${SOURCES} "./main.cpp")

# set_target_properties(
#     PL0_Compiler PROPERTIES 
#     RUNTIME_OUTPUT_DIRECTORY
#     ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}
# )