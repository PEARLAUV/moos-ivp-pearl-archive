#--------------------------------------------------------
# The CMakeLists.txt for:                   iActuationKF
# Author(s):                                Alon Yaari 
#--------------------------------------------------------

FILE(GLOB SRC *.h *.cpp)

ADD_EXECUTABLE(iActuationKF ${SRC})

TARGET_LINK_LIBRARIES(iActuationKF
   ${MOOS_LIBRARIES}
   mbutil
   clearpath
   m
   pthread
  )

