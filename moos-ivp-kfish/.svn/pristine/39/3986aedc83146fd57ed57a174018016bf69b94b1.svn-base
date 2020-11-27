##############################################################################
# FILE:        moos-ivp-kfish/src/CMakeLists.txt
# DATE:        2011/03/22
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-extend source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_* ../lib)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})
LINK_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

#============================================================================
# List the subdirectories to build...
#============================================================================
ADD_SUBDIRECTORY(lib_mbtest)
ADD_SUBDIRECTORY(lib_clearpath)
ADD_SUBDIRECTORY(lib_anrp_util)
ADD_SUBDIRECTORY(app_botinfo)
ADD_SUBDIRECTORY(app_keyboard)
ADD_SUBDIRECTORY(iKFController)
ADD_SUBDIRECTORY(iActuationKF)
ADD_SUBDIRECTORY(iGPS_KF)
ADD_SUBDIRECTORY(iOS5000)
ADD_SUBDIRECTORY(iMetaCompass)
ADD_SUBDIRECTORY(iNewKF)

##############################################################################
#                           END of CMakeLists.txt
##############################################################################

