#--------------------------------------------------------
# The CMakeLists.txt for:                   keyboard_ctrl
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
   keyboard_raw_ctrl.cpp
)  

ADD_EXECUTABLE(keyboard_ctrl ${SRC})

TARGET_LINK_LIBRARIES(keyboard_ctrl
  clearpath
  ${SYSTEM_LIBS} )
