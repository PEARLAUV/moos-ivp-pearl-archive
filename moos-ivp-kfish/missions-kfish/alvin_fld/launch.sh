#!/bin/bash 

HELP="no"
BAD_ARGS=""

#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
let COUNT=0
for ARGI; do
    UNDEFINED_ARG=$ARGI
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	HELP="yes"
	UNDEFINED_ARG=""
    fi
    if [ "${UNDEFINED_ARG}" != "" ] ; then
	BAD_ARGS=$UNDEFINED_ARG
    fi
done

if [ "${BAD_ARGS}" != "" ] ; then
    printf "Bad Argument: %s \n" $BAD_ARGS
    exit 0
fi

if [ "${HELP}" = "yes" ]; then
    printf "%s [SWITCHES]                                   \n" $0
    printf "Switches:                                       \n" 
    printf "  --help, -h                                    \n" 
    exit 0;
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------

printf "Launching ... \n"
pAntler targ_betty.moos >& /dev/null &
printf "Done Launching... \n"


#-------------------------------------------------------
#  Part 4: Exiting and/or killing the run
#-------------------------------------------------------

ANSWER="0"
while [ "${ANSWER}" != "1" -a "${ANSWER}" != "2" ]; do
    printf "Now what? (1) Exit script (2) Exit and Kill the run \n"
    printf "> "
    read ANSWER
done

# %1 matches the PID of the first job in the active jobs list,
# namely the pAntler job launched in Part 3.
if [ "${ANSWER}" = "2" ]; then
    printf "Killing all processes ... \n"
    kill %1
    printf "Done killing processes.   \n"
fi

exit 0
