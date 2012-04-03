cmake_minimum_required(VERSION 2.6)

#set(CMAKE_C_COMPILER "gcc-4.2")
#set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.2")

#set(CMAKE_C_COMPILER "/usr/bin/gcc")
#set(CMAKE_CXX_COMPILER "/usr/bin/g++")
#are links to:
#set(CMAKE_C_COMPILER /usr/bin/llvm-gcc-4.2)
#set(CMAKE_CXX_COMPILER /usr/bin/llvm-g++-4.2)

#set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "4.0")


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

if(DEFINED ENV{DO_OPEN_MP})
  message("DO_OPEN_MP is defined")
  IF(APPLE)
    SET(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION com.apple.compilers.llvmgcc42 CACHE STRING "" FORCE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -O0")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -O0")
  ENDIF()
else()
  message("DO_OPEN_MP is NOT defined")
endif()


project(MilpSolver)

#if(DEFINED ENV{DO_OPEN_MP})
#  message("DO_OPEN_MP is defined")
#  IF(APPLE)
#    SET(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION com.apple.compilers.llvmgcc42 CACHE STRING "" FORCE)
#    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -O0")
#    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -O0")
#  ENDIF()
#else()
#  message("DO_OPEN_MP is NOT defined")
#endif()


# Recurse into the "libMilpSolver" etc. subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.

add_subdirectory(libAnalytics)
add_subdirectory(libAnalyticsTest)

add_subdirectory(libStringUtilities)

add_subdirectory(libMilpSolver)
add_subdirectory(milpSolverTest)

