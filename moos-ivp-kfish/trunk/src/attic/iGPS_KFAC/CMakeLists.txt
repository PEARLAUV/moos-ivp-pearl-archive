#--------------------------------------------------------
# The CMakeLists.txt for:                       iGPS_KFAC
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iGPS_KFAC ${SRC})
   
TARGET_LINK_LIBRARIES(iGPS_KFAC
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   NMEAParse
   SimpleSerial
   geometry
   mbutil
   m
   pthread)
