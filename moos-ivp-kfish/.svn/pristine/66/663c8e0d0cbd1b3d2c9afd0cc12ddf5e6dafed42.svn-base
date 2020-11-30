#--------------------------------------------------------
# The CMakeLists.txt for:                   iKFController
# Author(s):                                Mike Benjamin  
#--------------------------------------------------------

SET(SRC
   KFC_ExampleConfig.cpp  
   KFC_MOOSApp.cpp        
   main.cpp
)




ADD_EXECUTABLE(iKFController ${SRC})

TARGET_LINK_LIBRARIES(iKFController
   ${MOOS_LIBRARIES}
   mbutil
   clearpath
   m
   pthread)

