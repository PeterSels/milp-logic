cmake_minimum_required(VERSION 2.6)

project( milpSolverTest )

file(GLOB_RECURSE HEADERS "*.h")

set(SOURCES 
  src/milpSolverTest.cpp
)

include_directories( 
  ../libMilpSolver/include
  c:/xpressmp/include
  /Library/$ENV{GUROBI_VERSION}/mac64/include
  include
  $ENV{CPLEX_HOME}/include
  $ENV{CONCERT_HOME}/include
)

link_directories(
  /Library/$ENV{GUROBI_VERSION}/mac64/lib
  ${MilpSolver_SOURCE_DIR}/libMilpSolver
  $ENV{CPLEX_HOME}/lib/x86-64_darwin9_gcc4.0/static_pic
  $ENV{CONCERT_HOME}/lib/x86-64_darwin9_gcc4.0/static_pic
)

add_definitions( 
  -D_CONSOLE
  -D_DEBUG
)

IF(APPLE)
   FIND_LIBRARY(COREFOUNDATION CoreFoundation)
   FIND_LIBRARY(IOKIT IOKit)
   LIST(APPEND EXTRA_LIBS ${COREFOUNDATION} ${IOKIT})
ENDIF(APPLE)

add_executable( milpSolverTest ${SOURCES} ${HEADERS})

target_link_libraries( milpSolverTest MilpSolver gurobi_c++ $ENV{GUROBI_MAIN_VERSION} ilocplex cplex concert m ${EXTRA_LIBS})

