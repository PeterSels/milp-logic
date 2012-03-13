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
  include
  $ENV{MILP_SOLVER_HOME}/libAnalytics/include
  $ENV{MILP_SOLVER_HOME}/libStringUtilities/include
  $ENV{MILP_SOLVER_HOME}/libMilpSolver/include
  $ENV{CPLEX_HOME}/include
  $ENV{CONCERT_HOME}/include
  $ENV{GUROBI_HOME}/include
  $ENV{XPRESS_HOME}/include
)


IF(APPLE)
 set(CPLEX_ARCH_DIR x86-64_darwin9_gcc4.0/static_pic)
 set(GUROBI_CPP gurobi_c++)
 set(MATH_LIB m)
 set(THREAD )
ELSEIF(WIN32)
 set(CPLEX_ARCH_DIR x86_windows_vs2008/stat_mta)
 set(GUROBI_CPP gurobi_c++mtd2008)
 set(MATH_LIB )
 set(THREAD )
ELSE()
 set(CPLEX_ARCH_DIR x86-64_sles10_4.1/static_pic)
 set(GUROBI_CPP gurobi_c++)
 set(MATH_LIB m)
 set(THREAD pthread)
ENDIF()

link_directories(
  $ENV{MILP_SOLVER_HOME}/libStringUtilities
  $ENV{MILP_SOLVER_HOME}/libMilpSolver
  $ENV{CPLEX_HOME}/lib/${CPLEX_ARCH_DIR}
  $ENV{CONCERT_HOME}/lib/${CPLEX_ARCH_DIR}
  $ENV{GUROBI_HOME}/lib
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

target_link_libraries( milpSolverTest MilpSolver Analytics StringUtilities 
${GUROBI_CPP} $ENV{GUROBI_MAIN_VERSION} 
ilocplex cplex${CPLEX_VERSION} concert 
#xprs xprb 
${MATH_LIB} ${EXTRA_LIBS} ${THREAD})

