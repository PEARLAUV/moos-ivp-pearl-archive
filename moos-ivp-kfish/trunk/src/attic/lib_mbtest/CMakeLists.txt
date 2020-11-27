#--------------------------------------------------------
# The CMakeLists.txt for:                      lib_mbtest
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  TestUtils.cpp
)

SET(HEADERS
  TestUtils.h
)

# Build Library
ADD_LIBRARY(mbtest ${SRC})

# Install Headers
#INSTALL( FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/lib_mbtest)

# Install Libararies
INSTALL( TARGETS mbtest
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
