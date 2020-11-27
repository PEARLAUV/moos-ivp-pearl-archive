#--------------------------------------------------------
# The CMakeLists.txt for:                 iClapboard
# Author(s):                              Alon Yaari
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  # Windows Libraries
  SET(SYSTEM_LIBS
     wsock32 )
else (${WIN32})
  # Linux and Apple Libraries
  SET(SYSTEM_LIBS
      m
      pthread )
endif (${WIN32})

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iClapboard ${SRC})
   
TARGET_LINK_LIBRARIES(iClapboard
   ${MOOS_LIBRARIES}
   mbutil
   ${SYSTEM_LIBS} )
