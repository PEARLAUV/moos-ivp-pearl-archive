#--------------------------------------------------------
# The CMakeLists.txt for:                      iuModemMP
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

ADD_EXECUTABLE(iuModemMP ${SRC})
   
TARGET_LINK_LIBRARIES(iuModemMP
   ${MOOSGeodesy_LIBRARIES}
   ${MOOS_LIBRARIES}
MOOS
geometry
mbutil
   NMEAParse
   SimpleSerial
   ${SYSTEM_LIBS} )
