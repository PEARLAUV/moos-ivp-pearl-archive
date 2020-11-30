#!/bin/bash 

JUST_MAKE="no"
VNAME="UNSET"

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --evan,  -e         \n" 
	printf "  --felix, -f         \n" 
	printf "  --help,  -h         \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--evan" -o "${ARGI}" = "-e" ] ; then
	VNAME="evan"
	VPORT="9005"
	SHARE_LISTEN=9305
    elif [ "${ARGI}" = "--felix" -o "${ARGI}" = "-f" ] ; then
	VNAME="felix"
	VPORT="9006"
	SHARE_LISTEN=9306
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

if [ ${VNAME} = "UNSET" ] ; then
    printf "Vehicle name not chosen. Re-launch with -h for options \n"
    exit 0
fi

#-------------------------------------------------------
#  Part 2: Create the .moos file 
#-------------------------------------------------------
SHORE_LISTEN=9300

nsplug meta_vehicle_thrust_test.moos targ_$VNAME.moos -f   \
        VNAME=$VNAME    SHARE_LISTEN=$SHARE_LISTEN \
        VPORT=$VPORT    SHORE_LISTEN=$SHORE_LISTEN \
        MULTICAST="multicast_8"

if [ ! -e targ_$VNAME.moos ]; then echo "no targ_$VNAME.moos"; exit; fi
if [ ${JUST_MAKE} = "yes" ]; then exit; fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------

printf "Launching $VNAME MOOS Community \n"
pAntler targ_$VNAME.moos >& /dev/null &

uMAC targ_$VNAME.moos

printf "Killing all processes ... \n"
mykill
printf "Done killing processes.   \n"
