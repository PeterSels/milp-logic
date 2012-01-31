cmake_minimum_required(VERSION 2.6)

project( libAnalytics )

CMAKE_POLICY(SET CMP0009 OLD)
file(GLOB_RECURSE HEADERS "*.h" "./include/*.h")

set(SOURCES 
  src/BiExponentialConvolution.cpp
  src/DataVectorCorrelator.cpp
  src/Exponential.cpp
  src/ExponentialFitter.cpp
  src/NewtonRaphsonsMethod.cpp
  src/Histogram.cpp
)

include_directories( 
  include
)

add_definitions( 
  -DWIN32
  -D_DEBUG
)

add_library( Analytics STATIC ${SOURCES} ${HEADERS})

