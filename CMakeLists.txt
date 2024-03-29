# Copyright (C) 2011 by Daniel Hauagge
#
# Permission is hereby granted, free  of charge, to any person obtaining
# a  copy  of this  software  and  associated  documentation files  (the
# "Software"), to  deal in  the Software without  restriction, including
# without limitation  the rights to  use, copy, modify,  merge, publish,
# distribute,  sublicense, and/or sell  copies of  the Software,  and to
# permit persons to whom the Software  is furnished to do so, subject to
# the following conditions:
#
# The  above  copyright  notice  and  this permission  notice  shall  be
# included in all copies or substantial portions of the Software.
#
# THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
# EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
# MERCHANTABILITY,    FITNESS    FOR    A   PARTICULAR    PURPOSE    AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE,  ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(CommandLineCore)
SUBDIRS(test)

# Boost
FIND_PACKAGE( Boost COMPONENTS timer system REQUIRED )
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

INCLUDE_DIRECTORIES(".")

SET(CMDC_SOURCES 
    CMDCore/cmdcore cmdcore.cpp
    CMDCore/logger logger.cpp 
    CMDCore/timer timer.cpp
    CMDCore/autotimer autotimer.cpp
    CMDCore/optparser optparser.cpp
    )

SET(CMDC_LIBS ${Boost_LIBRARIES} )

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Build a universal binary
  #SET( CMAKE_OSX_ARCHITECTURES x86_64;i386 )
  SET( CMAKE_OSX_ARCHITECTURES x86_64 )

  # The framework
  ADD_LIBRARY( CMDCore SHARED ${CMDC_SOURCES})
  TARGET_LINK_LIBRARIES(CMDCore ${CMDC_LIBS})

  # MacOS Framework
  SET_TARGET_PROPERTIES( CMDCore PROPERTIES
    FRAMEWORK TRUE
    FRAMEWORK_VERSION Current
    PUBLIC_HEADER "CMDCore/logger;CMDCore/cmdcore;CMDCore/autotimer;CMDCore/optparser;CMDCore/timer" 
    DEBUG_POSTIFX -d
    )
  
  INSTALL(TARGETS CMDCore
    FRAMEWORK DESTINATION "/Library/Frameworks"
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )

  SET(CMDC_LIBNAME CMDCore)
ELSE()
  ADD_LIBRARY(CMDCore SHARED ${CMDC_SOURCES})
  TARGET_LINK_LIBRARIES(CMDCore ${CMDC_LIBS})
  
  INSTALL_FILES(/include/CMDCore FILES CMDCore/cmdcore CMDCore/timer CMDCore/autotimer CMDCore/optparser CMDCore/logger)
  INSTALL_TARGETS(/lib CMDCore)
  #INSTALL_TARGETS(/lib RUNTIME_DIRECTORY /bin SharedLibraryTarget)

  SET(CMDC_LIBNAME CMDCore)

  # Generating the .pc file for pkg-config
  CONFIGURE_FILE("cmdcore.pc.in" "${CMAKE_BINARY_DIR}/cmdcore.pc" @ONLY IMMEDIATE)
  INSTALL(FILES ${CMAKE_BINARY_DIR}/cmdcore.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
ENDIF()

