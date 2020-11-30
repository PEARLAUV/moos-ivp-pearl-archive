 !/bin/bash 

THRUST=60
WARP=1
HELP="no"
JUST_BUILD="no"

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    UNDEFINED_ARG=$ARGI
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
				HELP="yes"
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
				JUST_BUILD="yes"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" ]; then 
        THRUST="${ARGI}"
    else 
				printf "Bad Argument: %s \n" $ARGI
				exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Handle command-line arguments
#-------------------------------------------------------


if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]            \n" $0
    printf "Switches:                \n"
    printf "  --just_build, -j       \n" 
    printf "  --help, -h             \n" 
    exit 0;
fi



read -p "Battery serial number: " SERIAL

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------

VNAME="betty"
VPORT="9100"
SHARE_LISTEN="9101"

#Define a basename so we don't have to change 2 nsplug args and one
#pantler arg if we change the filename
BASENAME="vehicle"

nsplug meta_$BASENAME.moos targ_$BASENAME.moos -f     \
    --path="/home/student/vehicleConfig"              \
    VNAME=$VNAME                                      \
    VPORT=$VPORT                                      \
    THRUST=$THRUST SERIAL=$SERIAL

#If nsplug fails, don't want to continue the launch
if [ $? != 0 ]; then # "$?" is "last exit code"
		exit 1
fi

if [ ${JUST_BUILD} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------

pAntler targ_$BASENAME.moos >& /dev/null &

uMAC targ_$BASENAME.moos

printf "Killing all processes ... \n"
kill %1 %2
printf "Done killing processes.   \n"
