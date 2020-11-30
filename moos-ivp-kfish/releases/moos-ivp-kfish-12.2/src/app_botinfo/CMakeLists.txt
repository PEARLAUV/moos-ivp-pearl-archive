#--------------------------------------------------------
# The CMakeLists.txt for:                         botinfo
# Author(s):                                Mike Benjamin
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
   botinfo.cpp
)  

ADD_EXECUTABLE(botinfo ${SRC})

TARGET_LINK_LIBRARIES(botinfo
  mbtest
  clearpath
  ${SYSTEM_LIBS} )
