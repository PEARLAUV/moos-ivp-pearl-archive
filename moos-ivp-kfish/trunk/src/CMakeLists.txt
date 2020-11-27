##############################################################################
# FILE:        moos-ivp-kfish/src/CMakeLists.txt
# DATE:        2011/03/22
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-kfish source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_* ../lib)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})
LINK_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

SET (KFISH-OPTIONS $ENV{KFISHOPTIONS})

#============================================================================
# List the subdirectories to build...
#============================================================================

IF ( KFISH-OPTIONS STREQUAL all )
     ADD_SUBDIRECTORY(lib_clearpath)
     ADD_SUBDIRECTORY(lib_NMEAParse)
     ADD_SUBDIRECTORY(lib_SimpleSerial)
     ADD_SUBDIRECTORY(iActuationKFAC)
     ADD_SUBDIRECTORY(iOS5000AC)
     ADD_SUBDIRECTORY(iHealth_KF100)
     ADD_SUBDIRECTORY(iM200)
     ADD_SUBDIRECTORY(uSimM200)
     ADD_SUBDIRECTORY(lib_gpsParser)
     ADD_SUBDIRECTORY(iGPSDevice)
     ADD_SUBDIRECTORY(iGPSSim)
     ADD_SUBDIRECTORY(uJSON)

ELSEIF ( KFISH-OPTIONS STREQUAL emulator )
     ADD_SUBDIRECTORY(lib_NMEAParse)
     ADD_SUBDIRECTORY(lib_gpsParser)
     ADD_SUBDIRECTORY(lib_SimpleSerial)
     ADD_SUBDIRECTORY(uSimM200)
     ADD_SUBDIRECTORY(iGPSSim)
     ADD_SUBDIRECTORY(iM200)

ELSEIF ( KFISH-OPTIONS STREQUAL m100)
     ADD_SUBDIRECTORY(lib_clearpath)
     ADD_SUBDIRECTORY(lib_NMEAParse)
     ADD_SUBDIRECTORY(lib_SimpleSerial)
     ADD_SUBDIRECTORY(iActuationKFAC)
     ADD_SUBDIRECTORY(iOS5000AC)
     ADD_SUBDIRECTORY(iHealth_KF100)
     ADD_SUBDIRECTORY(lib_gpsParser)
     ADD_SUBDIRECTORY(iGPSDevice)
     ADD_SUBDIRECTORY(iGPSSim)
     ADD_SUBDIRECTORY(uJSON)

ELSEIF ( KFISH-OPTIONS STREQUAL m200)
     ADD_SUBDIRECTORY(lib_NMEAParse)
     ADD_SUBDIRECTORY(lib_SimpleSerial)
     ADD_SUBDIRECTORY(iM200)
     ADD_SUBDIRECTORY(uSimM200)
     ADD_SUBDIRECTORY(lib_gpsParser)
     ADD_SUBDIRECTORY(iGPSDevice)
     ADD_SUBDIRECTORY(iGPSSim)
     ADD_SUBDIRECTORY(uJSON)

ELSEIF ( KFISH-OPTIONS STREQUAL rex4 )
     ADD_SUBDIRECTORY(lib_NMEAParse)
     ADD_SUBDIRECTORY(lib_SimpleSerial)
     ADD_SUBDIRECTORY(lib_gpsParser)
     ADD_SUBDIRECTORY(iGPSDevice)
     ADD_SUBDIRECTORY(iGPSSim)
     ADD_SUBDIRECTORY(uJSON)
     ADD_SUBDIRECTORY(iOS5000AC)


ELSEIF ( KFISH-OPTIONS STREQUAL test )
     ADD_SUBDIRECTORY(lib_NMEAParse)
     ADD_SUBDIRECTORY(lib_SimpleSerial)
     ADD_SUBDIRECTORY(iuModemMP)
     
ENDIF ()

# Retired
#ADD_SUBDIRECTORY(lib_anrp_util)
#ADD_SUBDIRECTORY(iGPS_KFAC)
#ADD_SUBDIRECTORY(app_botinfo)
#ADD_SUBDIRECTORY(app_keyboard)
#ADD_SUBDIRECTORY(iKFController)


##############################################################################
#                           END of CMakeLists.txt
##############################################################################

