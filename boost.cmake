set(BOOST_VERSION 1_50)

set(BOOST_TIMER_LIB boost_timer)
set(BOOST_CHRONO_LIB boost_chrono)
set(BOOST_REGEX_LIB boost_regex)
IF(APPLE)
 set(BOOST_ROOT /usr/local/boost_${BOOST_VERSION}_0)
ELSEIF(WIN32)
 set(BOOST_ROOT c:/boost_${BOOST_VERSION}_0)
 set(BOOST_TIMER_LIB libboost_timer-vc-90-mt-sgd-${BOOST_VERSION})
 set(BOOST_CHRONO_LIB libboost_chrono-vc-90-mt-sgd-${BOOST_VERSION})
 set(BOOST_REGEX_LIB libboost_regex-vc90-mt-sgd-${BOOST_VERSION})
ELSE()
 set(BOOST_ROOT /usr/local/boost_${BOOST_VERSION}_0)
ENDIF()

SET(BOOST_DIR ${BOOST_ROOT})

SET(BOOST_INCLUDEDIR ${BOOST_ROOT})
SET(BOOST_LIBRARYDIR ${BOOST_ROOT})

#SET(BOOST_COMPILER xgcc42)

