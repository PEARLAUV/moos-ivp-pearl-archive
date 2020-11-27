#--------------------------------------------------------
# The CMakeLists.txt for:                       iNewKF
# Author(s):                                Alon Yaari 
#--------------------------------------------------------

FILE(GLOB SRC *.h *.cpp)

ADD_EXECUTABLE(iNewKF ${SRC})

TARGET_LINK_LIBRARIES(iNewKF
   MOOS
   MOOSGen
   mbutil
   websockets
   jansson
   m
   pthread
  )

