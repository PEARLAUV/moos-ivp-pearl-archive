#!/bin/bash 

HOSTIP_FORCE="192.168.1.151"
SHORE_LISTEN="9300"
SHORESIDE_PORT="9000"
TIME_WARP=1
JUST_MAKE="no"

for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	printf "%s [SWITCHES] [time_warp]   \n" $0
	printf "  --just_make, -j    \n" 
	printf "  --help, -h         \n" 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
	printf "Bad Argument: %s \n" $ARGI
	exit 0
    fi
done

nsplug meta_uModem.moos targ_umodem.moos -f WARP=$TIME_WARP \
       HOSTIP_FORCE=$HOSTIP_FORCE 

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching umodem MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_umodem.moos >& /dev/null &
printf "Done \n"

uMAC targ_umodem.moos

printf "Killing all processes ... \n"
kill %1 
printf "Done killing processes.   \n"


