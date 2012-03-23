cmake_minimum_required(VERSION 2.6)

project( libStringUtilities )

CMAKE_POLICY(SET CMP0009 OLD)

if(NOT WIN32)
  file(GLOB_RECURSE HEADERS "*.h" "./include/*.h")
endif()

set(SOURCES 
  src/StringUtilities.cpp
  src/MyColors.cpp
)

include_directories( 
  include
)

add_definitions( 
##  -DWIN32
##  -D_DEBUG
)

add_library( StringUtilities STATIC ${SOURCES} ${HEADERS})

