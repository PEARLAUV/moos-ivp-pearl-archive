#--------------------------------------------------------
# The CMakeLists.txt for:                   iActuationKFAC
# Author(s):                                Michael "Misha" Novitzky 
#--------------------------------------------------------

FILE(GLOB SRC *.h *.cpp)

ADD_EXECUTABLE(iActuationKFAC ${SRC})

TARGET_LINK_LIBRARIES(iActuationKFAC
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   clearpath
   m
   pthread
  )

