cmake_minimum_required(VERSION 2.6)

project( libAnalyticsTest )

set(SOURCES 
  src/libAnalyticsTest.cpp
)

include_directories( 
  ../libAnalytics/include
)

link_directories(
  ../libAnalytics
)

add_definitions( 
  -DWIN32
  -D_CONSOLE
  -D_DEBUG
)

add_executable( libAnalyticsTest WIN32 ${SOURCES} )

target_link_libraries( libAnalyticsTest Analytics)

