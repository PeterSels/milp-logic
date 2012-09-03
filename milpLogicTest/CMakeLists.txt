cmake_minimum_required(VERSION 2.6)

project( milpLogicTest )

IF(WIN32)
 SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /FORCE:MULTIPLE")
 SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug  /FORCE:MULTIPLE")
 SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /FORCE:MULTIPLE")
ENDIF()

file(GLOB_RECURSE HEADERS "*.h")

INCLUDE(${CMAKE_SOURCE_DIR}/boost.cmake)

set(SOURCES 
  src/milpLogicTest.cpp
)

include_directories( 
  include
  ${CMAKE_SOURCE_DIR}/libMilpLogic/include
  $ENV{CPLEX_HOME}/include
  $ENV{CONCERT_HOME}/include
  $ENV{GUROBI_HOME}/include
  $ENV{XPRESS_HOME}/include
)

IF(APPLE)
 set(CPLEX_ARCH_DIR x86-64_darwin9_gcc4.0/static_pic)
 set(GUROBI_CPP gurobi_c++)
 set(XPRESS_LIBS )
 set(MATH_LIB m)
 set(THREAD )
ELSEIF(WIN32)
 set(CPLEX_ARCH_DIR x64_windows_vs2008/stat_mta)
 set(GUROBI_CPP gurobi_c++mtd2008)
 set(XPRESS_LIBS xprs xprb)
 set(MATH_LIB )
 set(THREAD )
ELSE()
 set(CPLEX_ARCH_DIR x86-64_sles10_4.1/static_pic)
 set(GUROBI_CPP gurobi_c++)
 set(XPRESS_LIBS )
 set(MATH_LIB m)
 set(THREAD pthread)
ENDIF()

link_directories(
  ${CMAKE_SOURCE_DIR}/libMilpLogic
  $ENV{CPLEX_HOME}/lib/${CPLEX_ARCH_DIR}
  $ENV{CONCERT_HOME}/lib/${CPLEX_ARCH_DIR}
  $ENV{GUROBI_HOME}/lib
  $ENV{XPRESS_HOME}/lib
  ${BOOST_LIBRARYDIR}/stage/lib
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

add_executable( milpLogicTest ${SOURCES} ${HEADERS})

target_link_libraries( milpLogicTest MilpLogic
${GUROBI_CPP} $ENV{GUROBI_MAIN_VERSION} 
ilocplex cplex$ENV{CPLEX_VERSION} concert 
${XPRESS_LIBS}
${MATH_LIB} ${EXTRA_LIBS} ${THREAD}
${BOOST_TIMER_LIB}
${BOOST_SYSTEM_LIB}
${BOOST_CHRONO_LIB}
)

