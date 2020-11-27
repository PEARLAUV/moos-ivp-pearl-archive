#--------------------------------------------------------
# The CMakeLists.txt for:                         iGPS_KF
# Author(s):    Paul Newman, modified Andrew Patrikalakis
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iGPS_KF ${SRC})
   
TARGET_LINK_LIBRARIES(iGPS_KF
   MOOS
   MOOSGen
   MOOSUtility
   m
   pthread)
