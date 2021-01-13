##############################################################################
# FILE:        moos-ivp-extend/src/CMakeLists.txt
# DATE:        2010/09/07
#              2020/05/09 minor mods
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_* ../lib)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})
LINK_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

SET (PEARL-OPTIONS $ENV{PEARLOPTIONS})

#============================================================================
# List the subdirectories to build...
#============================================================================
IF ( PEARL-OPTIONS STREQUAL all )
     ADD_SUBDIRECTORY(lib_NMEAParse)
     ADD_SUBDIRECTORY(lib_SimpleSerial)
#d     ADD_SUBDIRECTORY(iActuationKFAC)
#d     ADD_SUBDIRECTORY(iOS5000AC)
#d     ADD_SUBDIRECTORY(iHealth_KF100)
#d     ADD_SUBDIRECTORY(iM200)
#d     ADD_SUBDIRECTORY(uSimM200)
     ADD_SUBDIRECTORY(lib_gpsParser)
#d     ADD_SUBDIRECTORY(iGPSDevice)
     ADD_SUBDIRECTORY(iGPSSim)
     ADD_SUBDIRECTORY(iGPS)

ELSEIF ( PEARL-OPTIONS STREQUAL emulator )
     ADD_SUBDIRECTORY(lib_NMEAParse)
     ADD_SUBDIRECTORY(lib_gpsParser)
     ADD_SUBDIRECTORY(lib_SimpleSerial)
     ADD_SUBDIRECTORY(uSimM200)
     ADD_SUBDIRECTORY(iGPSSim)
     ADD_SUBDIRECTORY(iM200)

ENDIF ()

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

