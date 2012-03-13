cmake_minimum_required(VERSION 2.6)

project( libAnalyticsTest )

set(SOURCES 
  src/libAnalyticsTest.cpp
)

include_directories( 
  ${CMAKE_SOURCE_DIR}/libAnalytics/include
)

link_directories(
  ${CMAKE_SOURCE_DIR}/libAnalytics
)

add_definitions( 
##  -DWIN32
##  -D_CONSOLE
##  -D_DEBUG
)

add_executable( libAnalyticsTest ${SOURCES} )

target_link_libraries( libAnalyticsTest Analytics)

