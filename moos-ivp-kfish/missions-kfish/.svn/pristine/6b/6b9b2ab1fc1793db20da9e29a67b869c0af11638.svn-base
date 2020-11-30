#!/bin/bash 

MULTICAST1="multicast_1"
WARP=1
HELP="no"
JUST_BUILD="no"
EVAN="no"
FELIX="no"
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
    if [ "${ARGI}" = "--felix" -o "${ARGI}" = "-f" ] ; then
    	FELIX="yes"
    	UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--evan" -o "${ARGI}" = "-e" ] ; then
    	EVAN="yes"
    	UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_BUILD="yes"
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
    printf "  --evan, -e	     evan vehicle only                     \n"
    printf "  --felix, -f	     evan vehicle only                     \n"
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    exit 0;
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------

SHORE_LISTEN="9300"

VNAME5="evan"
VPORT5="9005"
SHARE_LISTEN5="9305"
RETURN_PT5="30,-20"
CRUISESPEED5="1.3"

VNAME6="felix"
VPORT6="9006"
SHARE_LISTEN6="9306"
RETURN_PT6="30,-25"
CRUISESPEED6="1.3"

# Conditionally Prepare Evan files
if [ "${EVAN}" = "yes" ]; then
    nsplug meta_vehicle_fld_m200.moos targ_evan.moos -f      \
        VNAME=$VNAME5                                        \
        VPORT=$VPORT5                                        \
        WARP="1"                                             \
        SHARE_LISTEN=$SHARE_LISTEN5                          \
        SHORE_LISTEN=$SHORE_LISTEN                           \
        MULTICAST=$MULTICAST1

     nsplug meta_vehicle.bhv targ_evan.bhv -f                \
        SPEED=$CRUISESPEED5                                  \
        RETURN_PT=$RETURN_PT5                             
fi

# Conditionally Prepare Felix files
if [ "${FELIX}" = "yes" ]; then
    nsplug meta_vehicle_fld_m200.moos targ_felix.moos -f     \
        VNAME=$VNAME6                                        \
        VPORT=$VPORT6                                        \
        WARP="1"                                             \
        SHARE_LISTEN=$SHARE_LISTEN6                          \
        SHORE_LISTEN=$SHORE_LISTEN                           \
        MULTICAST=$MULTICAST1

     nsplug meta_vehicle.bhv targ_felix.bhv -f               \
        SPEED=$CRUISESPEED6                                  \
        RETURN_PT=$RETURN_PT6                             
fi


#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------

# Launch Evan
if [   "${EVAN}" = "yes" ] ; then
    printf "Launching Evan MOOS Community \n"
    pAntler targ_evan.moos >& /dev/null &
fi

# Launch Felix
if [   "${FELIX}" = "yes" ] ; then
    printf "Launching Felix MOOS Community \n"
    pAntler targ_felix.moos >& /dev/null &
fi

ANSWER="0"
while [ "${ANSWER}" != "1" -a "${ANSWER}" != "2" ]; do
    printf "Now what? (1) Exit script (2) Exit and Kill Simulation \n"
    printf "> "
    read ANSWER
done

# %1 matches the PID of the first job in the active jobs list, 
# namely the pAntler job launched in Part 4.
if [ "${ANSWER}" = "2" ]; then
    printf "Killing all processes ... \n "
    kill %1 %2 
    printf "Done killing processes.   \n "
fi


