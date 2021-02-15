#------------------------------------------
# The CMakeLists.txt for: uSolarHeading
#------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uSolarHeading ${SRC})

TARGET_LINK_LIBRARIES(uSolarHeading
   ${MOOS_LIBRARIES}
   SimpleSerial
   mbutil
   ${SYSTEM_LIBS} )