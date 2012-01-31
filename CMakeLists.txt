cmake_minimum_required(VERSION 2.6)

#set(CMAKE_C_COMPILER "gcc-4.2")
#set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.2")


# The name of our project is "MilpSolver".  CMakeLists files in this project can
# refer to the root source directory of the project as ${MilpSolver_SOURCE_DIR} and
# to the root binary directory of the project as ${MilpSolver_BINARY_DIR}.
project(MilpSolver)

# Recurse into the "libMilpSolver" etc. subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.

add_subdirectory(libAnalytics)
add_subdirectory(libAnalyticsTest)

add_subdirectory(libStringUtilities)

add_subdirectory(libMilpSolver)
add_subdirectory(milpSolverTest)

