cmake_minimum_required(VERSION 2.6)

project(libMilpLogic)

CMAKE_POLICY(SET CMP0009 OLD)

INCLUDE(${CMAKE_SOURCE_DIR}/boost.cmake)

file(GLOB HEADERS "*.h" "./include/*.h")

set(SOURCES 
  src/BclException.cpp
  src/CplexSolver.cpp
  src/GurobiSolver.cpp
  src/Solver.cpp
  src/StringUtilities.cpp
  src/XpressSolver.cpp
)

include_directories(
  include
  $ENV{CPLEX_HOME}/include
  $ENV{CONCERT_HOME}/include
  $ENV{GUROBI_HOME}/include
  $ENV{XPRESS_HOME}/include
  ${BOOST_INCLUDEDIR}
)

add_library(MilpLogic STATIC ${SOURCES} ${HEADERS})

#if(DEFINED $ENV{DO_OPEN_MP})
#  SET_TARGET_PROPERTIES( MilpSolver PROPERTIES COMPILE_FLAGS "-fopenmp" )
#endif()

