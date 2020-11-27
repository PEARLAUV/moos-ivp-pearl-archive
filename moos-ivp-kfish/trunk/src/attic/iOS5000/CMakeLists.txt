#--------------------------------------------------------
# The CMakeLists.txt for:                         iOS5000
# Author(s):                          Andrew Patrikalakis
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iOS5000 ${SRC})
   
TARGET_LINK_LIBRARIES(iOS5000 
   ${MOOS_LIBRARIES}
//   anrp_util
   m
   pthread)
