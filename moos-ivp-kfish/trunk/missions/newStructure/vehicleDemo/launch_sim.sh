
MULTICAST1="multicast_1"

TIME_WARP=1
HELP="no"
JUST_BUILD="no"
BAD_ARGS=""

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    UNDEFINED_ARG=$ARGI
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        HELP="yes"
        UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
        JUST_BUILD="yes"
    fi
    if [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then
        TIME_WARP=$ARGI
        UNDEFINED_ARG=""
    fi
    if [ "${UNDEFINED_ARG}" != "" ] ; then
        BAD_ARGS=$UNDEFINED_ARG
    fi
done

#-------------------------------------------------------
#  Part 2: Handle Ill-formed command-line arguments
#-------------------------------------------------------


if [ "${BAD_ARGS}" != "" ] ; then
    printf "Bad Argument: %s \n" $BAD_ARGS
    exit 0
fi

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]            \n" $0
    printf "Switches:                \n"
    printf "  --just_build, -j       \n"
    printf "  --help, -h             \n"
    exit 0;
fi


#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files.
#-------------------------------------------------------

#For simplicity using the same .bhv file for sim and real

SHORE_LISTEN="9300"
SHORESIDE_PORT="9000"

VNAME5="evan"
VPORT5="9005"
SHARE_LISTEN5="9305"
LOITER_POS5="x=50,y=0"
START_POS5="30,-20"
RETURN_PT5="30,-20"

VNAME6="felix"
VPORT6="9006"
SHARE_LISTEN6="9306"
LOITER_POS6="x=50,y=10"
START_POS6="30,-25"
RETURN_PT6="30,-25"

nsplug meta_vehicle_sim.moos targ_veh_sim_evan.moos -f      \
        -p ../../sim_plugs/  \
        VNAME=$VNAME5                                   \
        VPORT=$VPORT5                                   \
        WARP=$WARP                                      \
        SHARE_LISTEN=$SHARE_LISTEN5                     \
        SHORE_LISTEN=$SHORE_LISTEN                      \
        START_POS=START_POS5                            \
        MULTICAST=$MULTICAST1

nsplug meta_brains.moos targ_brains_evan.moos -f -p ../../sim_plugs/ \
        VNAME=$VNAME5 \
        VPORT=$VPORT5 \
        WARP=$WARP \
        SHARE_LISTEN=$SHARE_LISTEN5 \
        SHORE_LISTEn=$SHORE_LISTEN \
        MULTICAST=$MULTICAST1

nsplug meta_vehicle.bhv targ_evan.bhv -f       \
        VNAME=$VNAME5                                  \
        SPEED=$CRUISESPEED5                             \
        ORDER="reverse" \
        CRUISESPEED=$CRUISESPEED5                         \
        RETURN_PT=$RETURN_PT5                             \
        LOITER_POS=$LOITER_POS5                             \
        START_POS=$START_POS5

nsplug meta_vehicle_sim.moos targ_veh_sim_felix.moos -f -p ../../sim_plugs/ \
        VNAME=$VNAME6                                   \
        VPORT=$VPORT6                                   \
        WARP=$WARP                                      \
        SHARE_LISTEN=$SHARE_LISTEN6                     \
        SHORE_LISTEN=$SHORE_LISTEN                      \
        START_POS=START_POS5                            \
        MULTICAST=$MULTICAST1

nsplug meta_brains.moos targ_brains_felix.moos -f -p ../../sim_plugs/ \
        VNAME=$VNAME6 \
        VPORT=$VPORT6 \
        WARP=$WARP \
        SHARE_LISTEN=$SHARE_LISTEN6 \
        SHORE_LISTEn=$SHORE_LISTEN \
        MULTICAST=$MULTICAST1

nsplug meta_vehicle.bhv targ_felix.bhv -f       \
        VNAME=$VNAME6                                  \
        SPEED=$CRUISESPEED6                             \
        ORDER="reverse" \
        CRUISESPEED=$CRUISESPEED6                         \
        RETURN_PT=$RETURN_PT6                             \
        LOITER_POS=$LOITER_POS6                             \
        START_POS=$START_POS6

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
        SNAME="shoreside" SHARE_LISTEN=$SHORE_LISTEN SPORT=$SHORESIDE_PORT \
        MULTICAST=$MULCTICAST1

if [ "${JUST_MAKE}" = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching Evan MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_brains_evan.moos >& /dev/null &
sleep .25
pAntler targ_veh_sim_evan.moos >& /dev/null &
sleep .25
printf "Launching Felix MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_brains_felix.moos >& /dev/null &
sleep .25
pAntler targ_veh_sim_felix.moos >& /dev/null &
sleep .25
printf "Launching Shoreside MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos
#sleep 50

printf "Killing all processes ... \n"
mykill
kill %1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13
printf "Done killing processes.   \n"


