#!/bin/bash 
#-------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------
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

#-------------------------------------------------------
#  Part 2: Create the .moos and .bhv files. 
#-------------------------------------------------------
VNAME1="archie"      # The first   vehicle community
VNAME2="betty"       # The second  vehicle community
VNAME3="charlie"     # The third vehicle community
VNAME4="davis"       # The fourth vehicle community
VNAME5="evan"        # The fifth vehicle community
VNAME6="felix"        # The sixth vehicle community
VPORT1="9001"
VPORT2="9002"
VPORT3="9003"
VPORT4="9004"
VPORT5="9005"
VPORT6="9006"
SHARE_LISTEN1="9301"
SHARE_LISTEN2="9302"
SHARE_LISTEN3="9303"
SHARE_LISTEN4="9304"
SHARE_LISTEN5="9305"
SHARE_LISTEN6="9306"
SHORE_LISTEN="9300"
SHORESIDE_PORT="9000"
START_POS1="28,0"  
START_POS2="30,0"
START_POS3="40,0"
START_POS4="50,0"   
START_POS5="60,0"
START_POS6="70,0" 
SPEED1="1.5"
SPEED2="1.5"
SPEED3="1.5"
SPEED4="1.5"
SPEED5="1.5"
SPEED6="1.5"

#SHORE="multicast_8"  What is this anyway?
#because MULTICAST defined previosly as multicast_9 in shoreside file
#let's try MULTICAST
MULTICAST1="multicast_8"

# What is nsplug? Type "nsplug --help" or "nsplug --manual"

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
   SNAME="shoreside" SHARE_LISTEN=$SHORE_LISTEN SPORT=$SHORESIDE_PORT  \
   MULTICAST=$MULTICAST1

#--------------------------------
nsplug meta_vehicle_sim.moos targ_veh_sim_$VNAME1.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP  VTYPE=UUV \
   VNAME=$VNAME1      START_POS=$START_POS1                \
   VPORT=$VPORT1       SHARE_LISTEN=$SHARE_LISTEN1 SHORE_LISTEN=$SHORE_LISTEN  \
   MULTICAST=$MULTICAST1

nsplug meta_brains.moos targ_brains_$VNAME1.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP VTYPE=UUV \
   VNAME=$VNAME1  START_POS=$START_POS1 \
   VPORT=$VPORT1 SHARE_LISTEN=$SHARE_LISTEN1 SHORE_LISTEN=$SHORE_LISTEN \
   MULTICAST=$MULTICAST1

nsplug meta_vehicle.bhv targ_$VNAME1.bhv -f VNAME=$VNAME1     \
    START_POS=$START_POS1   SPEED=$SPEED1 ORDER="normal"

#--------------------------------
nsplug meta_vehicle_sim.moos targ_veh_sim_$VNAME2.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP  VTYPE=UUV \
   VNAME=$VNAME2      START_POS=$START_POS2                \
   VPORT=$VPORT2       SHARE_LISTEN=$SHARE_LISTEN2 SHORE_LISTEN=$SHORE_LISTEN  \
   MULTICAST=$MULTICAST1

nsplug meta_brains.moos targ_brains_$VNAME2.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP VTYPE=UUV \
   VNAME=$VNAME2  START_POS=$START_POS2 \
   VPORT=$VPORT2 SHARE_LISTEN=$SHARE_LISTEN2 SHORE_LISTEN=$SHORE_LISTEN \
   MULTICAST=$MULTICAST1

nsplug meta_vehicle.bhv targ_$VNAME2.bhv -f VNAME=$VNAME2     \
    START_POS=$START_POS2   SPEED=$SPEED2 ORDER="normal"

#--------------------------------
nsplug meta_vehicle_sim.moos targ_veh_sim_$VNAME3.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP  VTYPE=UUV \
   VNAME=$VNAME3      START_POS=$START_POS3                \
   VPORT=$VPORT3       SHARE_LISTEN=$SHARE_LISTEN3 SHORE_LISTEN=$SHORE_LISTEN  \
   MULTICAST=$MULTICAST1

nsplug meta_brains.moos targ_brains_$VNAME3.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP VTYPE=UUV \
   VNAME=$VNAME3  START_POS=$START_POS3 \
   VPORT=$VPORT3 SHARE_LISTEN=$SHARE_LISTEN3 SHORE_LISTEN=$SHORE_LISTEN \
   MULTICAST=$MULTICAST1

