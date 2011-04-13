cmake_minimum_required(VERSION 2.6)

project(libMilpSolver)

file(GLOB_RECURSE HEADERS "*.h")

set(SOURCES 
  src/BclException.cpp
  src/ScheduleSvg.cpp
  src/Solver.cpp
  src/Svg.cpp
  src/StringUtilities.cpp
  src/GurobiSolver.cpp
  src/XpressSolver.cpp
  src/CplexSolver.cpp
)

include_directories(
  c:/xpressmp/include
  $ENV{GUROBI_HOME}/include
  include
  $ENV{CPLEX_HOME}/include
  $ENV{CONCERT_HOME}/include
)

add_definitions( 
  -D_DEBUG
)

#add_library(../libMilpSolver/Debug/MilpSolver.lib SHARED ${SOURCES} ${HEADERS})
add_library(MilpSolver STATIC ${SOURCES} ${HEADERS})

