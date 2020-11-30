#--------------------------------------------------------
# The CMakeLists.txt for:                      iM200
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

SET(SRC 
  M200.cpp
  M200_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iM200 ${SRC})
   
TARGET_LINK_LIBRARIES(iM200
   ${MOOS_LIBRARIES}
   NMEAParse
   geometry
   mbutil
   ${SYSTEM_LIBS} )