nsplug meta_vehicle.bhv targ_$VNAME3.bhv -f VNAME=$VNAME3     \
    START_POS=$START_POS3   SPEED=$SPEED3 ORDER="reverse"

#--------------------------------
nsplug meta_vehicle_sim.moos targ_veh_sim_$VNAME4.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP  VTYPE=UUV \
   VNAME=$VNAME4      START_POS=$START_POS4                \
   VPORT=$VPORT4       SHARE_LISTEN=$SHARE_LISTEN4 SHORE_LISTEN=$SHORE_LISTEN  \
   MULTICAST=$MULTICAST1

nsplug meta_brains.moos targ_brains_$VNAME4.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP VTYPE=UUV \
   VNAME=$VNAME4  START_POS=$START_POS4 \
   VPORT=$VPORT4 SHARE_LISTEN=$SHARE_LISTEN4 SHORE_LISTEN=$SHORE_LISTEN \
   MULTICAST=$MULTICAST1

nsplug meta_vehicle.bhv targ_$VNAME4.bhv -f VNAME=$VNAME4     \
    START_POS=$START_POS4   SPEED=$SPEED4 ORDER="reverse"

#--------------------------------
nsplug meta_vehicle_sim.moos targ_veh_sim_$VNAME5.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP  VTYPE=UUV \
   VNAME=$VNAME5      START_POS=$START_POS5                \
   VPORT=$VPORT5       SHARE_LISTEN=$SHARE_LISTEN5 SHORE_LISTEN=$SHORE_LISTEN  \
   MULTICAST=$MULTICAST1

nsplug meta_brains.moos targ_brains_$VNAME5.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP VTYPE=UUV \
   VNAME=$VNAME5  START_POS=$START_POS5 \
   VPORT=$VPORT5 SHARE_LISTEN=$SHARE_LISTEN5 SHORE_LISTEN=$SHORE_LISTEN \
   MULTICAST=$MULTICAST1

nsplug meta_vehicle.bhv targ_$VNAME5.bhv -f VNAME=$VNAME5     \
    START_POS=$START_POS5   SPEED=$SPEED5 ORDER="normal"

#--------------------------------
nsplug meta_vehicle_sim.moos targ_veh_sim_$VNAME6.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP  VTYPE=UUV \
   VNAME=$VNAME6      START_POS=$START_POS5                \
   VPORT=$VPORT6       SHARE_LISTEN=$SHARE_LISTEN6 SHORE_LISTEN=$SHORE_LISTEN  \
   MULTICAST=$MULTICAST1

nsplug meta_brains.moos targ_brains_$VNAME6.moos -f -p ../../sim_plugs/ WARP=$TIME_WARP VTYPE=UUV \
   VNAME=$VNAME6  START_POS=$START_POS6 \
   VPORT=$VPORT6 SHARE_LISTEN=$SHARE_LISTEN6 SHORE_LISTEN=$SHORE_LISTEN \
   MULTICAST=$MULTICAST1

nsplug meta_vehicle.bhv targ_$VNAME6.bhv -f VNAME=$VNAME5     \
    START_POS=$START_POS6   SPEED=$SPEED6 ORDER="normal"

if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME1 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_brains_$VNAME1.moos >& /dev/null &
sleep .25
pAntler targ_veh_sim_$VNAME1.moos >& /dev/null &
printf "Launching $VNAME2 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_brains_$VNAME2.moos >& /dev/null &
sleep .25
pAntler targ_veh_sim_$VNAME2.moos >& /dev/null &
sleep .25
printf "Launching $VNAME3 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_brains_$VNAME3.moos >& /dev/null &
sleep .25
pAntler targ_veh_sim_$VNAME3.moos >& /dev/null &
sleep .25
printf "Launching $VNAME4 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_brains_$VNAME4.moos >& /dev/null &
sleep .25
pAntler targ_veh_sim_$VNAME4.moos >& /dev/null &
sleep .25
printf "Launching $VNAME5 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_brains_$VNAME5.moos >& /dev/null &
sleep .25
pAntler targ_veh_sim_$VNAME5.moos >& /dev/null &
sleep .25
printf "Launching $VNAME6 MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_brains_$VNAME6.moos >& /dev/null &
sleep .25
pAntler targ_veh_sim_$VNAME6.moos >& /dev/null &
sleep .25
printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
printf "Done \n"

uMAC targ_shoreside.moos
#sleep 50

printf "Killing all processes ... \n"
mykill
kill %1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13
printf "Done killing processes.   \n"


