cmake_minimum_required(VERSION 2.6)

project( milpLogicSortingNetworkOptimizer )

INCLUDE(${CMAKE_SOURCE_DIR}/boost.cmake)

IF(WIN32)
  SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /FORCE:MULTIPLE")
  SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug  /FORCE:MULTIPLE")
  SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /FORCE:MULTIPLE")
ENDIF()

file(GLOB_RECURSE HEADERS "*.h")

set(SOURCES
 src/milpLogicSortingNetworkOptimizer.cpp
)

include_directories(
 include
 $ENV{MILP_LOGIC_HOME}/libMilpLogic/include
# $ENV{COIN_OR_HOME}/CoinAll-1.3.0/include/coin
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
 set(CPLEX_ARCH_DIR x64_windows_vs2008/stat_mta)
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
 $ENV{MILP_LOGIC_HOME}/libMilpLogic
# $ENV{COIN_OR_HOME}/CoinAll-1.3.0/lib
 $ENV{CPLEX_HOME}/lib/${CPLEX_ARCH_DIR}
 $ENV{CONCERT_HOME}/lib/${CPLEX_ARCH_DIR}
 $ENV{GUROBI_HOME}/lib
 ${BOOST_LIBRARYDIR}/stage/lib
)

add_definitions(
-D_CONSOLE
)

IF(APPLE)
  FIND_LIBRARY(COREFOUNDATION CoreFoundation)
  FIND_LIBRARY(IOKIT IOKit)
  LIST(APPEND EXTRA_LIBS ${COREFOUNDATION} ${IOKIT})
ENDIF(APPLE)

add_executable( milpLogicSortingNetworkOptimizer ${SOURCES} ${HEADERS})

target_link_libraries( milpLogicSortingNetworkOptimizer MilpLogic
# FlopCpp OsiDylp Dylp OsiVol Vol OsiSym Sym OsiCbc Cbc OsiClp Clp Cgl
#Osi CoinUtils 
#xprs xprb
${GUROBI_CPP} $ENV{GUROBI_MAIN_VERSION} 
ilocplex cplex$ENV{CPLEX_VERSION} concert 
${MATH_LIB} ${EXTRA_LIBS} ${THREAD}
${BOOST_TIMER_LIB}
${BOOST_SYSTEM_LIB}
${BOOST_CHRONO_LIB}
)

