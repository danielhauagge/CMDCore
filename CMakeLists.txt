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

PROJECT(Logger)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE_DIRECTORIES(".")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Build a universal binary
  SET( CMAKE_OSX_ARCHITECTURES x86_64;i386 )

  # The framework
  ADD_LIBRARY( Logger 
    SHARED 
    logger  logger.cpp)

  # MacOS Framework
  SET_TARGET_PROPERTIES( Logger PROPERTIES
    FRAMEWORK TRUE
    FRAMEWORK_VERSION none
    PUBLIC_HEADER "logger" 
    DEBUG_POSTIFX -d
    )
  
  INSTALL(TARGETS Logger
    FRAMEWORK DESTINATION Library/Frameworks
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
ELSE()
  ADD_LIBRARY(logger SHARED
    logger.cpp)
  
  INSTALL_FILES(/include/Logger/  logger)
  INSTALL_TARGETS(/lib logger)
  #INSTALL_TARGETS(/lib RUNTIME_DIRECTORY /bin SharedLibraryTarget)
ENDIF()

ADD_EXECUTABLE( test test.cpp )
TARGET_LINK_LIBRARIES( test logger )

# Generating the .pc file for pkg-config
CONFIGURE_FILE("logger.pc.in" "${CMAKE_BINARY_DIR}/logger.pc" @ONLY IMMEDIATE)
INSTALL(FILES ${CMAKE_BINARY_DIR}/logger.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
