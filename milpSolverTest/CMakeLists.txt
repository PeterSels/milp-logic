cmake_minimum_required(VERSION 2.6)

project( milpSolverTest )

file(GLOB_RECURSE HEADERS "*.h")

set(SOURCES 
  src/milpSolverTest.cpp
)

include_directories( 
  ../libMilpSolver/include
  c:/xpressmp/include
  /Library/gurobi301/mac64/include
  include
)

link_directories(
  /Library/gurobi301/mac64/lib
  ${MilpSolver_SOURCE_DIR}/libMilpSolver
)

add_definitions( 
  -D_CONSOLE
  -D_DEBUG
)

add_executable( milpSolverTest ${SOURCES} ${HEADERS})

#target_link_libraries( milpSolverTest MilpSolver c:/xpressmp/lib/xprs c:/xpressmp/lib/xprb gurobi_c++ gurobi30)
target_link_libraries( milpSolverTest MilpSolver gurobi_c++ gurobi30)

