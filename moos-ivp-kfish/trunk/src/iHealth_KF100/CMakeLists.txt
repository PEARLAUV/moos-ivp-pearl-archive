#--------------------------------------------------------
# The CMakeLists.txt for:                       iHealth_KF
# Author(s): Michael "Misha" Novitzky                             
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iHealth_KF100 ${SRC})

TARGET_LINK_LIBRARIES(iHealth_KF100
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

