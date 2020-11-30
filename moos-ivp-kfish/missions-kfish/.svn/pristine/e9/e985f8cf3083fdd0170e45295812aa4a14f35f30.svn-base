#!/bin/bash 

WARP=1
HELP="no"
JUST_BUILD="no"
ARCHIE="no"
BETTY="no"
CHARLIE="no"
DAVIS="no"
NUM_V=0
BAD_ARGS=""

#-------------------------------------------------------
#  Part 1: Process command-line arguments
#-------------------------------------------------------
printf "NUM_V: %d \n" $NUM_V
for ARGI; do
    UNDEFINED_ARG=$ARGI
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	HELP="yes"
	UNDEFINED_ARG=""
    fi
    if [ "${ARGI}" = "--davis" -o "${ARGI}" = "-d" ] ; then
      DAVIS="yes"
      UNDEFINED_ARG=""
      NUM_V="$(expr "$NUM_V" '+' '1')"
    fi
    if [ "${ARGI}" = "--charlie" -o "${ARGI}" = "-c" ] ; then
      CHARLIE="yes"
      UNDEFINED_ARG=""
      NUM_V="$(expr "$NUM_V" '+' '1')"
    fi
    if [ "${ARGI}" = "--betty" -o "${ARGI}" = "-b" ] ; then
      BETTY="yes"
      UNDEFINED_ARG=""
      NUM_V="$(expr "$NUM_V" '+' '1')"
    fi
    if [ "${ARGI}" = "--archie" -o "${ARGI}" = "-a" ] ; then
      ARCHIE="yes"
      UNDEFINED_ARG=""
      NUM_V="$(expr "$NUM_V" '+' '1')"
    fi
    if [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
      JUST_BUILD="yes"
      UNDEFINED_ARG=""
      NUM_V="$(expr "$NUM_V" '+' '1')"
    fi
    if [ "${UNDEFINED_ARG}" != "" ] ; then
	BAD_ARGS=$UNDEFINED_ARG
    fi
done
printf "NUM_V: %d \n" $NUM_V

#-------------------------------------------------------
#  Part 2: Handle Ill-formed command-line arguments
#-------------------------------------------------------


if [ "${BAD_ARGS}" != "" ] ; then
    printf "Bad Argument: %s \n" $BAD_ARGS
    exit 0
fi

if [ $NUM_V -gt 1 ] ; then
    printf "ONE vehicle MUST be selected!!!!!!!!!!!! \n"
    HELP="yes"
fi

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]            \n" $0
    printf "Switches:                \n"
    printf "  --archie, -a           archie vehicle only                   \n"
    printf "  --betty, -b            betty vehicle only                    \n"
    printf "  --charlie, -c          betty vehicle only                    \n"
    printf "  --davis, -d            betty vehicle only                    \n"
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    exit 0;
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------

CRUISESPEED="1.8"


VNAME1="archie"  # The archie vehicle Community
VPORT1="9100"
LPORT1="9101"
START_POS1="0,-20"
LOITER_PT1="x=-10,y=-60"
RETURN_PT1="-10,-60"

VNAME2="betty"  # The betty vehicle Community
VPORT2="9200"
LPORT2="9201"
START_POS2="50,0"
LOITER_PT2="x=50,y=-40"
RETURN_PT2="50,-40"

VNAME3="charlie"  # The charlie vehicle Community
VPORT3="9300"
LPORT3="9301"
START_POS3="0,-50"
LOITER_PT3="x=-10,y=-90"
RETURN_PT3="-10,-90"

VNAME4="davis"  # The davis vehicle Community
VPORT4="9400"
LPORT4="9401"
START_POS4="50,-30"
LOITER_PT4="x=50,y=-70"
RETURN_PT4="50,-70"

# Conditionally Prepare Archie files
if [ "${ARCHIE}" = "yes" ]; then
    nsplug meta_vehicle_fld.moos targ_archie.moos -f      \
      VNAME=$VNAME1 VPORT=$VPORT1 LPORT=$LPORT1 WARP=$WARP     

    nsplug meta_vehicle.bhv targ_archie.bhv -f            \
        VNAME=$VNAME1                                     \
	CRUISESPEED=$CRUISESPEED                          \
        RETURN_PT=$RETURN_PT1                             \
        LOITER_PT=$LOITER_PT1
fi

# Conditionally Prepare Betty files
if [ "${BETTY}" = "yes" ]; then
    nsplug meta_vehicle_fld.moos targ_betty.moos -f       \
      VNAME=$VNAME2 VPORT=$VPORT2 LPORT=$LPORT2 WARP=$WARP     

    nsplug meta_vehicle.bhv targ_betty.bhv -f             \
            VNAME=$VNAME2                                 \
      CRUISESPEED=$CRUISESPEED                            \
        RETURN_PT=$RETURN_PT2                             \
        LOITER_PT=$LOITER_PT2
fi

# Conditionally Prepare Charlie files
if [ "${CHARLIE}" = "yes" ]; then
    nsplug meta_vehicle_fld.moos targ_charlie.moos -f      \
      VNAME=$VNAME3 VPORT=$VPORT3 LPORT=$LPORT3 WARP=$WARP     

    nsplug meta_vehicle.bhv targ_charlie.bhv -f            \
        VNAME=$VNAME3                                     \
	CRUISESPEED=$CRUISESPEED                          \
        RETURN_PT=$RETURN_PT3                             \
        LOITER_PT=$LOITER_PT3
fi

# Conditionally Prepare Davis files
if [ "${DAVIS}" = "yes" ]; then
    nsplug meta_vehicle_fld.moos targ_davis.moos -f       \
      VNAME=$VNAME4 VPORT=$VPORT4 LPORT=$LPORT4 WARP=$WARP     

    nsplug meta_vehicle.bhv targ_davis.bhv -f             \
            VNAME=$VNAME4                                 \
      CRUISESPEED=$CRUISESPEED                            \
        RETURN_PT=$RETURN_PT4                             \
        LOITER_PT=$LOITER_PT4
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
if [ "${CHARLIE}" = "yes" ]; then
    printf "Launching Charlie MOOS Community \n"
    pAntler targ_charlie.moos >& /dev/null &
fi

# Launch Davis
if [ "${DAVIS}" = "yes" ]; then
    printf "Launching Davis MOOS Community \n"
    pAntler targ_davis.moos >& /dev/null &
fi
#-------------------------------------------------------
#  Part 5: Exiting and/or killing the simulation
#-------------------------------------------------------

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

