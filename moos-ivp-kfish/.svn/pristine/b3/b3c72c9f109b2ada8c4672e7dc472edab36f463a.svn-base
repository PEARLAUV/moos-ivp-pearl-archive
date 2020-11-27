## LAB7

#!/bin/bash 

SHORE_STATION_IP=192.168.1.209
#MULTICAST=multicast_2
MULTICAST1="multicast_8"

WARP=1
HELP="no"
JUST_BUILD="no"
ARCHIE="no"
BETTY="no"
CHARLIE="no"
DAVIS="no"
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
    if [ "${ARGI}" = "--davis" -o "${ARGI}" = "-d" ] ; then
        DAVIS="yes"
        UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--charlie" -o "${ARGI}" = '-c' ] ; then
        CHARLIE="yes"
        UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--betty" -o "${ARGI}" = "-b" ] ; then
      BETTY="yes"
      UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--archie" -o "${ARGI}" = "-a" ] ; then
      ARCHIE="yes"
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

#if [ "${ARCHIE}" = "no" -a "${BETTY}" = "no" ] ; then
#    printf "ONE vehicle MUST be selected!!!!!!!!!!!! \n"
#    HELP="yes"
#fi

if [ "${ARCHIE}" = "yes" -a "${BETTY}" = "yes" ] ; then
    printf "ONE vehicle MUST be selected!!!!!!!!!!!! \n"
    HELP="yes"
fi

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]            \n" $0
    printf "Switches:                \n"
    printf "  --archie, -a           archie vehicle only                   \n"
    printf "  --betty, -b            betty vehicle only                    \n"
    printf "  --charlie, -c          charlie vehicle only                  \n"
    printf "  --davis, -d            davis vehicle only                    \n"
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    exit 0;
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------

CRUISESPEED1="1.5"
CRUISESPEED2="1.5"
CRUISESPEED3="1.5"
CRUISESPEED4="1.5"
SHORE_LISTEN="9300"

VNAME1="archie"
VPORT1="9001"
SHARE_LISTEN1="9301"
LOITER_PT1="x=-10,y=-60"
RETURN_PT1="0,-20"

VNAME2="betty"
VPORT2="9002"
SHARE_LISTEN2="9302"
LOITER_PT2="x=50,y=-40"
RETURN_PT2="30,-10"

VNAME3="charlie"
VPORT3="9003"
SHARE_LISTEN3="9303"
LOITER_PT3="x=50,y=-20"
RETURN_PT3="30,0"

VNAME4="davis"
VPORT4="9004"
SHARE_LISTEN4="9304"
LOITER_PT4="x=50,y=-10"
RETURN_PT4="30-10"

# Conditionally Prepare Archie files
if [ "${ARCHIE}" = "yes" ]; then
    nsplug meta_vehicle_fld.moos targ_archie.moos -f      \
        VNAME=$VNAME1                                     \
	VPORT=$VPORT1                                     \
	WARP=$WARP                                        \
	SHARE_LISTEN=$SHARE_LISTEN1                       \
	SHORE_LISTEN=$SHORE_LISTEN                        \
        MULTICAST=$MULTICAST1

    nsplug meta_vehicle.bhv targ_archie.bhv -f            \
        VNAME=$VNAME1                                     \
	SPEED=$CRUISESPEED1                          \
        ORDER="normal"
fi

# Conditionally Prepare Betty files
if [ "${BETTY}" = "yes" ]; then
    nsplug meta_vehicle_fld.moos targ_betty.moos -f       \
        VNAME=$VNAME2                                     \
	VPORT=$VPORT2                                     \
	WARP=$WARP                                        \
	SHARE_LISTEN=$SHARE_LISTEN2                       \
	SHORE_LISTEN=$SHORE_LISTEN                        \
        MULTICAST=$MULTICAST1

    nsplug meta_vehicle.bhv targ_betty.bhv -f             \
        VNAME=$VNAME2                                     \
	SPEED=$CRUISESPEED2                          \
        ORDER="reverse"
fi

# Conditionally Prepare Charlie files
if [ "${CHARLIE}" = "yes" ]; then
    nsplug meta_vehicle_fld.moos targ_charlie.moos -f   \
        VNAME=$VNAME3                                   \
        VPORT=$VPORT3                                   \
        WARP=$WARP                                      \
        SHARE_LISTEN=$SHARE_LISTEN3                     \
        SHORE_LISTEN=$SHORE_LISTEN                      \
        MULTICAST=$MULTICAST1

     nsplug meta_vehicle.bhv targ_charlie.bhv -f       \
        VNAME=$VNAME3                                  \
        SPEED=$CRUISESPEED3                             \
        ORDER="normal"
fi

# Conditionally Prepare Davis files
if [ "${DAVIS}" = "yes" ]; then
    nsplug meta_vehicle_fld.moos targ_davis.moos -f   \
        VNAME=$VNAME4                                   \
        VPORT=$VPORT4                                   \
        WARP=$WARP                                      \
        SHARE_LISTEN=$SHARE_LISTEN4                     \
        SHORE_LISTEN=$SHORE_LISTEN                      \
        MULTICAST=$MULTICAST1

     nsplug meta_vehicle.bhv targ_davis.bhv -f       \
        VNAME=$VNAME4                                  \
        SPEED=$CRUISESPEED4                             \
        ORDER="normal"
fi

if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------

# Launch Archie
if [ "${ARCHIE}" = "yes" ]; then
    printf "Launching Archie MOOS Community \n"
    pAntler targ_archie.moos >& /dev/null &
fi

# Launch Betty
if [ "${BETTY}" = "yes" ]; then
    printf "Launching Betty MOOS Community \n"
    pAntler targ_betty.moos >& /dev/null &
fi

# Launch Charlie
if [   "${CHARLIE}" = "yes" ] ; then
      printf "Launching Charlie MOOS Community \n"
      pAntler targ_charlie.moos >& /dev/null &
fi

# Launch Davis
if [   "${DAVIS}" = "yes" ] ; then
      printf "Launching DAVIS MOOS Community \n"
      pAntler targ_davis.moos >& /dev/null &
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
    kill %1 
    printf "Done killing processes.   \n "
fi


