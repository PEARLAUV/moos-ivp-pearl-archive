#--------------------------------------------------------
# The CMakeLists.txt for:                   lib_anrp_util
# Author(s):                          Andrew Patrikalakis
#--------------------------------------------------------

FILE(GLOB SRC *.cpp *.c)

# Replaced SHARED with a variable because cross-compiling for my TS-7800 ARM board requires static linking.
ADD_LIBRARY(anrp_util ${SRC})

# Install Libararies
INSTALL( TARGETS anrp_util
   RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
   ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
