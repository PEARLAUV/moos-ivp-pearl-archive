#!/bin/bash -e
COMMUNITY="m200Emulator"
PORT=8999
HELP="no"
JUST_BUILD="no"
TIME_WARP=1
VISUALS="// "
VISUAL_STATUS="WITHOUT"

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    UNDEFINED_ARG=$ARGI
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	    printf "%s [SWITCHES]\n" $0
	    printf "  --visuals, -V \n"
	    printf "  --just_build, -j \n"
	    printf "  --help, -h \n" 
	    exit 0;
	fi
    if [ "${ARGI}" = "--visuals" -o "${ARGI}" = "-V" ]; then 
        VISUALS="   "
        VISUAL_STATUS="WITH"
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
    printf "  --evan, -e             evan vehicle only                     \n"
    printf "  --felix, -f            felix vehicle only                    \n"
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    exit 0;
fi

#-------------------------------------------------------
#  Part 3: Create the .moos file 
#-------------------------------------------------------

printf "Assembling MOOS file targ_m200_emulator.moos ${VISUAL_STATUS} pMarineViewer visuals.\n"

nsplug meta_m200_emulator.moos targ_m200_emulator.moos -f        \
    PORT=$PORT VISUALS=$VISUALS

if [ ${JUST_BUILD} = "yes" ] ; then
    printf "File assembled; emulator not launched; exiting per request.\n"
    exit 0
fi

#-------------------------------------------------------
#  Part 2: Launch the processes
#-------------------------------------------------------
printf "Launching the %s.moos emulator community %s pMarineViewer visuals.\n"  $COMMUNITY $VISUAL_STATUS

pAntler targ_m200_emulator.moos --MOOSTimeWarp=$TIME_WARP >& /dev/null &

uMAC targ_m200_emulator.moos

printf "Killing all processes ... \n"
kill %1 
mykill
printf "Done killing processes.   \n"

