cmake_minimum_required(VERSION 2.6)

IF(WIN32)
  SET(CMAKE_CXX_FLAGS_RELEASE "/MT" CACHE STRING "runtime-library flags" FORCE)
  SET(CMAKE_CXX_FLAGS_DEBUG "/MTd"  CACHE STRING "runtime-library flags" FORCE)
  SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT"  CACHE STRING "runtime-library flags" FORCE)
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MTd"  CACHE STRING  "runtime-library flags" FORCE)
ENDIF()

# The configurations with /MT io /MTd still give linker errors, a la error LNK2005: _heap_alloc already defined in LIBCMT.lib
# The name of our project is "MilpSolver".  CMakeLists files in this project can
# refer to the root source directory of the project as ${MilpSolver_SOURCE_DIR} and
# to the root binary directory of the project as ${MilpSolver_BINARY_DIR}.

project(MilpLogic)

INCLUDE(${CMAKE_SOURCE_DIR}/boost.cmake)


# Recurse into the "libMilpSolver" etc. subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.

add_subdirectory(libMilpLogic)
add_subdirectory(milpLogicTest)
add_subdirectory(milpLogicSortingNetworkOptimizer)

