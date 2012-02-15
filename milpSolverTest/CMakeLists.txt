cmake_minimum_required(VERSION 2.6)

project( milpSolverTest )

if(DEFINED ENV{DO_OPEN_MP})
  SET(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION com.apple.compilers.llvmgcc42 CACHE STRING "" FORCE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
endif()

file(GLOB_RECURSE HEADERS "*.h")

set(SOURCES 
  src/milpSolverTest.cpp
)

include_directories( 
  ../libAnalytics/include
  ../libStringUtilities/include
  ../libMilpSolver/include
  c:/xpressmp/include
  $ENV{GUROBI_HOME}/include
  include
  $ENV{CPLEX_HOME}/include
  $ENV{CONCERT_HOME}/include
  $ENV{XPRESS_HOME}/include
)

IF(APPLE)
 set(CPLEX_ARCH_DIR x86-64_darwin9_gcc4.0)
 set(THREAD )
ELSEIF(WIN32)
 set(CPLEX_ARCH_DIR DUNNO)
 set(THREAD )
ELSE()
 set(CPLEX_ARCH_DIR x86-64_sles10_4.1)
 set(THREAD pthread)
ENDIF()

link_directories(
  $ENV{GUROBI_HOME}/lib
  ../libStringUtilities
  ../libMilpSolver
  $ENV{CPLEX_HOME}/lib/${CPLEX_ARCH_DIR}/static_pic
  $ENV{CONCERT_HOME}/lib/${CPLEX_ARCH_DIR}/static_pic
  $ENV{XPRESS_HOME}/lib
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

if(DEFINED ENV{DO_OPEN_MP})
  SET_TARGET_PROPERTIES( milpSolverTest PROPERTIES COMPILE_FLAGS "-fopenmp" LINK_FLAGS "-fopenmp")
endif()

target_link_libraries( milpSolverTest MilpSolver Analytics StringUtilities gurobi_c++ $ENV{GUROBI_MAIN_VERSION} ilocplex cplex concert 
#xprs xprb 
m ${EXTRA_LIBS} ${THREAD})

