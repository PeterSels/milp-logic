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
  $ENV{CPLEX_HOME}/include
  $ENV{CONCERT_HOME}/include
)

link_directories(
  /Library/gurobi301/mac64/lib
  c:/xpressmp/lib
  ${MilpSolver_SOURCE_DIR}/libMilpSolver
  $ENV{CPLEX_HOME}/lib/x86_.net2005_8.0/stat_mta
  $ENV{CONCERT_HOME}/lib/x86_.net2005_8.0/stat_mta
  $ENV{CPLEX_HOME}/lib/x86_.net2005_8.0/stat_mta
)

add_definitions( 
  -D_CONSOLE
  -D_DEBUG
)

add_executable( milpSolverTest ${SOURCES} ${HEADERS})



#target_link_libraries( milpSolverTest MilpSolver c:/xpressmp/lib/xprs c:/xpressmp/lib/xprb gurobi_c++ gurobi30)

#IF(WIN32)
target_link_libraries(milpSolverTest MilpSolver 
                      c:/xpressmp/lib/xprs.lib 
                      c:/xpressmp/lib/xprb.lib 
                      $ENV{CPLEX_HOME}/lib/x86_.net2005_8.0/stat_mta/ilocplex.lib
                      $ENV{CONCERT_HOME}/lib/x86_.net2005_8.0/stat_mta/concert.lib
                      $ENV{CPLEX_HOME}/lib/x86_.net2005_8.0/stat_mta/$ENV{CPLEX_LIB}.lib                     
)

#target_link_libraries( milpSolverTest MilpSolver xplrs xprb)
#ELSE(WIN32)
#  target_link_libraries( milpSolverTest MilpSolver gurobi_c++ gurobi30 )
#ENDIF(WIN32)
