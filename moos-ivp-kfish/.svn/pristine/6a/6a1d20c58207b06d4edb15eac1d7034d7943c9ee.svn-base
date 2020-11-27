#--------------------------------------------------------
# The CMakeLists.txt for:                          uJSON
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uJSON ${SRC})
   
TARGET_LINK_LIBRARIES(uJSON
   ${MOOS_LIBRARIES}
   gpsParser
   mbutil
   m
   pthread)
