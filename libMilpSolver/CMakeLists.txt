cmake_minimum_required(VERSION 2.6)

project(libMilpSolver)

if(DEFINED ENV{DO_OPEN_MP})
  SET(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION com.apple.compilers.llvmgcc42 CACHE STRING "" FORCE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
endif()

CMAKE_POLICY(SET CMP0009 OLD)
file(GLOB_RECURSE HEADERS "*.h" "./include/*.h")

set(SOURCES 
  src/BclException.cpp
  src/BreakPointCalculator.cpp
  src/CplexSolver.cpp
  src/GurobiSolver.cpp
  src/MinimumCalculator.cpp
  src/PwlApproximator.cpp
  src/ScheduleSvg.cpp
  src/Solver.cpp
  src/Svg.cpp
  src/XpressSolver.cpp
)

include_directories(
  include
  $ENV{MILP_SOLVER_HOME}/libAnalytics/include
  $ENV{MILP_SOLVER_HOME}/libStringUtilities/include
  $ENV{CPLEX_HOME}/include
  $ENV{CONCERT_HOME}/include
  $ENV{GUROBI_HOME}/include
  $ENV{XPRESS_HOME}/include
)

add_definitions( 
#  -D_DEBUG
)

add_library(MilpSolver STATIC ${SOURCES} ${HEADERS})

if(DEFINED $ENV{DO_OPEN_MP})
  SET_TARGET_PROPERTIES( MilpSolver PROPERTIES COMPILE_FLAGS "-fopenmp" )
endif()

