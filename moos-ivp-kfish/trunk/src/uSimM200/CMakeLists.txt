#--------------------------------------------------------
# The CMakeLists.txt for:                 uSimM200
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

ADD_EXECUTABLE(uSimM200 ${SRC})
   
TARGET_LINK_LIBRARIES(uSimM200
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   mbutil
   NMEAParse
   ${SYSTEM_LIBS} )

message("+++++++++++++++++++++++++++++++++++++++++")
message("SYSTEM_LIBS:" ${SYSTEM_LIBS})
message("+++++++++++++++++++++++++++++++++++++++++")
