#!/bin/bash 

MULTICAST1="multicast_1"
WARP=10
JUST_BUILD="no"

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
    printf "%s [SWITCHES] [time_warp]   \n" $0
    printf "  --just_make, -j    \n" 
    printf "  --help, -h         \n" 
    exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" ]; then 
        WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
    JUST_BUILD="yes"
    else 
    printf "Bad Argument: %s \n" $ARGI
    exit 0
    fi
done

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------

SHORE_LISTEN="9300"

VNAME1="sim200"
VPORT1="9001"
SHARE_LISTEN1="9301"
RETURN_PT1="30,-20"
CRUISESPEED1="1.3"

nsplug meta_vehicle_sim.moos targ_sim200.moos -f        \
        VNAME=$VNAME1                                   \
        VPORT=$VPORT1                                   \
        WARP=$WARP                                      \
        SHARE_LISTEN=$SHARE_LISTEN1                     \
        SHORE_LISTEN=$SHORE_LISTEN                      \
        START_POS=$RETURN_PT1                           \
        MULTICAST=$MULTICAST1

nsplug meta_vehicle.bhv targ_sim200.bhv -f              \
        SPEED=$CRUISESPEED1                             \
        RETURN_PT=$RETURN_PT1                           

SHORESIDE_PORT="9000"

nsplug meta_shoreside.moos targ_shoreside.moos -f                 \
       WARP=$WARP                                                 \
       SNAME="shoreside"                                          \
       SHARE_LISTEN=$SHORE_LISTEN                                 \
       SPORT=$SHORESIDE_PORT                                      \
       MULTICAST=$MULTICAST1

if [ "${JUST_BUILD}" = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching sim200 MOOS Community (WARP=%s) \n" $WARP
pAntler targ_sim200.moos >& /dev/null &
sleep .25
printf "Launching Shoreside MOOS Community (WARP=%s) \n"  $WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos
#sleep 50

printf "Killing all processes ... \n"
mykill
kill %1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13
printf "Done killing processes.   \n"


