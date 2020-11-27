#!/bin/bash 

# M200_IP
#  Emulator running on same machine as vehicle:     localhost
#  Emulator running on different machine:           IP address of that machine (often 192.168.2.1)
#  Actual evan vehicle:                             192.168.5.1
#  Actual felix vehile:                             192.168.6.1
M200_IP="192.168.5.1"

# SHORE_IP
#  Emulation, shoreside running on same machine as vehicle: localhost
#  Emulation, shoreside running on a different machine:     IP address of that machine (often 192.168.2.1)
#  Actual vehicle:                                          IP address of the shoreside computer
SHORE_IP="192.168.1.150"

TIME_WARP=1
JUST_BUILD="no"
EVAN="no"
FELIX="no"
MOOS_FILE=""
BHV_FILE=""

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]                   \n" $0
	printf "Switches:                                   \n"
	printf "  --evan, -e           evan vehicle only    \n"
	printf "  --felix, -f          felix vehicle only   \n"
	printf "  --just_build, -j                          \n" 
	printf "  --help, -h                                \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--felix" -o "${ARGI}" = "-f" ] ; then
        FELIX="yes"
        VNAME="felix"
        VPORT="9006"
        SHARE_LISTEN="9306"
        LOITER_PT="x=50,y=10"
        RETURN_PT="0,-20"
        MOOS_FILE="targ_felix.moos"
        BHV_FILE="targ_felix.bhv"
        printf "FELIX vehicle selected.\n"
    elif [ "${ARGI}" = "--evan" -o "${ARGI}" = "-e" ] ; then
        EVAN="yes"
        VNAME="evan"
        VPORT="9005"
        SHARE_LISTEN="9305"
        LOITER_PT="x=50,y=0"
        RETURN_PT="30,-15"
        MOOS_FILE="targ_evan.moos"
        BHV_FILE="targ_evan.bhv"
        printf "EVAN vehicle selected.\n"
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_BUILD="yes"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Handle Ill-formed command-line arguments
#-------------------------------------------------------

if [ "${EVAN}" = "no" -a "${FELIX}" = "no" ] ; then
    printf "A vehicle MUST be selected. Use --evan or --felix. \n"
    exit 0
elif [ "${EVAN}" = "yes" -a "${FELIX}" = "yes" ] ; then
    printf "ONE vehicle MUST be selected. Use --evan or --felix. Not both. \n"
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------

printf "Assembling MOOS file ${MOOS_FILE}\n"

CRUISESPEED="1.5"
SHORE_LISTEN="9300"

nsplug meta_vehicle.moos ${MOOS_FILE} -f                   \
       VNAME=$VNAME          SHORE_LISTEN=$SHORE_LISTEN    \
       WARP=$TIME_WARP       SHARE_LISTEN=$SHARE_LISTEN    \
       SHORE_IP=$SHORE_IP    VPORT=$VPORT                  \
       START_POS="0,0"       HOSTIP_FORCE="localhost" 

#nsplug meta_vehicle_fld.moos ${MOOS_FILE} -f \
#       VNAME=$VNAME                          \
#       VPORT=$VPORT                          \
#       WARP=$TIME_WARP                       \
#       SHARE_LISTEN=$SHARE_LISTEN            \
#       SHORE_LISTEN=$SHORE_LISTEN            \
#       SHORE_IP=$SHORE_IP                    \
#       M200_IP=$M200_IP                      \
#       HOSTIP_FORCE="localhost" 

printf "Assembling BHV file $BHV_FILE\n"
nsplug meta_vehicle.bhv $BHV_FILE -f         \
       VNAME=$VNAME                          \
       SPEED=$CRUISESPEED                    \
       ORDER="normal"

if [ ${JUST_BUILD} = "yes" ] ; then
    printf "Files assembled; vehicle not launched; exiting per request.\n"
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------

printf "Launching $VNAME MOOS Community \n"
pAntler $MOOS_FILE >& /dev/null &
uMAC $MOOS_FILE

# %1 matches the PID of the first job in the active jobs list, 
# namely the pAntler job launched in Part 4.
if [ "${ANSWER}" = "2" ]; then
    printf "Killing all processes ... \n "
    kill %1 
    printf "Done killing processes.   \n "
fi

